cmake_minimum_required(VERSION 3.14)
project(MyEGEApp)

set(CMAKE_CXX_STANDARD 20)

# 添加你的源文件
add_executable(MyEGEApp
    main.cpp
    src/Base.cpp
    src/Widget.cpp
    src/Collision.cpp
    src/Element.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
)

target_include_directories(MyEGEApp PRIVATE include)

# 链接静态库（graphics.a）
target_link_libraries(MyEGEApp PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a)

# 链接系统库，EGE 需要 gdi32（Windows GDI）
target_link_libraries(MyEGEApp PRIVATE gdi32)
target_link_libraries(MyEGEApp PRIVATE gdiplus)
target_link_libraries(MyEGEApp PRIVATE imm32)
