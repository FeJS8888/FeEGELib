cmake_minimum_required(VERSION 3.14)
project(FeEGEApp)

set(CMAKE_CXX_STANDARD 20)

# target_compile_definitions(FeEGEApp PRIVATE FPS)
# set_target_properties(FeEGEApp PROPERTIES WIN32_EXECUTABLE TRUE)

# 添加你的源文件
add_executable(FeEGEApp
    main.cpp
    src/Base.cpp
    src/Widget.cpp
    src/Collision.cpp
    src/Element.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
    src/Layout.cpp
)

target_include_directories(FeEGEApp PRIVATE include)

# 链接静态库（graphics.a）
target_link_libraries(FeEGEApp PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a)

# 链接系统库，EGE 需要 gdi32（Windows GDI）
target_link_libraries(FeEGEApp PRIVATE gdi32)
target_link_libraries(FeEGEApp PRIVATE gdiplus)
target_link_libraries(FeEGEApp PRIVATE imm32)


target_compile_options(FeEGEApp PRIVATE -static-libgcc -static-libstdc++)
target_link_options(FeEGEApp PRIVATE -static-libgcc -static-libstdc++)
