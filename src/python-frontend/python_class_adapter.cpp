#include <python_class_adapter.h>
#include "python_converter.h"
#include <type_utils.h>
#include <json_utils.h>
#include <symbol_id.h>
#include <util/irep.h>
#include <util/std_code.h>
#include <util/python_types.h>
#include <cassert>

void python_class_adapter::run(
  const nlohmann::json &class_node,
  codet &target_block)
{
  // === 1. Parse the ClassDef using python_class ===
  python_class cls;
  cls.build(class_node);

  // === 2. Create the class symbol (possibly incomplete) ===
  struct_typet clazz;
  conv_.current_class_name_ = cls.name();
  clazz.tag(conv_.current_class_name_);
  const std::string id = "tag-" + conv_.current_class_name_;

  const locationt location_begin = conv_.get_location_from_decl(class_node);
  const std::string module_name = location_begin.get_file().as_string();

  symbolt *added_symbol = conv_.symbol_table_.find_symbol(id);
  if (!added_symbol)
  {
    // Create an incomplete type first to handle recursive references
    struct_typet incomplete_type;
    incomplete_type.tag(conv_.current_class_name_);
    incomplete_type.incomplete(true);

    symbolt symbol = conv_.create_symbol(
      module_name,
      conv_.current_class_name_,
      id,
      location_begin,
      incomplete_type);
    symbol.is_type = true;

    // Add incomplete type before processing members
    added_symbol = conv_.symbol_table_.move_symbol_to_context(symbol);
  }

  assert(added_symbol && added_symbol->is_type);

  // Skip if already complete
  if (!added_symbol->type.incomplete())
    return;

  // Mark as being processed (remove incomplete flag)
  added_symbol->type.remove(irept::a_incomplete);

  // === 3. Handle base classes ===
  bool has_user_defined_base = false;
  irept::subt &base_ids = clazz.add("bases").get_sub();

  for (const auto &base_full : cls.bases())
  {
    const std::string base_simple = simple_name(base_full);

    // Skip built-in or consensus types
    if (
      type_utils::is_builtin_type(base_simple) ||
      type_utils::is_consensus_type(base_simple))
      continue;

    has_user_defined_base = true;

    symbolt *class_symbol =
      conv_.symbol_table_.find_symbol("tag-" + base_simple);
    if (!class_symbol)
      throw std::runtime_error("Base class not found: " + base_simple);

    // Record the base ID
    base_ids.emplace_back(class_symbol->id);

    // Inherit base components (fields)
    struct_typet &class_type = static_cast<struct_typet &>(class_symbol->type);
    for (const auto &component : class_type.components())
      clazz.components().emplace_back(component);
  }

  // === 4. Extract instance attributes from 'self' (same as before) ===
  for (const auto &class_member : class_node["body"])
    if (class_member["_type"] == "FunctionDef")
      conv_.get_attributes_from_self(class_member["body"], clazz);

  // Commit partial type definition to the symbol
  added_symbol->type = clazz;

  // === 5. Process class members (methods and annotated attributes) ===
  for (const auto &class_member : class_node["body"])
  {
    if (class_member["_type"] == "FunctionDef")
    {
      // Replace "__init__" with the class name (constructor)
      std::string method_name = class_member["name"].get<std::string>();
      if (method_name == "__init__")
        method_name = conv_.current_class_name_;

      conv_.current_func_name_ = method_name;
      conv_.get_function_definition(class_member);

      exprt added_method = symbol_expr(
        *conv_.symbol_table_.find_symbol(conv_.create_symbol_id().to_string()));

      struct_typet::componentt method(added_method.name(), added_method.type());
      clazz.methods().push_back(method);
      conv_.current_func_name_.clear();
    }
    else if (class_member["_type"] == "AnnAssign")
    {
      // Ensure that the annotation type exists in the symbol table
      const std::string &ann_class_name = class_member["annotation"]["id"];
      if (!conv_.symbol_table_.find_symbol("tag-" + ann_class_name))
      {
        // Try to locate and convert the class definition recursively
        const auto class_node_in_ast =
          json_utils::find_class((*conv_.ast_json)["body"], ann_class_name);
        if (!class_node_in_ast.empty())
        {
          const std::string saved = conv_.current_class_name_;
          this->run(class_node_in_ast, target_block);
          conv_.current_class_name_ = saved;
        }
      }

      // Create the variable and its symbol
      conv_.get_var_assign(class_member, target_block);

      symbol_id sid = conv_.create_symbol_id();
      sid.set_object(class_member["target"]["id"].get<std::string>());
      symbolt *class_attr_symbol =
        conv_.symbol_table_.find_symbol(sid.to_string());
      if (!class_attr_symbol)
        throw std::runtime_error("Class attribute not found");

      // Class-level attributes are static
      class_attr_symbol->static_lifetime = true;
    }
  }

  // === 6. Generate a default constructor if necessary ===
  bool has_init = cls.methods().count("__init__") > 0;

  // Generate default constructor only if:
  // 1. There is no __init__ method, and
  // 2. The class has no user-defined base
  if (!has_init && !has_user_defined_base)
  {
    code_typet function_type;
    function_type.return_type() = none_type();

    // Generate a single 'self' parameter
    code_typet::argumentt self_param;
    self_param.type() = gen_pointer_type(clazz);
    self_param.cmt_base_name("self");
    function_type.arguments().push_back(self_param);

    locationt location = conv_.get_location_from_decl(class_node);
    std::string module_name2 = location.get_file().as_string();

    // Create the symbol for the constructor
    symbol_id sid;
    sid.set_filename(module_name2);
    sid.set_class(conv_.current_class_name_);
    sid.set_function(conv_.current_class_name_);

    symbolt constructor_symbol = conv_.create_symbol(
      module_name2,
      conv_.current_class_name_,
      sid.to_string(),
      location,
      function_type);
    constructor_symbol.value = code_blockt(); // Empty body
    constructor_symbol.lvalue = true;

    conv_.symbol_table_.add(constructor_symbol);

    // Add the constructor to the class methods list
    struct_typet::componentt method(
      constructor_symbol.name, constructor_symbol.type);
    clazz.methods().push_back(method);
  }

  // === 7. Finalize and update the complete type ===
  added_symbol->type = clazz;

  conv_.current_class_name_.clear();
}
