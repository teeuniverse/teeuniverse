TeeUniverses
=============

TeeUniverses is a fork of TeeWorlds that provides interesting possibilities to make good quality mods.

Compilation (Linux)
-------

Download the sources using github
```
git clone https://github.com/teeuniverses/teeuniverses.git
cd teeuniverses
```

Create a build folder to keep the sources clean
```
mkdir build
cd build
```

Compile the project
```
cmake ..
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
