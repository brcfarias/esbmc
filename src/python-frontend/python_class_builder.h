#include <nlohmann/json.hpp>
#include <util/std_code.h>

class python_converter;
class struct_typet;
using json = nlohmann::json;

class python_class_builder
{
public:
  explicit python_class_builder(python_converter &conv) : conv_(conv)
  {
  }

  void get(const nlohmann::json &class_node, codet &target_block);

private:
  void collect_bases(const json &class_node, struct_typet &clazz);

  void collect_members_from_init(const json &class_node, struct_typet &clazz);

  void collect_class_level_assigns(const json &class_node, codet &target_block);

  python_converter &conv_; // non-owning; lifetime managed by caller
  std::string current_class_name_;
};
