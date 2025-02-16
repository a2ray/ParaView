// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqResetScalarRangeReaction.h"
#include "ui_pqResetScalarRangeToDataOverTime.h"

#include "pqActiveObjects.h"
#include "pqCoreUtilities.h"
#include "pqPipelineRepresentation.h"
#include "pqPropertyLinks.h"
#include "pqRescaleRange.h"
#include "pqServerManagerModel.h"
#include "pqTimeKeeper.h"
#include "pqUndoStack.h"
#include "vtkPVDataInformation.h"
#include "vtkSMColorMapEditorHelper.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSessionProxyManager.h"
#include "vtkSMTimeKeeperProxy.h"
#include "vtkSMTransferFunction2DProxy.h"
#include "vtkSMTransferFunctionManager.h"
#include "vtkSMTransferFunctionProxy.h"

#include <QDebug>

namespace
{
vtkSMProxy* lutProxy(pqPipelineRepresentation* repr)
{
  vtkSMProxy* reprProxy = repr ? repr->getProxy() : nullptr;
  if (vtkSMColorMapEditorHelper::GetUsingScalarColoring(reprProxy))
  {
    return vtkSMPropertyHelper(reprProxy, "LookupTable", true).GetAsProxy();
  }
  return nullptr;
}
}

//-----------------------------------------------------------------------------
pqResetScalarRangeReaction::pqResetScalarRangeReaction(
  QAction* parentObject, bool track_active_objects, pqResetScalarRangeReaction::Modes mode)
  : Superclass(parentObject)
  , Mode(mode)
  , Connection(nullptr)
{
  if (track_active_objects)
  {
    QObject::connect(&pqActiveObjects::instance(),
      SIGNAL(representationChanged(pqDataRepresentation*)), this,
      SLOT(setRepresentation(pqDataRepresentation*)));
    this->setRepresentation(pqActiveObjects::instance().activeRepresentation());

    if (this->Mode == TEMPORAL)
    {
      // Get ready to connect timekeepers with the reaction enabled state
      this->Connection = vtkSmartPointer<vtkEventQtSlotConnect>::New();
      pqServerManagerModel* model = pqApplicationCore::instance()->getServerManagerModel();
      this->connect(model, SIGNAL(serverAdded(pqServer*)), SLOT(onServerAdded(pqServer*)));
      this->connect(
        model, SIGNAL(aboutToRemoveServer(pqServer*)), SLOT(onAboutToRemoveServer(pqServer*)));
    }
  }
}

//-----------------------------------------------------------------------------
pqResetScalarRangeReaction::~pqResetScalarRangeReaction()
{
  if (this->Connection)
  {
    this->Connection->Disconnect();
  }
}

//-----------------------------------------------------------------------------
void pqResetScalarRangeReaction::setRepresentation(pqDataRepresentation* repr)
{
  this->Representation = qobject_cast<pqPipelineRepresentation*>(repr);
  this->updateEnableState();
}

//-----------------------------------------------------------------------------
void pqResetScalarRangeReaction::updateEnableState()
{
  bool enabled = this->Representation != nullptr;
  if (enabled && this->Mode == TEMPORAL)
  {
    pqPipelineSource* source = this->Representation->getInput();
    pqTimeKeeper* timeKeeper = source->getServer()->getTimeKeeper();
    enabled =
      (this->Representation->getOutputPortFromInput()->getDataInformation()->GetHasTime() != 0) &&
      vtkSMTimeKeeperProxy::IsTimeSourceTracked(timeKeeper->getProxy(), source->getProxy());
  }
  this->parentAction()->setEnabled(enabled);
}

//-----------------------------------------------------------------------------
void pqResetScalarRangeReaction::onTriggered()
{
  switch (this->Mode)
  {
    case DATA:
      pqResetScalarRangeReaction::resetScalarRangeToData(this->Representation);
      break;

    case CUSTOM:
      pqResetScalarRangeReaction::resetScalarRangeToCustom(this->Representation);
      break;

    case TEMPORAL:
      pqResetScalarRangeReaction::resetScalarRangeToDataOverTime(this->Representation);
      break;

    case VISIBLE:
      pqResetScalarRangeReaction::resetScalarRangeToVisible(this->Representation);
      break;
  }
}

