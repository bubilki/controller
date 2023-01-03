cmake_minimum_required(VERSION 3.20)
project(controller)

set(CMAKE_CXX_STANDARD 14)

if(UNIX)
	message(STATUS ">>> Linux")
	message("kuku ${PROJECT_SOURCE_DIR}")
	include_directories(${PROJECT_SOURCE_DIR}/include_linux)
	link_libraries(${PROJECT_SOURCE_DIR}/libs_static_linux/libpaho-mqtt3a.a
		${PROJECT_SOURCE_DIR}/libs_static_linux/libpaho-mqtt3as.a
		${PROJECT_SOURCE_DIR}/libs_static_linux/libpaho-mqtt3c.a
		${PROJECT_SOURCE_DIR}/libs_static_linux/libpaho-mqtt3cs.a
		${PROJECT_SOURCE_DIR}/libs_static_linux/libpaho-mqttpp3.a
		${PROJECT_SOURCE_DIR}/libs_shared_linux/libpaho-mqtt3a.so
		${PROJECT_SOURCE_DIR}/libs_shared_linux/libpaho-mqtt3as.so
		${PROJECT_SOURCE_DIR}/libs_shared_linux/libpaho-mqtt3c.so
		${PROJECT_SOURCE_DIR}/libs_shared_linux/libpaho-mqtt3cs.so
		${PROJECT_SOURCE_DIR}/libs_shared_linux/libpaho-mqttpp3.so)

endif(UNIX)

if(APPLE)
	message(STATUS ">>> APPLE")
	include_directories(${PROJECT_SOURCE_DIR}/include)
	link_libraries(${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3a.a
		${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3as.a
		${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3c.a
		${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqtt3cs.a
		${PROJECT_SOURCE_DIR}/libs_static/libpaho-mqttpp3.a)
endif(APPLE)

# set(CMAKE_XCODE_ATTRIBUTE_ARCHS "arm64")
# set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "arm64")


add_executable(${PROJECT_NAME} main.cpp)