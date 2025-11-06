cmake_minimum_required(VERSION 3.14)
project(FeEGEApp)

set(CMAKE_CXX_STANDARD 23)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# 源文件列表
set(LIB_SOURCES
    src/Base.cpp
    src/Widget.cpp
    src/Collision.cpp
    src/Element.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
    src/Layout.cpp
)

# 主应用程序（如果有main.cpp）
if(EXISTS ${CMAKE_SOURCE_DIR}/main.cpp)
    add_executable(FeEGEApp
        main.cpp
        ${LIB_SOURCES}
    )

    target_include_directories(FeEGEApp PRIVATE include)

    # target_compile_definitions(FeEGEApp PRIVATE FPS)
    set_target_properties(FeEGEApp PROPERTIES WIN32_EXECUTABLE TRUE)

    # 链接静态库（graphics.a）
    target_link_libraries(FeEGEApp PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a)

    # 链接系统库，EGE 需要 gdi32（Windows GDI）
    target_link_libraries(FeEGEApp PRIVATE gdi32)
    target_link_libraries(FeEGEApp PRIVATE gdiplus)
    target_link_libraries(FeEGEApp PRIVATE imm32)

    target_compile_options(FeEGEApp PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(FeEGEApp PRIVATE -static-libgcc -static-libstdc++)
endif()

# ==========================================
# 测试程序
# ==========================================

# 测试：Base模块
add_executable(test_base
    test/test_base.cpp
    src/Base.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
)
target_include_directories(test_base PRIVATE include)
target_link_libraries(test_base PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a gdi32 gdiplus imm32)
target_compile_options(test_base PRIVATE -static-libgcc -static-libstdc++)
target_link_options(test_base PRIVATE -static-libgcc -static-libstdc++)

# 测试：Collision模块
add_executable(test_collision
    test/test_collision.cpp
    src/Collision.cpp
    src/Base.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
)
target_include_directories(test_collision PRIVATE include)
target_link_libraries(test_collision PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a gdi32 gdiplus imm32)
target_compile_options(test_collision PRIVATE -static-libgcc -static-libstdc++)
target_link_options(test_collision PRIVATE -static-libgcc -static-libstdc++)

# 测试：Element基础功能
add_executable(test_element_basic
    test/test_element_basic.cpp
    ${LIB_SOURCES}
)
target_include_directories(test_element_basic PRIVATE include)
target_link_libraries(test_element_basic PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a gdi32 gdiplus imm32)
target_compile_options(test_element_basic PRIVATE -static-libgcc -static-libstdc++)
target_link_options(test_element_basic PRIVATE -static-libgcc -static-libstdc++)

# 测试：Layout模块
add_executable(test_layout
    test/test_layout.cpp
    src/Layout.cpp
    src/Base.cpp
    src/font_manager.cpp
    src/sys_edit.cpp
)
target_include_directories(test_layout PRIVATE include)
target_link_libraries(test_layout PRIVATE ${CMAKE_SOURCE_DIR}/lib/libgraphics.a gdi32 gdiplus imm32)
target_compile_options(test_layout PRIVATE -static-libgcc -static-libstdc++)
target_link_options(test_layout PRIVATE -static-libgcc -static-libstdc++)
