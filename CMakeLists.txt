cmake_minimum_required(VERSION 3.18)

project(vdriver LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(VDRIVER_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/vdriver")
set(VDRIVER_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${VDRIVER_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${VDRIVER_SRC_PATH}/*.cpp)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_subdirectory(gitversion)
include(gen)

configure_file(${CMAKE_SOURCE_DIR}/cconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_executable(vdriver
    ${files_cpp}
    ${files_h}
)
target_link_libraries(vdriver Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network gen gitversion)

include(GNUInstallDirs)

install(TARGETS vdriver
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(VERSION_IN ${CMAKE_SOURCE_DIR}/VERSION.in)
set(VERSION_OUT ${CMAKE_SOURCE_DIR}/package/src/VERSION)
configure_file(${VERSION_IN} ${VERSION_OUT} @ONLY)
