TeeUniverse [![CircleCI](https://circleci.com/gh/teeuniverse/teeuniverse.svg?style=svg)](https://circleci.com/gh/teeuniverse/teeuniverse)
=============

TeeUniverse is a free and open-source game offering powerful and robust support for playing online various gamemodes and playgrounds.


Required Libraries and Programs
-------

TeeUniverse, like TeeWorlds, needs SDL2, OpenGL, libpng and freetype.
The libraries ICU and Harfbuzz must be also installed.
Please make sure that CMake is install too, because the compilation process relies on it.


Compilation (Linux)
-------

Download the sources using github
```
git clone https://github.com/teeuniverse/teeuniverse.git
cd teeuniverse
```

Compile the project
```
cmake -DCMAKE_BUILD_TYPE=Release .
make
sudo make install
```


Compilation (Windows, MinGW)
-------

Download the sources from [GitHub (ZIP file)](https://github.com/teeuniverse/teeuniverse/archive/master.zip)

Download and install [CMake (official download page)](https://cmake.org/download)

Download all needed libraries and sources from [the official website of TeeUniverse (ZIP file)](http://teeuniverse.net/data/teeuniverse-windows-libs.zip), and place them in the TeeUniverse sources

Run CMake and add the following entries/variables:
```
CMAKE_BUILD_TYPE=Release
WITHOUT_HARFBUZZ
```

Configure and generate the Makefile with CMake

Execute the Makefile with MinGW


Execution
-------

Currently, only TeeUniverse editor is working. You can try it using this command:
```
teeuniverse_editor
```
or, if you are using Windows, by clicking on teeuniverse_editor.exe
