#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class python_class
{
public:
  python_class() = default;

  void build(const json &class_def);

  const std::string &methods() const
  {
    return methods_;
  }

  const std::string &attributes() const
  {
    return attrs_;
  }

  const std::vector<python_class> &bases() const
  {
    return bases_;
  }

private:
  std::string methods_;
  std::string attrs_;
  std::vector<python_class> bases_;
};
