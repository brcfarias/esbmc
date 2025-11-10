#include <python_class_builder.h>
#include "python_converter.h"
#include "type_utils.h"
#include "json_utils.h"
#include <util/std_types.h>
#include <util/symbol.h>
#include <stdexcept>

void python_class_builder::get(const json &class_node, codet &target_block)
{
  std::string cls = class_node.at("name").get<std::string>();
  std::string id = "tag-" + cls;

  if (conv_.symbol_table().find_symbol(id) != nullptr)
    return;

  locationt loc = conv_.get_location_from_decl(class_node);
  struct_typet clazz;
  clazz.tag(cls);

  symbolt sym =
    conv_.create_symbol(loc.get_file().as_string(), cls, id, loc, clazz);
  sym.is_type = true;
  symbolt *added = conv_.symbol_table().move_symbol_to_context(sym);

  current_class_name_ = cls;

  collect_bases(class_node, clazz);
  collect_members_from_init(class_node, clazz);
  collect_class_level_assigns(class_node, target_block);

  added->type = clazz;
  current_class_name_.clear();
}

void python_class_builder::collect_bases(const json &class_node, struct_typet &clazz)
{
  if (!class_node.contains("bases"))
    return;

  auto &bases = clazz.add("bases").get_sub();
  for (const auto &b : class_node["bases"])
  {
    if (!b.contains("id"))
      continue;
    std::string base = b["id"].get<std::string>();

    if (
      type_utils::is_builtin_type(base) || type_utils::is_consensus_type(base))
      continue;

    symbolt *sym = conv_.symbol_table().find_symbol("tag-" + base);
    if (!sym)
    {
      auto maybe = json_utils::find_class(conv_.ast()["body"], base);
      if (!maybe.is_null())
      {
        codet dummy;
        get(maybe, dummy);
        sym = conv_.symbol_table().find_symbol("tag-" + base);
      }
    }

    if (!sym)
      throw std::runtime_error("Base class not found: " + base);

    bases.emplace_back(sym->id);
  }
}

static bool has_component_named(const struct_typet &t, const irep_idt &name)
{
  for (const auto &c : t.components())
    if (c.get_name() == name)
      return true;
  return false;
}

static struct_typet::componentt build_component(
  const std::string &class_name,
  const std::string &comp_name,
  const typet &type)
{
  struct_typet::componentt c(comp_name, type);
  c.type().set("#member_name", "tag-" + class_name);
  c.set_access("public");
  return c;
}

void python_class_builder::collect_members_from_init(
  const json &class_node,
  struct_typet &clazz)
{
  if (!class_node.contains("body"))
    return;

  for (const auto &item : class_node["body"])
  {
    if (item.value("_type", "") != "FunctionDef")
      continue;
    if (item.value("name", "") != "__init__")
      continue;
    if (!item.contains("body"))
      continue;

    for (const auto &stmt : item["body"])
    {
      // AnnAssign: self.attr: T = ...
      if (
        stmt.value("_type", "") == "AnnAssign" && stmt.contains("target") &&
        stmt["target"].value("_type", "") == "Attribute" &&
        stmt["target"].contains("value") &&
        stmt["target"]["value"].value("id", "") == "self")
      {
        std::string attr = stmt["target"].value("attr", "");
        typet t;
        auto comp = build_component(current_class_name_, attr, t);
        if (!has_component_named(clazz, attr))
          clazz.components().push_back(comp);
      }

      // Assign: self.attr = <expr>
      if (
        stmt.value("_type", "") == "Assign" && stmt.contains("targets") &&
        !stmt["targets"].empty() &&
        stmt["targets"][0].value("_type", "") == "Attribute" &&
        stmt["targets"][0]["value"].value("id", "") == "self")
      {
        std::string attr = stmt["targets"][0].value("attr", "");
        typet t = typet(/*ID_empty*/); // TODO: any_type()
        auto comp = build_component(current_class_name_, attr, t);
        if (!has_component_named(clazz, attr))
          clazz.components().push_back(comp);
      }
    }
  }
}

void python_class_builder::collect_class_level_assigns(
  const json &class_node,
  codet &/*target_block*/)
{
  if (!class_node.contains("body"))
    return;

  for (const auto &stmt : class_node["body"])
  {
    if (
      stmt.value("_type", "") == "Assign" && stmt.contains("targets") &&
      !stmt["targets"].empty() &&
      stmt["targets"][0].value("_type", "") == "Name")
    {
      std::string attr_name = stmt["targets"][0].value("id", "");
      std::string symbol_id = current_class_name_ + "::" + attr_name;
      if (auto *sym = conv_.symbol_table().find_symbol(symbol_id))
      {
        sym->static_lifetime = true;
      }
    }
  }
}
