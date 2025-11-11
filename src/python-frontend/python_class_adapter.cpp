#include "python_class_adapter.h"
#include "python_converter.h"
#include <symbol_id.h>
#include <json_utils.h>
#include <util/python_types.h>
#include <util/symbol.h>
#include <type_utils.h>
#include <util/std_code.h>
#include <util/irep.h>

std::string python_class_adapter::leaf(const std::string &dotted)
{
  auto p = dotted.rfind('.');
  return p == std::string::npos ? dotted : dotted.substr(p + 1);
}

symbolt *python_class_adapter::ensure_sym(
  const std::string &name,
  const nlohmann::json &cls_node)
{
  const std::string id = "tag-" + name;
  if (auto *s = conv_.symbol_table_.find_symbol(id))
    return s;

  locationt loc = conv_.get_location_from_decl(cls_node);
  std::string mod = loc.get_file().as_string();

  struct_typet inc;
  inc.tag(name);
  inc.incomplete(true);

  symbolt sym = conv_.create_symbol(mod, name, id, loc, inc);
  sym.is_type = true;
  return conv_.symbol_table_.move_symbol_to_context(sym);
}

bool python_class_adapter::bases(const python_class &pc, struct_typet &ty)
{
  bool has_ud = false;
  auto &ids = ty.add("bases").get_sub();

  for (const auto &b : pc.bases())
  {
    std::string base = leaf(b);
    if (
      type_utils::is_builtin_type(base) || type_utils::is_consensus_type(base))
      continue;

    has_ud = true;
    auto *bsym = conv_.symbol_table_.find_symbol("tag-" + base);
    if (!bsym)
      throw std::runtime_error("Base class not found: " + base);

    ids.emplace_back(bsym->id);

    auto &bty = static_cast<struct_typet &>(bsym->type);
    for (const auto &c : bty.components())
      ty.components().push_back(c);
  }

  return has_ud;
}

void python_class_adapter::self_attrs(
  const nlohmann::json &cls_node,
  struct_typet &ty)
{
  for (const auto &n : cls_node["body"])
    if (n["_type"] == "FunctionDef")
      conv_.get_attributes_from_self(n["body"], ty);
}

void python_class_adapter::members(
  const nlohmann::json &cls_node,
  struct_typet &ty,
  codet &out)
{
  for (const auto &n : cls_node["body"])
  {
    if (n["_type"] == "FunctionDef")
    {
      std::string mname = n["name"].get<std::string>();
      if (mname == "__init__")
        mname = conv_.current_class_name_;

      conv_.current_func_name_ = mname;
      conv_.get_function_definition(n);

      exprt me = symbol_expr(
        *conv_.symbol_table_.find_symbol(conv_.create_symbol_id().to_string()));
      ty.methods().emplace_back(me.name(), me.type());
      conv_.current_func_name_.clear();
    }
    else if (n["_type"] == "AnnAssign")
    {
      const std::string &ann = n["annotation"]["id"];
      if (!conv_.symbol_table_.find_symbol("tag-" + ann))
      {
        auto ref = json_utils::find_class((*conv_.ast_json)["body"], ann);
        if (!ref.empty())
        {
          auto save = conv_.current_class_name_;
          run(ref, out); // recursive conversion of referenced class
          conv_.current_class_name_ = save;
        }
      }

      conv_.get_var_assign(n, out);

      symbol_id sid = conv_.create_symbol_id();
      sid.set_object(n["target"]["id"].get<std::string>());
      auto *sym = conv_.symbol_table_.find_symbol(sid.to_string());
      if (!sym)
        throw std::runtime_error("Class attribute not found");

      sym->static_lifetime = true; // mark as class-level attribute
    }
  }
}

void python_class_adapter::gen_ctor(
  const python_class &pc,
  bool has_ud_base,
  const nlohmann::json &cls_node,
  struct_typet &ty)
{
  if (pc.methods().count("__init__") || has_ud_base)
    return;

  code_typet f;
  f.return_type() = none_type();

  code_typet::argumentt self;
  self.type() = gen_pointer_type(ty);
  self.cmt_base_name("self");
  f.arguments().push_back(self);

  locationt loc = conv_.get_location_from_decl(cls_node);
  std::string mod = loc.get_file().as_string();

  symbol_id sid;
  sid.set_filename(mod);
  sid.set_class(conv_.current_class_name_);
  sid.set_function(conv_.current_class_name_);

  symbolt ctor = conv_.create_symbol(
    mod, conv_.current_class_name_, sid.to_string(), loc, f);
  ctor.value = code_blockt(); // empty body
  ctor.lvalue = true;

  conv_.symbol_table_.add(ctor);
  ty.methods().emplace_back(ctor.name, ctor.type);
}

void python_class_adapter::run(const nlohmann::json &cls_node, codet &out)
{
  // Parse the ClassDef structure
  python_class pc;
  pc.build(cls_node);

  // Ensure (incomplete) class symbol
  symbolt *sym = ensure_sym(pc.name(), cls_node);
  assert(sym && sym->is_type);

  // Prevent re-entrancy / infinite recursion
  if (!sym->type.incomplete())
    return;
  sym->type.remove(irept::a_incomplete);

  // Build struct type
  struct_typet clazz;
  conv_.current_class_name_ = pc.name();
  clazz.tag(conv_.current_class_name_);

  // Bases and instance attrs
  bool has_ud_base = bases(pc, clazz);
  self_attrs(cls_node, clazz);

  // Partial commit (enables recursive lookups for nested types)
  sym->type = clazz;

  // Members and default constructor
  members(cls_node, clazz, out);
  gen_ctor(pc, has_ud_base, cls_node, clazz);

  sym->type = clazz;
  conv_.current_class_name_.clear();
}
