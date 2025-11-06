@echo off
REM FeEGELib 测试编译和运行脚本
REM 适用于 Windows 环境

echo ========================================
echo FeEGELib 测试套件
echo ========================================
echo.

REM 创建 build 目录
if not exist build (
    echo 创建 build 目录...
    mkdir build
)

cd build

REM 配置 CMake
echo.
echo 配置 CMake...
cmake .. -G "MinGW Makefiles"
if errorlevel 1 (
    echo CMake 配置失败！
    pause
    exit /b 1
)

REM 编译
echo.
echo 编译测试程序...
cmake --build .
if errorlevel 1 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo 编译成功！测试程序位于 build\bin\ 目录
echo ========================================
echo.

REM 询问是否运行测试
set /p run_tests="是否立即运行所有测试？(Y/N): "
if /i "%run_tests%"=="Y" (
    echo.
    echo 运行测试...
    echo ========================================
    
    echo.
    echo [1/4] 运行 Base 模块测试...
    echo ----------------------------------------
    bin\test_base.exe
    
    echo.
    echo [2/4] 运行 Collision 模块测试...
    echo ----------------------------------------
    bin\test_collision.exe
    
    echo.
    echo [3/4] 运行 Element 基础测试...
    echo ----------------------------------------
    bin\test_element_basic.exe
    
    echo.
    echo [4/5] 运行 Layout 模块测试...
    echo ----------------------------------------
    bin\test_layout.exe
    
    echo.
    echo [5/5] 运行集成测试...
    echo ----------------------------------------
    bin\test_integration.exe
    
    echo.
    echo ========================================
    echo 所有测试完成！
    echo ========================================
)

cd ..
pause
