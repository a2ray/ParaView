// SPDX-FileCopyrightText: Copyright (c) Copyright 2021 NVIDIA Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "vtknvindex_receiving_logger.h"

#include <atomic>

#include "vtkOutputWindow.h"
#include "vtkSetGet.h"

#include <nv/index/version.h>

#include "vtknvindex_global_settings.h"

//----------------------------------------------------------------------
vtknvindex_receiving_logger::vtknvindex_receiving_logger() = default;

//----------------------------------------------------------------------
void vtknvindex_receiving_logger::message(mi::base::Message_severity level, const char* category,
  const mi::base::Message_details& /*details*/, const char* message)
{
#ifdef NDEBUG
  if (level >= mi::base::MESSAGE_SEVERITY_DEBUG) // No debug output in an optimized build.
    return;
#endif

  mi::base::Message_severity level_output = mi::base::MESSAGE_SEVERITY_WARNING;
  mi::base::Message_severity level_stdout = mi::base::MESSAGE_SEVERITY_VERBOSE;
  vtknvindex_global_settings* settings = vtknvindex_global_settings::GetInstance();
  if (settings)
  {
    level_output = mi::base::Message_severity(settings->GetLogLevel());
    level_stdout = mi::base::Message_severity(settings->GetLogLevelStandardOutput());
  }

  std::string message_str = message;

  if (message_str.empty() && level > mi::base::MESSAGE_SEVERITY_FATAL)
    return; // no message

  bool use_output_window = (level <= level_output);
  const std::string prefix = "nvindex: ";

  // Customize how some specific log messages are handled
  const std::string category_str = category;
  if (category_str == "TCPNET:NETWORK")
  {
    use_output_window = true; // Always show these in the Output Messages window

#if ((NVIDIA_INDEX_LIBRARY_REVISION_MAJOR > 327600 && NVIDIA_INDEX_LIBRARY_REVISION_MINOR > 0) ||  \
  NVIDIA_INDEX_LIBRARY_REVISION_MAJOR >= 337377)
    const std::string cluster_interface_warning =
      "TCPNET net  warn : The 'any' address can't be used as the cluster interface address.";
#else
    const std::string cluster_interface_warning =
      "TCPNET net  warn : The any address can't be used.";
#endif
    if (message_str.find(cluster_interface_warning) != std::string::npos)
    {
      level = mi::base::MESSAGE_SEVERITY_INFO; // make it less severe

      static bool already = false;
      if (!already)
      {
        already = true;
        message_str = std::string("Note: It is recommended to explicitly set the "
                                  "'cluster_interface_address' option. See the NVIDIA IndeX for "
                                  "ParaView Plugin User's Guide for details.\n") +
          prefix + message_str;
      }
    }
  }
  else if (category_str == "API:MISC")
  {
    // These messages are printed by DiCE during normal startup, skip them
    static std::atomic_int counter(0);
    if (counter == 0 && message_str.rfind("  0.0   API    misc info : Loaded \"", 0) == 0)
    {
      counter++;
      return;
    }
    else if (counter == 1 && message_str.rfind("  0.0   API    misc info : DiCE ", 0) == 0)
    {
      counter++;
      return;
    }
  }

  // This is based on vtkErrorWithObjectMacro()
  if (use_output_window && vtkObject::GetGlobalWarningDisplay())
  {
    // This will pop up the Output Messages window. The messages will also be printed to the
    // console.
    vtkOStreamWrapper::EndlType endl;
    vtkOStreamWrapper::UseEndl(endl);
    vtkOStrStreamWrapper vtkmsg;
    vtkmsg << prefix << message_str << '\n';

    if (level <= mi::base::MESSAGE_SEVERITY_ERROR)
    {
      vtkOutputWindowDisplayErrorText(vtkmsg.str());
    }
    else if (level <= mi::base::MESSAGE_SEVERITY_WARNING)
    {
      vtkOutputWindowDisplayWarningText(vtkmsg.str());
    }
    else
    {
      vtkOutputWindowDisplayText(vtkmsg.str());
    }

    vtkmsg.rdbuf()->freeze(0);
    if (level <= mi::base::MESSAGE_SEVERITY_WARNING)
    {
      vtkObject::BreakOnError();
    }
  }
  else if (level <= level_stdout)
  {
    // Log only to console.
    std::cout << prefix << message_str << std::endl;
  }
}
