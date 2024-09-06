#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <python-frontend/python_module.h>.h>

TEST_CASE("Test python_module class", "[python_module]") {
    python_module pm;

    SECTION("Check if a standard module is recognized correctly") {
        std::string standard_module = "os";
        REQUIRE(pm.is_standard_module(standard_module) == true);

        std::string non_standard_module = "my_custom_module";
        REQUIRE(pm.is_standard_module(non_standard_module) == false);
    }

    SECTION("Check return type of a function") {
        std::string module_name = "os";
        std::string function_name = "listdir";
        std::string return_type = pm.getReturnType(module_name, function_name);

        REQUIRE(return_type == "list");
    }
}
