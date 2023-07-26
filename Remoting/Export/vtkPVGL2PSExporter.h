// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation, Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkPVGL2PSExporter
 * @brief   ParaView wrapper for vtkGL2PS exporter.
 *
 * This is used to export ParaView renderings to a variety of vector graphics
 * formats.
 */

#ifndef vtkPVGL2PSExporter_h
#define vtkPVGL2PSExporter_h

#include "vtkOpenGLGL2PSExporter.h"
#include "vtkRemotingExportModule.h" // needed for export macro

class VTKREMOTINGEXPORT_EXPORT vtkPVGL2PSExporter : public vtkOpenGLGL2PSExporter
{
public:
  static vtkPVGL2PSExporter* New();
  vtkTypeMacro(vtkPVGL2PSExporter, vtkOpenGLGL2PSExporter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the name of the output file.
   */
  vtkSetMacro(FileName, const char*) const char* GetFileName() { return this->FileName.c_str(); }
  ///@}

  ///@{
  /**
   * If Write3DPropsAsRasterImage is true, add all instances of
   * vtkCubeAxesActors to the RenderExclusions.
   */
  vtkSetMacro(ExcludeCubeAxesActorsFromRasterization, int);
  vtkGetMacro(ExcludeCubeAxesActorsFromRasterization, int);
  vtkBooleanMacro(ExcludeCubeAxesActorsFromRasterization, int);
  ///@}

protected:
  vtkPVGL2PSExporter();
  ~vtkPVGL2PSExporter() override;

  void WriteData() override;

  std::string FileName;
  int ExcludeCubeAxesActorsFromRasterization;

private:
  vtkPVGL2PSExporter(const vtkPVGL2PSExporter&) = delete;
  void operator=(const vtkPVGL2PSExporter&) = delete;
};

#endif
