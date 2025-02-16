// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef pqPythonQtWrapperFactory_h
#define pqPythonQtWrapperFactory_h

#include <PythonQtCppWrapperFactory.h>

class pqPythonQtWrapperFactory : public PythonQtForeignWrapperFactory
{
public:
  virtual PyObject* wrap(const QByteArray& classname, void* ptr);
  virtual void* unwrap(const QByteArray& classname, PyObject* object);
};

#endif
