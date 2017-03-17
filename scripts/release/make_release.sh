#!/bin/sh

#Rewrite parameters
OPTIONS_SHORT=hv:lLwWcj:
OPTIONS_LONG=help,version:,linux32,linux64,win32,win64,cleanup

PARSED=`getopt --options $OPTIONS_SHORT --longoptions $OPTIONS_LONG --name "$0" -- "$@"`
eval set -- "$PARSED"

#Get parameters
TUVERSION="unstable"
OPT_LINUX32=0
OPT_LINUX64=0
OPT_WIN32=0
OPT_WIN64=0
OPT_CLEANUP=0
OPT_CFLAGS=0
while true; do
    case "$1" in
        -v|--version)
            TUVERSION="$2"
            shift 2
            ;;
        -j)
            OPT_CFLAGS="-j$2"
            shift 2
            ;;
        -l|--linux32)
            OPT_LINUX32=1
            shift
            ;;
        -L|--linux64)
            OPT_LINUX64=1
            shift
            ;;
        -w|--win32)
            OPT_WIN32=1
            shift
            ;;
        -W|--win64)
            OPT_WIN64=1
            shift
            ;;
        -c|--cleanup)
            OPT_CLEANUP=1
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS...]"
            echo "  -h,  --help        print this help message"
            echo "  -v,  --version     set the version number of the release to generate"
            echo "  -l,  --linux32     generate a binary for linux 32-bit systems"
            echo "  -L,  --linux64     generate a binary for linux 64-bit systems"
            echo "  -w,  --win32       generate a binary for windows 32-bit systems"
            echo "  -W,  --win64       generate a binary for windows 64-bit systems"
            echo "  -c,  --cleanup     clean compilations files"
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
			echo "Unknown argument \"$1\""
            exit 3
            ;;
    esac
done

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
	make $OPT_CFLAGS
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
	make teeuniverse_editor $OPT_CFLAGS

	rm -R $BUILDDIR/lib
	cp -R $HOSTDIR/data/packages $BUILDDIR/data
}

mkdir -p $SCRIPTDIR/cmake-build
mkdir -p $SCRIPTDIR/archives

# Check if the program compile on the host, check tests and generate assets
make_host

# Generate a build for each platform

## --- Linux 32bit
if [ "$OPT_LINUX32" = 1 ] ; then
	make_release linux_x86 $TOOLCHAINDIR/Toolchain-linux32.cmake
	cd $SCRIPTDIR/cmake-build/linux_x86/build/release/linux_x86
	cp -a $ENVDIR/linux_x86/usr/lib/libicu* .
	cd ..
	rm -Rf teeuniverse-$TUVERSION-linux_x86
	mv linux_x86 teeuniverse-$TUVERSION-linux_x86
	tar -czvf $SCRIPTDIR/archives/teeuniverse-$TUVERSION-linux_x86.tar.gz teeuniverse-$TUVERSION-linux_x86
fi

## --- Linux 64bit
if [ "$OPT_LINUX64" = 1 ] ; then
	make_release linux_x86_64 $TOOLCHAINDIR/Toolchain-linux64.cmake
	cd $SCRIPTDIR/cmake-build/linux_x86_64/build/release/linux_x86_64
	cp -a $ENVDIR/linux_x86_64/usr/lib/libicu* .
	cd ..
	rm -Rf teeuniverse-$TUVERSION-linux_x86_64
	mv linux_x86_64 teeuniverse-$TUVERSION-linux_x86_64
	tar -czvf $SCRIPTDIR/archives/teeuniverse-$TUVERSION-linux_x86_64.tar.gz teeuniverse-$TUVERSION-linux_x86_64
fi

## --- Windows 32bit
if [ "$OPT_WIN32" = 1 ] ; then
	make_release win32 $TOOLCHAINDIR/Toolchain-mingw32.cmake "-DWITHOUT_HARFBUZZ=1"
	cd $SCRIPTDIR/cmake-build/win32/build/release/win32
	cp $ENVDIR/win32/usr/lib/*.dll .
	cd ..
	rm -Rf teeuniverse-$TUVERSION-win32
	mv win32 teeuniverse-$TUVERSION-win32
	zip -r $SCRIPTDIR/archives/teeuniverse-$TUVERSION-win32.zip teeuniverse-$TUVERSION-win32
fi

## --- Windows 64bit
if [ "$OPT_WIN64" = 1 ] ; then
	make_release win64 $TOOLCHAINDIR/Toolchain-mingw64.cmake "-DWITHOUT_HARFBUZZ=1"
	cd $SCRIPTDIR/cmake-build/win64/build/release/win64
	cp $ENVDIR/win64/usr/lib/*.dll .
	cd ..
	rm -Rf teeuniverse-$TUVERSION-win64
	mv win64 teeuniverse-$TUVERSION-win64
	zip -r $SCRIPTDIR/archives/teeuniverse-$TUVERSION-win64.zip teeuniverse-$TUVERSION-win64
fi

## --- Cleanup
if [ "$OPT_CLEANUP" = 1 ] ; then
	echo "CleanUp!"
	cd $SCRIPTDIR
	rm -R cmake-build
fi
