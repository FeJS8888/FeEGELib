cmake_minimum_required(VERSION 3.14)
project(FeEGELib)

set(CMAKE_CXX_STANDARD 23)

# 添加 xege 子模块
set(EGE_BUILD_DEMO OFF CACHE BOOL "Do not build xege demos" FORCE)
set(EGE_BUILD_TEST OFF CACHE BOOL "Do not build xege tests" FORCE)
set(EGE_BUILD_TEMP OFF CACHE BOOL "Do not build xege temp" FORCE)
add_subdirectory(3rdparty/xege)

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

# 创建静态库以避免重复编译
add_library(FeEGELib_core STATIC ${LIB_SOURCES})
target_include_directories(FeEGELib_core PUBLIC include)
target_link_libraries(FeEGELib_core PUBLIC xege)
target_link_libraries(FeEGELib_core PUBLIC gdi32 gdiplus imm32)
target_compile_options(FeEGELib_core PRIVATE -static)
target_compile_definitions(FeEGELib_core PRIVATE FPS)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")

# 如果存在main.cpp，编译主程序
if(EXISTS "${CMAKE_SOURCE_DIR}/main.cpp")
    add_executable(FeEGEApp main.cpp)
    target_link_libraries(FeEGEApp PRIVATE FeEGELib_core)
    # set_target_properties(FeEGEApp PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_options(FeEGEApp PRIVATE -static)
endif()

# 查找test目录下的所有.cpp文件
file(GLOB TEST_SOURCES "test/*.cpp")

# 为每个测试文件创建一个可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    # 获取文件名（不含路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # 创建可执行文件，只链接库而不是重新编译所有源文件
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE FeEGELib_core)
    
    # 设置为Windows可执行文件
    set_target_properties(${TEST_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    
    # 链接选项
    target_link_options(${TEST_NAME} PRIVATE -static)
endforeach()
