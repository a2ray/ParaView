// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkPython.h"

#include "pqConsoleWidget.h"

#include "pqPythonShellCompleter.h"

#include <QAbstractItemView>
#include <QStringListModel>

#include "vtkPythonCompatibility.h"
#include "vtkPythonInteractiveInterpreter.h"

pqPythonShellCompleter::pqPythonShellCompleter(
  QWidget* parent, vtkPythonInteractiveInterpreter* interp)
{
  this->Interpreter = interp;
  this->setParent(parent);
}

//----------------------------------------------------------------------------
void pqPythonShellCompleter::updateCompletionModel(const QString& rootText)
{
  // Start by clearing the model
  this->setModel(nullptr);

  // Don't try to complete the empty string
  if (rootText.isEmpty())
  {
    return;
  }

  // Search backward through the string for usable characters
  QString textToComplete;
  for (int i = rootText.length() - 1; i >= 0; --i)
  {
    QChar c = rootText.at(i);
    if (c.isLetterOrNumber() || c == '.' || c == '_')
    {
      textToComplete.prepend(c);
    }
    else
    {
      break;
    }
  }

  // Split the string at the last dot, if one exists
  QString lookup;
  QString compareText = textToComplete;
  int dot = compareText.lastIndexOf('.');
  if (dot != -1)
  {
    lookup = compareText.mid(0, dot);
    compareText = compareText.mid(dot + 1);
  }

  // Lookup python names
  QStringList attrs;
  if (!lookup.isEmpty() || !compareText.isEmpty())
  {
    attrs = this->getPythonAttributes(lookup);
  }

  // Initialize the completion model
  if (!attrs.isEmpty())
  {
    this->setCompletionMode(QCompleter::PopupCompletion);
    this->setModel(new QStringListModel(attrs, this));
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompletionPrefix(compareText.toLower());
    if (this->popup())
    {
      this->popup()->setCurrentIndex(this->completionModel()->index(0, 0));
    }
  }
}

//-----------------------------------------------------------------------------
QStringList pqPythonShellCompleter::getPythonAttributes(const QString& pythonObjectName)
{
  vtkPythonScopeGilEnsurer gilEnsurer;
  if (this->Interpreter == nullptr ||
    this->Interpreter->GetInteractiveConsoleLocalsPyObject() == nullptr)
  {
    return QStringList();
  }

  PyObject* object =
    reinterpret_cast<PyObject*>(this->Interpreter->GetInteractiveConsoleLocalsPyObject());
  Py_INCREF(object);

  if (!pythonObjectName.isEmpty())
  {
    QStringList tmpNames = pythonObjectName.split('.');
    for (int i = 0; i < tmpNames.size() && object; ++i)
    {
      QByteArray tmpName = tmpNames.at(i).toUtf8();
      PyObject* prevObj = object;
      if (PyDict_Check(object))
      {
        object = PyDict_GetItemString(object, tmpName.data());
        Py_XINCREF(object);
      }
      else
      {
        object = PyObject_GetAttrString(object, tmpName.data());
      }
      Py_DECREF(prevObj);
    }
    PyErr_Clear();
  }

  QStringList results;
  if (object)
  {
    PyObject* keys = nullptr;
    bool is_dict = PyDict_Check(object);
    if (is_dict)
    {
      keys = PyDict_Keys(object); // returns *new* reference.
    }
    else
    {
      keys = PyObject_Dir(object); // returns *new* reference.
    }
    if (keys)
    {
      PyObject* key;
      PyObject* value;
      QString keystr;
      int nKeys = PyList_Size(keys);
      for (int i = 0; i < nKeys; ++i)
      {
        key = PyList_GetItem(keys, i);
        if (is_dict)
        {
          value = PyDict_GetItem(object, key); // Return value: Borrowed reference.
          Py_XINCREF(value);                   // so we can use Py_DECREF later.
        }
        else
        {
          value = PyObject_GetAttr(object, key); // Return value: New reference.
        }
        if (!value)
        {
          continue;
        }
        results << PyUnicode_AsUTF8(key);
        Py_DECREF(value);

        // Clear out any errors that may have occurred.
        PyErr_Clear();
      }
      Py_DECREF(keys);
    }
    Py_DECREF(object);
  }
  return results;
}
