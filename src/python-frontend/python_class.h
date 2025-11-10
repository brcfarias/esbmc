#pragma once

#include <string>
#include <nlohmann/json.hpp>

class python_class
{
public:
  python_class() = default;

  void build(const nlohmann::json &class_def);

  const std::string &methods() const
  {
    return methods_;
  }

  const std::string &attributes() const
  {
    return attrs_;
  }

  const std::string &bases() const
  {
    return bases_;
  }

private:
  std::string methods_;
  std::string attrs_;
  std::vector<python_class> bases_;
};
