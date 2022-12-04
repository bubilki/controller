cmake_minimum_required(VERSION 3.20)
project(controller)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_libraries(${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3a.a ${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3as.a ${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3c.a ${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3cs.a ${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqttpp3.a)
link_libraries(${PROJECT_SOURCE_DIR}/libs_shared)

set(CMAKE_XCODE_ATTRIBUTE_ARCHS "arm64")
set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "arm64")


add_executable(${PROJECT_NAME} main.cpp)