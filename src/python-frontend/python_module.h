#pragma once

#include <string>

class python_module
{
public:
  python_module();

  ~python_module();

  bool is_standard_module(const std::string &module_name) const;

  std::string getReturnType(
    const std::string &module_name,
    const std::string &function_name) const;
};
