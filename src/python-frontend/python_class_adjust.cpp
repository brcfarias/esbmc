#include "python_class_adjust.h"
#include "python-frontend/json_utils.h"

python_class_adjust::python_class_adjust(nlohmann::json &ast_) : ast(ast_)
{
}

bool has_init(const nlohmann::json &class_)
{
  for (auto &body_elem : class_["body"])
  {
    if (body_elem["_type"] == "FunctionDef" && body_elem["name"] == "__init__")
      return true;
  }
  return false;
}

/* Copy __init__ from base class
 */
void python_class_adjust::add_constructor(nlohmann::json &class_)
{
  // Remove pass from the class body
  auto pass = std::find_if(
    class_["body"].begin(),
    class_["body"].end(),
    [](const nlohmann::json &elem) { return elem["_type"] == "Pass"; });

  // Check if the element was found
  if (pass != class_["body"].end())
    // Remove the element from the array
    class_["body"].erase(pass);

  if (!has_init(class_))
  {
    /* If __init__ isn't defined, it is copied from first base class of the inheritance list*/
    if (!class_["bases"].empty())
    {
      // Get the first class in the inheritance list
      const std::string &base_class_name = class_["bases"][0]["id"];

      // Find the base class
      nlohmann::json base_class_node =
        json_utils::find_class<nlohmann::json>(ast["body"], base_class_name);
      assert(base_class_node != nlohmann::json());

      // Get the __init__ method from the base class
      nlohmann::json init_func =
        json_utils::find_function(base_class_node["body"], "__init__");
      assert(init_func != nlohmann::json());

      // Copy to the derived class
      nlohmann::json &class_body = class_["body"];
      class_body.insert(class_body.begin(), init_func);
    }
  }
}

void python_class_adjust::adjust()
{
  for (auto &element : ast["body"])
  {
    if (element["_type"] == "ClassDef" && !element["bases"].empty())
    {
      add_constructor(element);
    }
  }
}
