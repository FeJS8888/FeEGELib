cmake_minimum_required(VERSION 3.14)
project(FeEGELib)

set(CMAKE_CXX_STANDARD 23)

# 库源文件
set(LIB_SOURCES
    src/Base.cpp
    src/Widget.cpp
    src/Collision.cpp
    src/Element.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
    src/Layout.cpp
)

# 查找test目录下的所有.cpp文件
file(GLOB TEST_SOURCES "test/*.cpp")

# 为每个测试文件创建一个可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    # 获取文件名（不含路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # 创建可执行文件
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${LIB_SOURCES})
    
    # 包含头文件目录
    target_include_directories(${TEST_NAME} PRIVATE include)
    
    # 设置为Windows可执行文件
    set_target_properties(${TEST_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    
    # 链接静态库（graphics.a）
    target_link_libraries(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a)
    
    # 链接系统库，EGE 需要 gdi32（Windows GDI）
    target_link_libraries(${TEST_NAME} PRIVATE gdi32)
    target_link_libraries(${TEST_NAME} PRIVATE gdiplus)
    target_link_libraries(${TEST_NAME} PRIVATE imm32)
    
    # 编译和链接选项
    target_compile_options(${TEST_NAME} PRIVATE -static)
    target_link_options(${TEST_NAME} PRIVATE -static)
endforeach()
