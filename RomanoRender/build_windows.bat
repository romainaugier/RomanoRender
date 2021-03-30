rem executable for building the executable on windows


if exist windows-build (
	echo "Removing build directory"
	rmdir windows-build
) else (
	echo "Creating build directory"
)

mkdir windows-build
cd windows-build
cmake ../ 
cmake --build ./ 