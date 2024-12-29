@echo off
setlocal enabledelayedexpansion

:: 设置变量
set "APP_NAME=Serial-Studio"
set "BUILD_DIR=build"
set "DEPLOY_DIR=deploy"
set "QT_DIR=D:\Qt6\6.8.1\mingw_64"
set "MINGW_DIR=%QT_DIR%\..\..\Tools\mingw1310_64"
set "APP_VERSION=0.0.1"

echo Creating deployment package for %APP_NAME% v%APP_VERSION%...

:: 创建并进入构建目录
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: 设置 MinGW 环境
set "PATH=%MINGW_DIR%\bin;%QT_DIR%\bin;%PATH%"

:: 运行 CMake 构建
echo Building Release version...
cmake ../ -G "MinGW Makefiles" -DPRODUCTION_OPTIMIZATION=ON -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Error: CMake configuration failed
    cd ..
    exit /b 1
)

cmake --build . --parallel
if errorlevel 1 (
    echo Error: Build failed
    cd ..
    exit /b 1
)

:: 创建部署目录
cd ..
if exist %DEPLOY_DIR% rd /s /q %DEPLOY_DIR%
mkdir %DEPLOY_DIR%

:: 复制主程序
echo Copying application files...
copy "%BUILD_DIR%\app\%APP_NAME%.exe" "%DEPLOY_DIR%\"
if errorlevel 1 (
    echo Error: Failed to copy executable
    exit /b 1
)

:: 运行 windeployqt
echo Running windeployqt...
"%QT_DIR%\bin\windeployqt.exe" --release --qmldir app/qml "%DEPLOY_DIR%\%APP_NAME%.exe"
if errorlevel 1 (
    echo Error: windeployqt failed
    exit /b 1
)

:: 复制 MinGW 运行库
echo Copying MinGW runtime libraries...
copy "%MINGW_DIR%\bin\libstdc++-6.dll" "%DEPLOY_DIR%\" || echo Failed to copy libstdc++-6.dll
copy "%MINGW_DIR%\bin\libgcc_s_seh-1.dll" "%DEPLOY_DIR%\" || echo Failed to copy libgcc_s_seh-1.dll
copy "%MINGW_DIR%\bin\libwinpthread-1.dll" "%DEPLOY_DIR%\" || echo Failed to copy libwinpthread-1.dll

:: 复制 OpenSSL DLL
echo Copying OpenSSL libraries...
if exist "lib\OpenSSL\dll\Windows\*.dll" (
    copy "lib\OpenSSL\dll\Windows\*.dll" "%DEPLOY_DIR%\"
) else (
    echo OpenSSL DLLs not found in lib folder, copying from Qt...
    copy "%QT_DIR%\bin\libssl-3-x64.dll" "%DEPLOY_DIR%\" || echo Failed to copy libssl-3-x64.dll
    copy "%QT_DIR%\bin\libcrypto-3-x64.dll" "%DEPLOY_DIR%\" || echo Failed to copy libcrypto-3-x64.dll
)

:: 复制其他必要文件
echo Copying additional files...
copy "LICENSE.md" "%DEPLOY_DIR%\" || echo LICENSE.md not found
copy "README.md" "%DEPLOY_DIR%\" || echo README.md not found

:: 创建版本信息文件
echo v%APP_VERSION% > "%DEPLOY_DIR%\version.txt"

echo.
echo Deployment package created successfully in %DEPLOY_DIR% directory
echo.

:: 打开部署目录
explorer %DEPLOY_DIR%

endlocal 