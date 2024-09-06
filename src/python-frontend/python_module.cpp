#include <python-frontend/python_module.h>

#include <Python.h>

python_module::python_module()
{
  Py_Initialize();
}

python_module::~python_module()
{
  Py_Finalize();
}

bool python_module::is_standard_module(const std::string &module_name) const
{
  PyObject *moduleName = PyUnicode_FromString(module_name.c_str());
  PyObject *module = PyImport_Import(moduleName);
  bool is_standard = (module != nullptr);

  Py_XDECREF(module);
  Py_XDECREF(moduleName);

  return is_standard;
}

std::string getTypeName(PyObject *obj)
{
  if (PyLong_Check(obj))
    return "int";
  if (PyFloat_Check(obj))
    return "float";
  if (PyUnicode_Check(obj))
    return "str";
  if (PyList_Check(obj))
    return "list";
  if (PyDict_Check(obj))
    return "dict";
  if (PyBool_Check(obj))
    return "bool";
  return "unknown";
}

std::string python_module::getReturnType(
  const std::string &module_name,
  const std::string &function_name) const
{
  PyObject *moduleName = PyUnicode_FromString(module_name.c_str());
  PyObject *module = PyImport_Import(moduleName);
  Py_XDECREF(moduleName);

  if (module == nullptr)
  {
    PyErr_Print();
    return "Failed to load module";
  }

  PyObject *func = PyObject_GetAttrString(module, function_name.c_str());
  Py_XDECREF(module);

  if (func == nullptr || !PyCallable_Check(func))
  {
    PyErr_Print();
    return "Function not found or not callable";
  }

  PyObject *result = PyObject_CallObject(func, nullptr);
  Py_XDECREF(func);

  if (result == nullptr)
  {
    PyErr_Print();
    return "Error calling function";
  }

  std::string return_type = getTypeName(result);
  Py_XDECREF(result);

  return return_type;
}
