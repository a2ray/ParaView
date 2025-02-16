// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkSMMaterialLibraryProxy
 * @brief   proxy for a camera.
 *
 * This a proxy for controlling vtkOSPRayMaterialLibraries on various nodes.
 * In particular we use it to ensure that all rendering processes have
 * a consistent set of materials.
 */

#ifndef vtkSMMaterialLibraryProxy_h
#define vtkSMMaterialLibraryProxy_h

#include "vtkPVSession.h"
#include "vtkRemotingViewsModule.h" //needed for exports
#include "vtkSMProxy.h"

class VTKREMOTINGVIEWS_EXPORT vtkSMMaterialLibraryProxy : public vtkSMProxy
{
public:
  static vtkSMMaterialLibraryProxy* New();
  vtkTypeMacro(vtkSMMaterialLibraryProxy, vtkSMProxy);

  /**
   * Copies the Material library on the root node of server to the client by default.
   * You can also specify the start and end location with \p from and \p to arguments.
   */
  void Synchronize(vtkPVSession::ServerFlags from = vtkPVSession::RENDER_SERVER_ROOT,
    vtkPVSession::ServerFlags to = vtkPVSession::CLIENT);

  /**
   * Reads default materials on the process.
   */
  void LoadDefaultMaterials();

  /**
   * Reads and specified materials.
   */
  void LoadMaterials(const char*);

  /**
   * Overridden to control load from server file system.
   */
  void UpdateVTKObjects() override;

protected:
  vtkSMMaterialLibraryProxy() = default;
  ~vtkSMMaterialLibraryProxy() override = default;

private:
  vtkSMMaterialLibraryProxy(const vtkSMMaterialLibraryProxy&) = delete;
  void operator=(const vtkSMMaterialLibraryProxy&) = delete;
};

#endif
