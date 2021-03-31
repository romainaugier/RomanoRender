rem executable for building vcpkg and libraries needed

@echo off

if exist vcpkg\ (
	echo "vcpkg is already installed"
	echo "Installing libraries"
	cd vcpkg
	echo "--------------------------------------------------------"
	echo "Installing embree"
	vcpkg install embree
	echo "--------------------------------------------------------"
	echo "Installing OpenImageIO"
	vcpkg install OpenImageIO
	echo "--------------------------------------------------------"
	echo "Installing Boost"
	vcpkg install Boost
	echo "--------------------------------------------------------"
	echo "Installing OpenColorIO"
	vcpkg install OpenColorIO
	echo "--------------------------------------------------------"
	echo "Libraries installation finished. See log for errors"
) else (
	echo "Installing vcpkg"
	mkdir vcpkg
	cd vcpkg
	git clone https://github.com/microsoft/vcpkg.git
	bootstrap-vcpkg
	echo "--------------------------------------------------------"
	echo "Integrating vcpkg to VS"
	vcpkg integrate install
	echo "--------------------------------------------------------"
	echo "Installing embree"
	vcpkg install embree
	echo "--------------------------------------------------------"
	echo "Installing OpenImageIO"
	vcpkg install OpenImageIO
	echo "--------------------------------------------------------"
	echo "Installing Boost"
	vcpkg install Boost
	echo "--------------------------------------------------------"
	echo "Installing OpenColorIO"
	vcpkg install OpenColorIO
	echo "--------------------------------------------------------"
	echo "Libraries installation finished. See log for errors"
)