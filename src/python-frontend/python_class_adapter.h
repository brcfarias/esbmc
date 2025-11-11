#pragma once

#include <nlohmann/json.hpp>
#include <python-frontend/python_class.h>

class python_converter; // fwd
struct codet;

class python_class_adapter {
public:
  explicit python_class_adapter(python_converter &conv) : conv_(conv) {}

  // Convert a ClassDef JSON node into symbols/types using python_class for structure
  void run(const nlohmann::json &class_node, codet &target_block);

private:
  python_converter &conv_;

  // Take the last identifier token (keeps behavior of current implementation)
  static std::string simple_name(const std::string &dotted) {
    auto pos = dotted.rfind('.');
    return pos == std::string::npos ? dotted : dotted.substr(pos + 1);
  }
};
