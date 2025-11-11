// python_class_adapter.cpp
#include "python_class_adapter.h"
#include "python_converter.h"
#include <util/python_types.h>
#include <symbol_id.h>
#include <json_utils.h>
#include <util/symbol.h>
#include <type_utils.h>
#include <util/std_code.h>
#include <util/irep.h>
#include <util/expr_util.h>
#include <cassert>

std::string python_class_adapter::leaf(const std::string &dotted)
{
  auto p = dotted.rfind('.');
  return p == std::string::npos ? dotted : dotted.substr(p + 1);
}

symbolt *python_class_adapter::ensure_sym(const std::string &name)
{
  const std::string id = "tag-" + name;
  if (auto *s = conv_.symbol_table_.find_symbol(id))
    return s;

  locationt loc = conv_.get_location_from_decl(cls_);
  std::string mod = loc.get_file().as_string();

  struct_typet inc;
  inc.tag(name);
  inc.incomplete(true);
  symbolt sym = conv_.create_symbol(mod, name, id, loc, inc);
  sym.is_type = true;
  return conv_.symbol_table_.move_symbol_to_context(sym);
}

bool python_class_adapter::bases(struct_typet &ty)
{
  bool has_ud = false;
  auto &ids = ty.add("bases").get_sub();

  for (const auto &bfull : pc_.bases())
  {
    std::string base = leaf(bfull);
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

void python_class_adapter::members(struct_typet &ty, codet &out)
{
  for (const auto &n : cls_.at("body"))
  {
    const std::string kind = n.value("_type", "");
    if (kind == "FunctionDef")
    {
      std::string mname = n.value("name", "");
      if (mname == "__init__")
        mname = conv_.current_class_name_;

      conv_.current_func_name_ = mname;
      conv_.get_function_definition(n);

      exprt me = symbol_expr(
        *conv_.symbol_table_.find_symbol(conv_.create_symbol_id().to_string()));
      ty.methods().emplace_back(me.name(), me.type());
      conv_.current_func_name_.clear();
    }
    else if (kind == "AnnAssign")
    {
      // class-level annotated attribute
      const std::string &ann = n["annotation"]["id"];
      if (!conv_.symbol_table_.find_symbol("tag-" + ann))
      {
        auto ref = json_utils::find_class((*conv_.ast_json)["body"], ann);
        if (!ref.empty())
        {
          auto save = conv_.current_class_name_;
          python_class_adapter(conv_, ref).convert(out); // conversão recursiva
          conv_.current_class_name_ = save;
        }
      }
      conv_.get_var_assign(n, out);

      symbol_id sid = conv_.create_symbol_id();
      sid.set_object(n["target"]["id"].get<std::string>());
      auto *sym = conv_.symbol_table_.find_symbol(sid.to_string());
      if (!sym)
        throw std::runtime_error("Class attribute not found");
      sym->static_lifetime = true;
    }
  }
}

void python_class_adapter::add_self_attrs(struct_typet &ty) {
  // Extract instance attributes (e.g., self.x = ...) from each method body
  for (const auto &n : cls_.at("body"))
    if (n.value("_type", "") == "FunctionDef")
      conv_.get_attributes_from_self(n.at("body"), ty);
}


void python_class_adapter::gen_ctor(bool has_ud_base, struct_typet &st)
{
  const bool has_init = pc_.methods().count("__init__") > 0;
  if (has_init || has_ud_base)
    return;

  code_typet f;
  f.return_type() = none_type();

  code_typet::argumentt self;
  self.type() = gen_pointer_type(st);
  self.cmt_base_name("self");
  f.arguments().push_back(self);

  locationt loc = conv_.get_location_from_decl(cls_);
  std::string mod = loc.get_file().as_string();

  symbol_id sid;
  sid.set_filename(mod);
  sid.set_class(conv_.current_class_name_);
  sid.set_function(conv_.current_class_name_);

  symbolt ctor = conv_.create_symbol(
    mod, conv_.current_class_name_, sid.to_string(), loc, f);
  ctor.value = code_blockt();
  ctor.lvalue = true;

  conv_.symbol_table_.add(ctor);
  st.methods().emplace_back(ctor.name, ctor.type);
}

void python_class_adapter::convert(codet &out)
{
  // garante símbolo incompleto
  symbolt *sym = ensure_sym(pc_.name());
  assert(sym && sym->is_type);

  if (!sym->type.incomplete())
    return; // já convertido/convertendo

  sym->type.remove(irept::a_incomplete);

  struct_typet st;
  conv_.current_class_name_ = pc_.name();
  st.tag(conv_.current_class_name_);

  const bool has_ud_base = bases(st);
  add_self_attrs(st);

  // commit parcial (permite lookups recursivos)
  sym->type = st;

  members(st, out);
  gen_ctor(has_ud_base, st);

  sym->type = st;
  conv_.current_class_name_.clear();
}
