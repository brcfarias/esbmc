add_subdirectory(abstract_includes)
add_library(clangcppfrontend clang_cpp_language.cpp
            clang_cpp_convert.cpp clang_cpp_adjust_expr.cpp clang_cpp_adjust_code.cpp
            clang_cpp_adjust_code_gen.cpp clang_cpp_convert_vft.cpp clang_cpp_convert_bind.cpp clang_cpp_main.cpp)
target_include_directories(clangcppfrontend
    PRIVATE ${CLANG_INCLUDE_DIRS}
    PRIVATE ${Boost_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(esbmc_internal_cpp esbmc_internal_cpp.cpp)
target_include_directories(esbmc_internal_cpp
    PRIVATE ${Boost_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(clangcppfrontend PUBLIC BOOST_ALL_NO_LIB)
target_link_libraries(esbmc_internal_cpp cpp_includes)

target_link_libraries(clangcppfrontend esbmc_internal_cpp fmt::fmt)
