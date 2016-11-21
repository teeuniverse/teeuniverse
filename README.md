TeeUniverses
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
cmake .
make
```


Execution
-------

Before starting TeeUniverses, the assets packages must be generated.
```
for MAKEASSETS in ./make_assets_*; do $MAKEASSETS; done
```
This operation is needed because the assets package format changes regularly.
In the future, those packages will be included directly in the repository.

Finally, start TeeUniverses Editor
```
teeuniverses_editor
```
