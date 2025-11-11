#pragma once
#include <nlohmann/json.hpp>
#include <python-frontend/python_class.h>

class python_converter; // forward decl
struct codet;
class symbolt;
class struct_typet;

class python_class_adapter
{
public:
  explicit python_class_adapter(python_converter &conv) : conv_(conv)
  {
  }
  void run(const nlohmann::json &cls_node, codet &out);

private:
  python_converter &conv_;

  // helpers (short + clear)
  static std::string
  leaf(const std::string &dotted); // "pkg.sub.Base" -> "Base"

  symbolt *ensure_sym(const std::string &name, const nlohmann::json &cls_node);

  bool bases(
    const python_class &pc,
    struct_typet &ty); // returns if has user-defined base(s)

  void self_attrs(const nlohmann::json &cls_node, struct_typet &ty);

  void members(const nlohmann::json &cls_node, struct_typet &ty, codet &out);

  void gen_ctor(
    const python_class &pc,
    bool has_ud_base,
    const nlohmann::json &cls_node,
    struct_typet &ty);
};
