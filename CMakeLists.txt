cmake_minimum_required(VERSION 3.6)
project(ChatFriend LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
find_package(Qt6 COMPONENTS Widgets OpenGL REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 COMPONENTS Multimedia REQUIRED) # 添加qt多媒体模块
find_package(Qt6 COMPONENTS Network REQUIRED) # 添加qt网络模块
#find_library(APPLICATION_SERVICES_FRAMEWORK ApplicationServices)
#
#if (NOT APPLICATION_SERVICES_FRAMEWORK)
#    message(FATAL_ERROR "ApplicationServices framework not found!")
#endif()

add_library(Live2DCubismCore STATIC IMPORTED)
set_target_properties(Live2DCubismCore
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/third/live2d/lib/macos/arm64/libLive2DCubismCore.a
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/third/live2d/inc
)

set(GLEW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third/glew)
set(GLFW_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/third/glfw)
add_subdirectory(${GLEW_PATH}/build/cmake)
add_subdirectory(${GLFW_PATH})
set(FRAMEWORK_SOURCE OpenGL)
add_subdirectory(Framework)
target_compile_definitions(Framework PUBLIC CSM_TARGET_MAC_GL)
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
target_link_libraries(Framework Live2DCubismCore glew_s)
find_package(OpenGL REQUIRED)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCF_DEBUG") # debug模式下定义QF_DEBUG, 用来控制日志输出
include_directories(inc)
include_directories(src)
include_directories(${GLEW_PATH}/include)
include_directories(Framework/src)
include_directories(src/glwidget.h)
include_directories(third/stb)
aux_source_directory(src QF_SOURCES_FILE)

add_executable(ChatFriend)
add_subdirectory(src)
set_target_properties(ChatFriend PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(ChatFriend PRIVATE Qt6::Widgets Qt6::OpenGL glew_s Framework Qt6::OpenGLWidgets "-framework Cocoa" glfw Qt6::Multimedia Qt6::Network )
# 设置编译选项，使得 Qt 库以静态链接方式链接
#target_compile_options(ChatFriend PRIVATE -static-libgcc -static-libstdc++)\
#打包
#set_target_properties(ChatFriend PROPERTIES
#        MACOSX_BUNDLE TRUE
#)
#
#install(TARGETS ChatFriend
#        BUNDLE DESTINATION . COMPONENT Runtime
#        RUNTIME DESTINATION bin COMPONENT Runtime
#)
#执行alias alias otool='/usr/bin/otool'
#alias install_name_tool='/usr/bin/install_name_tool'
#sudo macdeployqt /path/to/ChatFriend.app 千万不要修改bash_profile中的路径（不明原因无法执行与anaconda3有关）

