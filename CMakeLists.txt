cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(cpp_gen)

find_package(fmt REQUIRED)
find_package(Boost COMPONENTS filesystem program_options REQUIRED)

add_executable(cpp_gen src/main.cc)

target_include_directories(cpp_gen PRIVATE include)
target_include_directories(cpp_gen PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(cpp_gen PRIVATE fmt::fmt)
target_link_libraries(cpp_gen PRIVATE ${Boost_LIBRARIES})

target_compile_features(cpp_gen PRIVATE cxx_std_17)

