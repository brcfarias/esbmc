#pragma once

#include <nlohmann/json.hpp>

class python_class_adjust
{
public:
  python_class_adjust(nlohmann::json &ast_);

  void adjust();

private:
  // Add __init__ method
  void add_constructor(nlohmann::json &class_);

  nlohmann::json &ast;
};