//-----------------------------------------------------------------------------
bool pqResetScalarRangeReaction::resetScalarRangeToData(pqPipelineRepresentation* repr)
{
  if (repr == nullptr)
  {
    repr =
      qobject_cast<pqPipelineRepresentation*>(pqActiveObjects::instance().activeRepresentation());
    if (!repr)
    {
      qCritical() << "No representation provided.";
      return false;
    }
  }

  BEGIN_UNDO_SET(QCoreApplication::translate(
    "pqResetScalarRangeReaction", "Reset transfer function ranges using data range"));
  repr->resetLookupTableScalarRange();
  repr->renderViewEventually();
  if (vtkSMProxy* lut = lutProxy(repr))
  {
    lut->UpdateVTKObjects();
  }
  END_UNDO_SET();
  return true;
}

//-----------------------------------------------------------------------------
bool pqResetScalarRangeReaction::resetScalarRangeToCustom(pqPipelineRepresentation* repr)
{
  if (repr == nullptr)
  {
    repr =
      qobject_cast<pqPipelineRepresentation*>(pqActiveObjects::instance().activeRepresentation());
    if (!repr)
    {
      qCritical() << "No representation provided.";
      return false;
    }
  }

  // See if we should show a separate opacity function range
  bool separateOpacity = false;
  auto proxy = repr->getProxy();
  if (proxy->GetProperty("UseSeparateOpacityArray"))
  {
    vtkSMPropertyHelper helper(proxy, "UseSeparateOpacityArray", true /*quiet*/);
    separateOpacity = helper.GetAsInt() == 1;
  }

  vtkSMProxy* lut = lutProxy(repr);
  if (pqResetScalarRangeReaction::resetScalarRangeToCustom(lut, separateOpacity))
  {
    repr->renderViewEventually();
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool pqResetScalarRangeReaction::resetScalarRangeToCustom(vtkSMProxy* lut, bool separateOpacity)
{
  vtkSMTransferFunctionProxy* tfProxy = vtkSMTransferFunctionProxy::SafeDownCast(lut);
  if (tfProxy)
  {
    double range[2];
    if (!tfProxy->GetRange(range))
    {
      range[0] = 0;
      range[1] = 1.0;
    }

    pqRescaleRange dialog(pqCoreUtilities::mainWidget());
    dialog.setRange(range[0], range[1]);
    dialog.showOpacityControls(separateOpacity);
    vtkSMTransferFunctionProxy* sofProxy = vtkSMTransferFunctionProxy::SafeDownCast(
      vtkSMPropertyHelper(lut, "ScalarOpacityFunction", true).GetAsProxy());
    vtkSMTransferFunction2DProxy* tf2dProxy = vtkSMTransferFunction2DProxy::SafeDownCast(
      vtkSMPropertyHelper(lut, "TransferFunction2D", true).GetAsProxy());
    if (sofProxy && true)
    {
      if (!sofProxy->GetRange(range))
      {
        range[0] = 0;
        range[1] = 1.0;
      }
      dialog.setOpacityRange(range[0], range[1]);
    }
    if (dialog.exec() == QDialog::Accepted)
    {
      BEGIN_UNDO_SET(QCoreApplication::translate(
        "pqResetScalarRangeReaction", "Reset transfer function ranges"));
      range[0] = dialog.minimum();
      range[1] = dialog.maximum();
      tfProxy->RescaleTransferFunction(range[0], range[1]);
      if (sofProxy)
      {
        // If we are using a separate opacity range, get those values from the GUI
        if (separateOpacity)
        {
          range[0] = dialog.opacityMinimum();
          range[1] = dialog.opacityMaximum();
        }
        vtkSMTransferFunctionProxy::RescaleTransferFunction(sofProxy, range[0], range[1]);
      }
      if (tf2dProxy)
      {
        double tf2dRange[4];
        if (!tf2dProxy->GetRange(tf2dRange))
        {
          tf2dRange[1] = 0.0;
          tf2dRange[2] = 1.0;
        }
        tf2dRange[0] = range[0];
        tf2dRange[1] = range[1];
        tf2dProxy->RescaleTransferFunction(tf2dRange);
      }
      // disable auto-rescale of transfer function since the user has set on
      // explicitly (BUG #14371).
      if (dialog.lock())
      {
        vtkSMPropertyHelper(lut, "AutomaticRescaleRangeMode")
          .Set(vtkSMTransferFunctionManager::NEVER);
        lut->UpdateVTKObjects();
      }
      END_UNDO_SET();
      return true;
    }
    return false;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool pqResetScalarRangeReaction::resetScalarRangeToDataOverTime(pqPipelineRepresentation* repr)
{
  if (repr == nullptr)
  {
    repr =
      qobject_cast<pqPipelineRepresentation*>(pqActiveObjects::instance().activeRepresentation());
    if (!repr)
    {
      qCritical() << "No representation provided.";
      return false;
    }
  }

  QDialog dialog(pqCoreUtilities::mainWidget());
  Ui::ResetScalarRangeToDataOverTime ui;
  ui.setupUi(&dialog);

  connect(ui.RescaleButton, &QPushButton::clicked, [&]() { dialog.done(QDialog::Accepted); });
  connect(ui.RescaleAndLockButton, &QPushButton::clicked,
    [&]() { dialog.done(static_cast<int>(QDialog::Accepted) + 1); });
  connect(ui.CancelButton, &QPushButton::clicked, [&]() { dialog.done(QDialog::Rejected); });

  int retcode = dialog.exec();
  if (retcode != QDialog::Rejected)
  {
    BEGIN_UNDO_SET(QCoreApplication::translate(
      "pqResetScalarRangeReaction", "Reset transfer function ranges using temporal data range"));
    vtkSMColorMapEditorHelper::RescaleTransferFunctionToDataRangeOverTime(repr->getProxy());

    // disable auto-rescale of transfer function since the user has set one
    // explicitly (BUG #14371).
    if (retcode == static_cast<int>(QDialog::Accepted) + 1)
    {
      if (vtkSMProxy* lut = lutProxy(repr))
      {
        vtkSMPropertyHelper(lut, "AutomaticRescaleRangeMode")
          .Set(vtkSMTransferFunctionManager::NEVER);
        lut->UpdateVTKObjects();
      }
    }
    repr->renderViewEventually();
    END_UNDO_SET();
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool pqResetScalarRangeReaction::resetScalarRangeToVisible(pqPipelineRepresentation* repr)
{
  if (repr == nullptr)
  {
    repr =
      qobject_cast<pqPipelineRepresentation*>(pqActiveObjects::instance().activeRepresentation());
    if (!repr)
    {
      qCritical() << "No representation provided.";
      return false;
    }
  }

  pqView* view = repr->getView();
  if (!view)
  {
    qCritical() << "No view found.";
    return false;
  }

  BEGIN_UNDO_SET(QCoreApplication::translate(
    "pqResetScalarRangeReaction", "Reset transfer function ranges to visible data range"));
  vtkSMColorMapEditorHelper::RescaleTransferFunctionToVisibleRange(
    repr->getProxy(), view->getProxy());
  repr->renderViewEventually();
  END_UNDO_SET();
  return true;
}

//-----------------------------------------------------------------------------
void pqResetScalarRangeReaction::onServerAdded(pqServer* server)
{
  if (server)
  {
    // Connect new server timekeeper with the reaction enable state
    vtkSMProxy* timeKeeper = server->getTimeKeeper()->getProxy();
    this->Connection->Connect(timeKeeper->GetProperty("SuppressedTimeSources"),
      vtkCommand::ModifiedEvent, this, SLOT(updateEnableState()));
  }
}

//-----------------------------------------------------------------------------
void pqResetScalarRangeReaction::onAboutToRemoveServer(pqServer* server)
{
  if (server)
  {
    // Disconnect previously connected timekeeper
    vtkSMProxy* timeKeeper = server->getTimeKeeper()->getProxy();
    this->Connection->Disconnect(timeKeeper->GetProperty("SuppressedTimeSources"),
      vtkCommand::ModifiedEvent, this, SLOT(updateEnableState()));
  }
}
