/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPVSource.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 1998-2000 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/

#include "vtkPVSource.h"
#include "vtkKWApplication.h"
#include "vtkPVComposite.h"
#include "vtkKWView.h"
#include "vtkKWRenderView.h"

int vtkPVSourceCommand(ClientData cd, Tcl_Interp *interp,
			   int argc, char *argv[]);

vtkPVSource::vtkPVSource()
{
  this->CommandFunction = vtkPVSourceCommand;
  
  this->Composite = NULL;
  this->DataWidget = NULL;
  this->Input = NULL;
}

vtkPVSource::~vtkPVSource()
{
  this->SetComposite(NULL);
  this->SetDataWidget(NULL);
  if (this->Input)
    {
    this->Input->UnRegister(this);
    this->Input = NULL;
    }
}

vtkPVSource* vtkPVSource::New()
{
  return new vtkPVSource();
}

void vtkPVSource::SetComposite(vtkPVComposite *comp)
{
  if (this->Composite == comp)
    {
    return;
    }
  this->Modified();

  if (this->Composite)
    {
    vtkPVComposite *tmp = this->Composite;
    this->Composite = NULL;
    tmp->UnRegister(this);
    }
  if (comp)
    {
    this->Composite = comp;
    comp->Register(this);
    }
}

void vtkPVSource::SetDataWidget(vtkPVData *data)
{
  if (this->DataWidget == data)
    {
    return;
    }
  this->Modified();

  if (this->DataWidget)
    {
    // extra careful for circular references
    vtkPVData *tmp = this->DataWidget;
    this->DataWidget = NULL;
    // Manage double pointer.
    tmp->SetSourceWidget(NULL);
    tmp->UnRegister(this);
    }
  if (data)
    {
    this->DataWidget = data;
    data->Register(this);
    // Manage double pointer.
    data->SetSourceWidget(this);
    }
}
  
vtkPVData *vtkPVSource::GetDataWidget()
{
  return this->DataWidget;
}


// Data must be set first.  This is OK, because Source will merge with PVComposite ...
void vtkPVSource::SetAssignment(vtkPVAssignment *a)
{
  if (this->GetDataWidget() == NULL)
    {
    vtkErrorMacro("Cannot make assignment.  Output has not been created.");
    return;
    }
  this->GetDataWidget()->SetAssignment(a);
}
