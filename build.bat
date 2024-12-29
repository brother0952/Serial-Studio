@echo off
:: 设置 MinGW 和 Qt 环境
set "QT_DIR=D:\Qt6\6.8.1\mingw_64"
set "MINGW_DIR=%QT_DIR%\..\..\Tools\mingw1120_64"
set "CMAKE_PREFIX_PATH=%QT_DIR%\lib\cmake"
set "PATH=%MINGW_DIR%\bin;%QT_DIR%\bin;%PATH%"

:: 设置编译器参数
set "CXXFLAGS=-O2"
set "MAKEFLAGS=-j4"    :: 使用4个线程编译，根据您的CPU核心数调整
set "GC_INITIAL_HEAP_SIZE=64M"
set "GC_MAXIMUM_HEAP_SIZE=2048M"

:: 创建并进入构建目录
if not exist build mkdir build
cd build

:: 运行 CMake
echo Running CMake configuration...
cmake ../ -G "MinGW Makefiles" ^
    -DPRODUCTION_OPTIMIZATION=ON ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT_DIR%" ^
    -DQt6_DIR="%QT_DIR%\lib\cmake\Qt6" ^
    -DQt6Core_DIR="%QT_DIR%\lib\cmake\Qt6Core" ^
    -DQt6Gui_DIR="%QT_DIR%\lib\cmake\Qt6Gui" ^
    -DQt6Widgets_DIR="%QT_DIR%\lib\cmake\Qt6Widgets" ^
    -DQt6Network_DIR="%QT_DIR%\lib\cmake\Qt6Network" ^
    -DCMAKE_CXX_FLAGS="%CXXFLAGS%" ^
    -DCMAKE_CXX_FLAGS_RELEASE="-O2"

if errorlevel 1 (
    echo Error: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

:: 构建项目
echo Building project...
cmake --build . %MAKEFLAGS%

if errorlevel 1 (
    echo Error: Build failed
    cd ..
    pause
    exit /b 1
)

echo Build completed successfully!
cd ..

:: 暂停以查看输出
pause 