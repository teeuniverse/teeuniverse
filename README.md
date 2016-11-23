TeeUniverses [![CircleCI](https://circleci.com/gh/teeuniverses/teeuniverses.svg?style=svg)](https://circleci.com/gh/teeuniverses/teeuniverses)
=============

TeeUniverses is a fork of TeeWorlds that provides interesting possibilities to make good quality mods.


Required Libraries and Programs
-------

TeeUniverses, like TeeWorlds, needs SDL2, OpenGL, libpng and freetype.
The libraries ICU and Harfbuzz must be also installed.
Please make sure that CMake is install too, because the compilation process relies on it.


Compilation (Linux)
-------

Download the sources using github
```
git clone https://github.com/teeuniverses/teeuniverses.git
cd teeuniverses
```

Compile the project
```
cmake -DCMAKE_BUILD_TYPE=Release .
make
sudo make install
```


Execution
-------

Currently, only TeeUniverses editor is working. You can try it using this command:
```
teeuniverses_editor
```
