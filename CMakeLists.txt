cmake_minimum_required(VERSION 3.10)
project(pyAGizmo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr/lib/python3.6" CACHE PATH "..." FORCE)
endif()

if (NOT HEADER_INSTALL_PREFIX)
    set(HEADER_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

include(CTest)

add_executable(BasicTest test/src/basic_test.cpp test/include/basic_test.hpp)
target_include_directories(BasicTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/test/include
)
target_compile_features(BasicTest PRIVATE cxx_std_17)
target_compile_options(BasicTest PRIVATE -g -march=x86-64 -mtune=native -O3 -g0
				       -pipe -fPIE -fPIC -fstack-protector-strong -fno-plt
				       -fvisibility=hidden -Werror -Wall -pthread)

add_test(Test BasicTest)
add_custom_command(TARGET BasicTest
                   POST_BUILD
                   COMMAND "ctest" "--output-on-failure")

add_subdirectory(pybind11)
pybind11_add_module(pyAGizmo src/pyagizmo.cpp)
target_include_directories(pyAGizmo
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_options(pyAGizmo PRIVATE -march=x86-64 -mtune=native -O3 -g0
				       -pipe -fPIE -fPIC -fstack-protector-strong -fno-plt
                       -fvisibility=hidden -Werror -Wall -pthread)
target_compile_features(pyAGizmo PRIVATE cxx_std_17)

install(TARGETS pyAGizmo LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY include/agizmo DESTINATION ${HEADER_INSTALL_PREFIX}/include)


