// SPDX-FileCopyrightText: Copyright (c) Copyright 2021 NVIDIA Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "vtknvindex_forwarding_logger.h"

#include <cassert>
#include <cstdio>
#include <iostream>

#include "vtkOutputWindow.h"
#include "vtkSetGet.h"

#include "vtknvindex_global_settings.h"

namespace vtknvindex
{
namespace logger
{

// Singleton instance.
vtknvindex_forwarding_logger_factory*
  vtknvindex_forwarding_logger_factory::G_p_forwarding_logger_factory = nullptr;

//-------------------------------------------------------------------------------------------------
vtknvindex_forwarding_logger::vtknvindex_forwarding_logger()
  : m_level(mi::base::MESSAGE_SEVERITY_INFO)
{
  // The forwarding logger pointer must be captured by a handle,
  // otherwise it leaks some memory.
  if (vtknvindex_forwarding_logger_factory::instance()->is_enabled())
  {
    mi::base::Handle<mi::base::ILogger> forwarding_logger(
      vtknvindex_forwarding_logger_factory::instance()->get_forwarding_logger());
    m_forwarding_logger.swap(forwarding_logger);
    assert(m_forwarding_logger.is_valid_interface());
  }
  else
  {
    // Show local logging.
  }

  std::string const hd = vtknvindex_forwarding_logger_factory::instance()->get_message_header();
  if (!(hd.empty()))
    m_os << hd << " ";
}

//-------------------------------------------------------------------------------------------------
vtknvindex_forwarding_logger::~vtknvindex_forwarding_logger()
{
  m_os << std::endl;

  // Always show the message if it has the level VERBOSE or lower.
  if (m_level <= mi::base::MESSAGE_SEVERITY_VERBOSE)
  {
    if (m_forwarding_logger.is_valid_interface())
    {
      m_forwarding_logger->message(m_level, "PVPLN:MAIN", m_os.str().c_str());
    }
    else
    {
      mi::base::Message_severity level_output = mi::base::MESSAGE_SEVERITY_WARNING;
      mi::base::Message_severity level_stdout = mi::base::MESSAGE_SEVERITY_VERBOSE;
      vtknvindex_global_settings* settings = vtknvindex_global_settings::GetInstance();
      if (settings)
      {
        level_output = mi::base::Message_severity(settings->GetLogLevel());
        level_stdout = mi::base::Message_severity(settings->GetLogLevelStandardOutput());
      }

      // No forwarding logger available (yet)
      if (vtknvindex_forwarding_logger_factory::instance()->get_fallback_log_severity() >= m_level)
      {
        std::string level = level_to_string(m_level);
        while (level.size() < 5)
        {
          level += ' ';
        }

        const std::string prefix = "nvindex: ";

        // This is based on vtkErrorWithObjectMacro().
        if (m_level <= level_output && vtkObject::GetGlobalWarningDisplay())
        {
          // This will pop up the Output Messages window, messages will also be printed to the
          // console.
          vtkOStreamWrapper::EndlType endl;
          vtkOStreamWrapper::UseEndl(endl);
          vtkOStrStreamWrapper vtkmsg;
          vtkmsg << prefix << "PVPLN  init " << level << ": " << m_os.str();

          if (m_level <= mi::base::MESSAGE_SEVERITY_ERROR)
            vtkOutputWindowDisplayErrorText(vtkmsg.str());
          else if (m_level <= mi::base::MESSAGE_SEVERITY_WARNING)
            vtkOutputWindowDisplayWarningText(vtkmsg.str());
          else
            vtkOutputWindowDisplayText(vtkmsg.str());

          vtkmsg.rdbuf()->freeze(0);
          vtkObject::BreakOnError();
        }
        else if (m_level <= level_stdout)
        {
          // Log info level only to console.
          std::cout << prefix << "        PVPLN  init " << level << ": " << m_os.str();
        }
      }
    }
  }
#ifndef NDEBUG
  else
  {
    // Show message with level DEBUG only in debug build.
    const std::string& str = m_os.str(); // for cpp check.

    if (m_forwarding_logger.is_valid_interface())
    {
      m_forwarding_logger->message(m_level, "PVPLN:MAIN", str.c_str());
    }
    else
    {
      // No forwarding logger available.
      if (vtknvindex_forwarding_logger_factory::instance()->get_fallback_log_severity() >= m_level)
      {
        std::string level = level_to_string(m_level);
        while (level.size() < 5)
          level += ' ';

        std::cout << "        PVPLN  init " << level << ": " << m_os.str();
      }
    }
  }
#endif

  // The m_forwarding_logger is destructed and ref-count is down here.
  m_forwarding_logger = nullptr;
}

//-------------------------------------------------------------------------------------------------
std::ostringstream& vtknvindex_forwarding_logger::get_message(mi::base::Message_severity level)
{
  m_level = level;
  return m_os;
}

//-------------------------------------------------------------------------------------------------
std::string vtknvindex_forwarding_logger::level_to_string(mi::base::Message_severity level)
{
  static const char* const g_buffer[] = {
    "fatal",
    "error",
    "warn",
    "info",
    "verbose",
    "debug",
    nullptr,
  };

  assert(0 <= level);
  assert(level <= (mi::base::MESSAGE_SEVERITY_DEBUG));

  return g_buffer[level];
}

//
// vtknvindex_forwarding_logger_factory
//

//-------------------------------------------------------------------------------------------------
vtknvindex_forwarding_logger_factory::vtknvindex_forwarding_logger_factory()
  : m_fallback_severity_level(mi::base::MESSAGE_SEVERITY_INFO)
{
  // empty
}

//-------------------------------------------------------------------------------------------------
vtknvindex_forwarding_logger_factory::~vtknvindex_forwarding_logger_factory()
{
  this->shutdown();
}

//-------------------------------------------------------------------------------------------------
void vtknvindex_forwarding_logger_factory::initialize(
  mi::base::Handle<nv::index::IIndex>& iindex_if)
{
  // This must be initialized once before the first use the
  // forwarding logger.
  assert(iindex_if.is_valid_interface());
  m_iindex_if = iindex_if;
}

//-------------------------------------------------------------------------------------------------
void vtknvindex_forwarding_logger_factory::set_message_header(std::string const& header_str)
{
  m_header_str = header_str;
}

//-------------------------------------------------------------------------------------------------
std::string vtknvindex_forwarding_logger_factory::get_message_header() const
{
  return m_header_str;
}

//-------------------------------------------------------------------------------------------------
bool vtknvindex_forwarding_logger_factory::shutdown()
{
  if (!this->is_enabled())
    return false;

  m_iindex_if = nullptr;
  return true;
}

//-------------------------------------------------------------------------------------------------
bool vtknvindex_forwarding_logger_factory::is_enabled() const
{
  return m_iindex_if.is_valid_interface();
}

//-------------------------------------------------------------------------------------------------
mi::base::ILogger* vtknvindex_forwarding_logger_factory::get_forwarding_logger() const
{
  if (!m_iindex_if.is_valid_interface())
  {
    fprintf(stdout,
      "Error: The forwarding logger factory has not been initialized. Please note "
      "that this may cause to a segmentation fault.\n");
    return nullptr;
  }
  return m_iindex_if->get_forwarding_logger();
}

//-------------------------------------------------------------------------------------------------
bool vtknvindex_forwarding_logger_factory::set_fallback_log_severity(
  mi::base::Message_severity fb_level)
{
  if ((fb_level < mi::base::MESSAGE_SEVERITY_FATAL) ||
    (fb_level > mi::base::MESSAGE_SEVERITY_DEBUG))
  {
    return false;
  }

  m_fallback_severity_level = fb_level;
  return true;
}

//-------------------------------------------------------------------------------------------------
mi::base::Message_severity vtknvindex_forwarding_logger_factory::get_fallback_log_severity() const
{
  return m_fallback_severity_level;
}
}
} // namespace
