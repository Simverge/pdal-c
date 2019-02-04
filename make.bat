:: Helper script for vcpkg-based x64-windows vscode build tasks
@echo off

:: This script assumes that cmake is in PATH
:: TODO Auto-detect latest platform
set SCRIPT_DIR=%~dp0

if not defined BUILD_TYPE set BUILD_TYPE=Release
if not defined ARCH set ARCH=x64
if not defined TARGET_OS set TARGET_OS=windows
set TRIPLET=%ARCH%-%TARGET_OS%

if defined TOOLCHAIN (
	set TOOLCHAIN_ARG=-DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN%
	set TRIPLET_ARG=-DVCPKG_TARGET_TRIPLET=%TRIPLET%
	set BUILD_DIR=%SCRIPT_DIR%\build\%TRIPLET%
) else if defined PREFIX_PATH (
	set PREFIX_PATH_ARG=-DCMAKE_PREFIX_PATH=%PREFIX_PATH%
	set BUILD_DIR=%SCRIPT_DIR%\build\%TRIPLET%-osgeo4w
)

if exist "%BUILD_DIR%\pdal-c.sln" (
	pushd "%BUILD_DIR%"
) else (
	mkdir "%BUILD_DIR%"
	pushd "%BUILD_DIR%"

	cmake ../.. ^
		%PREFIX_PATH_ARG% ^
		%TOOLCHAIN_ARG% ^
		%TRIPLET_ARG% ^
		-DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
		-DCMAKE_GENERATOR_PLATFORM=%ARCH%
)

:: Build and install solution
cmake --build . --target INSTALL --config %BUILD_TYPE%

popd

