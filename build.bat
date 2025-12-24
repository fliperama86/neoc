@echo off
setlocal enabledelayedexpansion

set "PROJECT_DIR=%~dp0"
set "BUILD_TYPE=%~1"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=release"

goto :main

:info
echo [INFO] %*
goto :eof

:error
echo [ERROR] %* 1>&2
goto :eof

:usage
echo Usage: %~nx0 [command]
echo.
echo Commands:
echo     debug       Build debug configuration
echo     release     Build optimized release configuration (default)
echo     clean       Remove build artifacts
echo     help        Show this help message
echo.
echo Examples:
echo     %~nx0              # Build release
echo     %~nx0 debug        # Build debug
echo     %~nx0 clean        # Clean build directory
goto :eof

:check_deps
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    call :error "cmake not found in PATH"
    exit /b 1
)
goto :eof

:build
set "BUILD_DIR=%PROJECT_DIR%build\%~1"

call :info "Building %~1 configuration..."

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if "%~1"=="debug" (
    cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Debug -DNEOC_ENABLE_SANITIZERS=OFF -DNEOC_ENABLE_LTO=OFF
) else (
    cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Release -DNEOC_ENABLE_LTO=ON
)

cmake --build "%BUILD_DIR%" --config %~1 --parallel

call :info "Build complete: %BUILD_DIR%\neoc.exe"
goto :eof

:clean
call :info "Cleaning build artifacts..."
if exist "%PROJECT_DIR%build" rmdir /s /q "%PROJECT_DIR%build"
if exist "%PROJECT_DIR%bin" rmdir /s /q "%PROJECT_DIR%bin"
if exist "%PROJECT_DIR%obj" rmdir /s /q "%PROJECT_DIR%obj"
call :info "Clean complete"
goto :eof

:main
call :check_deps
if %ERRORLEVEL% neq 0 exit /b 1

if /i "%BUILD_TYPE%"=="debug" (
    call :build debug
) else if /i "%BUILD_TYPE%"=="release" (
    call :build release
) else if /i "%BUILD_TYPE%"=="clean" (
    call :clean
) else if /i "%BUILD_TYPE%"=="help" (
    call :usage
) else if /i "%BUILD_TYPE%"=="-h" (
    call :usage
) else if /i "%BUILD_TYPE%"=="--help" (
    call :usage
) else (
    call :error "Unknown command: %BUILD_TYPE%"
    call :usage
    exit /b 1
)

endlocal
