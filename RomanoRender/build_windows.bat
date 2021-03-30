rem executable for building the executable on windows

@echo off

set current_dir=%cd%

if exist windows-build\ (
	echo "Removing build directory"
	rmdir /s /q %current_dir%"/windows-build"
) else (
	echo "Creating build directory"
)

mkdir windows-build
cd windows-build

@echo on

cmake ../ 
cmake --build ./ 