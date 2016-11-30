TeeUniverse [![CircleCI](https://circleci.com/gh/teeuniverse/teeuniverse.svg?style=svg)](https://circleci.com/gh/teeuniverse/teeuniverse)
=============

TeeUniverse is a fork of TeeWorlds that provides interesting possibilities to make good quality mods.


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


Execution
-------

Currently, only TeeUniverse editor is working. You can try it using this command:
```
teeuniverse_editor
```
