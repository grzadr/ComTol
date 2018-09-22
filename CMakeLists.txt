cmake_minimum_required(VERSION 3.10)
project(pyComTol VERSION 0.1 LANGUAGES CXX Python)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-march=x86-64 -mtune=native -O3 -g0 -pipe -fPIE -fPIC \
-fstack-protector-strong -fno-plt -fvisibility=hidden -Werror -Wall -pthread")
set(INSTALL_TARGET "/home/adrian")
set(PYBIND11_CPP_STANDARD -std=c++1z)

# )
# target_link_libraries(Verify DOGIToys)
file(GLOB HeaderFiles ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h)
# install(TARGETS DOGIToys ARCHIVE DESTINATION ${INSTALL_TARGET}/lib)
install(FILES ${HeaderFiles} DESTINATION ${INSTALL_TARGET}/lib/hkl)

add_subdirectory(pybind11)

pybind11_add_module(pyComTol pycomtol.cpp)
pybind11_add_module(pyHKL pyhkl.cpp)
# target_link_libraries(pyHKL PRIVATE DOGIToys)


install(TARGETS pyComTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)

# install(TARGETS pyHKL LIBRARY DESTINATION ${INSTALL_TARGET}/lib)
# install(TARGETS pyComTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)
# install(TARGETS pyStrTol LIBRARY DESTINATION ${INSTALL_TARGET}/lib)


# include(CTest)
# add_test(Complete Verify)
