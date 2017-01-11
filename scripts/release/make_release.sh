#!/bin/sh
set -e

SRCDIR=$(pwd)
SCRIPTDIR=$SRCDIR/scripts/release
TOOLCHAINDIR=$SCRIPTDIR/toolchains
ENVDIR=$SCRIPTDIR/env
HOSTDIR=$SCRIPTDIR/cmake-build/host/build/release/host

make_host()
{
	mkdir -p $SCRIPTDIR/cmake-build/host
	cd $SCRIPTDIR/cmake-build/host
	cmake $SRCDIR -DCMAKE_BUILD_TYPE=Release -DPLATFORM=host
	make
	make test
}

make_release()
{
	CMAKE_PLATFORM=$1
	CMAKE_TOOLCHAIN=$2
	CMAKE_PARAMS=$3
	
	BUILDDIR=build/release/$CMAKE_PLATFORM
	
	mkdir -p $SCRIPTDIR/cmake-build/$CMAKE_PLATFORM
	cd $SCRIPTDIR/cmake-build/$CMAKE_PLATFORM
	cmake $SRCDIR -DCMAKE_BUILD_TYPE=Release -DPLATFORM=$CMAKE_PLATFORM -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN $CMAKE_PARAMS
	make teeuniverse_editor

	rm -R $BUILDDIR/lib
	cp -R $HOSTDIR/data/assets $BUILDDIR/data
}

mkdir -p $SCRIPTDIR/cmake-build
mkdir -p $SCRIPTDIR/archives

# Check if the program compile on the host, check test and generate assets
make_host

# Generate a build for each platform

## --- Linux 32bit
make_release linux_x86 $TOOLCHAINDIR/Toolchain-linux32.cmake
cd $SCRIPTDIR/cmake-build/linux_x86/build/release/linux_x86
cp -a $ENVDIR/linux_x86/usr/lib/libicu* .
tar -czvf $SCRIPTDIR/archives/teeuniverse_linux_x86.tar.gz *

## --- Linux 64bit
make_release linux_x86_64 $TOOLCHAINDIR/Toolchain-linux64.cmake
cd $SCRIPTDIR/cmake-build/linux_x86_64/build/release/linux_x86_64
cp -a $ENVDIR/linux_x86_64/usr/lib/libicu* .
tar -czvf $SCRIPTDIR/archives/teeuniverse_linux_x86_64.tar.gz *

## --- Windows 32bit
make_release win32 $TOOLCHAINDIR/Toolchain-mingw32.cmake "-DWITHOUT_HARFBUZZ=1"
cd $SCRIPTDIR/cmake-build/win32/build/release/win32
cp $ENVDIR/win32/usr/lib/*.dll .
zip -r $SCRIPTDIR/archives/teeuniverse_win32.zip *

# Cleanup
cd $SCRIPTDIR
rm -R cmake-build
