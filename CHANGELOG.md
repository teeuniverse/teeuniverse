# Change Log

## [0.3.2]
### Added
- 'config_directory.bat' for windows

### Fixed
- DDNet Tele layer import for infclass maps

## [0.3.1] - 2022-01-22
### Added
- Ctrl+Backspace for smart characters deletion
- InfClass maps importer (we have no .tup for many maps)
- An icon for Windows executable

### Changed
- DDNet package now enabled for infclass format (use only for 'Turn entities off' signs)
- ddTele layer now exported to infclass maps

### Fixed
- Zoom with active Vertex Editor
- Tile texture grid spacing on map import
- A crash on invalid animation
- A crash on a switch between maps
- Grid alignment for quad vertex editor
- 'Paste' action for text editors (integer / float / color code line editors)
- Animated quad position on export
- The work with paths on windows

## [0.3.0] - (not released)
### Added
- Quads and objects can be animated
- Add zone palette for filling tool

### Changed
- Select range of subitem with Shift key
- Unselect single subitem with Control key

### Fixed
- Vertex list outdated after vertex insertion
- Pressing spacebar in text field open map palette
- Layers using tiling materials are not exported correctly
- Undo doesn't work for bezier controllers

## [0.2.5]
### Added
- Transform and stamp tools for object layers
- Spacebar to open or close the stamp palette
- Desktop integration for Linux
- About dialog

### Changed
- Package dependencies are searched in all directories where a package is opened
- File dialog list all directories where a package have been opened in current or previous sessions
- Zoom in map view is now relative to cursor position
- Default layer size set to the global size of all zones
- Context menu to create new element for map sections (background layers, zones, ...)
- Default license set to CC-BY-SA 4.0

### Fixed
- Crash after right clicking on a package
- Wrong map export when a zone layer position is negative
- Moving an element in the asset inspector change the current selection
- No vertices are shown when the vertex editor is used on an object layer for the first time
- Some debug message stay visible in the terminal

## [0.2.4]
### Added
- Position parameter for zone layers
- Seed parameter for layers that use a material
- Pressing shift with the stamp tool fill the selection with the first tile
- Filling tool
- Multiple items can be selected and edited together in the asset inspector
- Shortcuts for tools

### Changed
- Faster update of tiles for layers that use materials
- Crop tool can resize zone layers in all directions
- New quads are two times smaller to keep the same size than tiles
- New maps contain a background with caves and a playground with grass and unhookable ready

### Fixed
- Wrong TeleID in exported maps
- Alpha channel in env_infclass is not dilated
- Inversed diagonal in imported and exported quads
- Lost color change when the focus is released outside the color popup
- Better parsing of hexadecimal color code
- Inserting "0", "-" or "+" in a text field trigger the zoom change 

## [0.2.3]
### Added
- Automatic tiling
- Level of detail parameter for map layers (similar to "detail" option in TeeWorlds)

### Fixed
- Click on a quad don't change the rotation/scaling mode
- The stamp tool doesn't select the correct tiles in negative coordinates

## [0.2.2]
### Added
- Support of Ninslash maps
- Support of FNG2 maps
- Support of DummyCapture maps
- Support of Toot maps
- Support of DDNet front, switch, tele and tune layer
- Object zone layers
- Preference window to set keybindings
- Option to create a ZIP file that contains the current package and all its dependencies
- Button to apply dilate and border_add to images
- Each quad get its own global color in addition of the color set for each vertex

### Changed
- View background changed for a checkerboard
- Pressing the 1:1 zoom button two times get back the old zoom value
- The last selected tab in the color picker is always the default tab, even after restarting the editor
- Default packages are now opened in read-only mode by default

### Fixed
- OpenFNG map import
- Zoom-out can freeze the editor
- Flickering cursor on some environments
- Undo after creating a path block the tool
- List of quads/entities is not refreshed after deleting an element
- Crash when the editor is closed

## [0.2.1]
### Added
- Images can be replaced and exported
- Eraser tool for images
- Ability to change the group of a layer, using layer ordering or a dedicated button
- Ability to move a group in background/foreground using group ordering
- Button to duplicate groups, entity layers, zone layers and other map layers
- Button to set the zoom to 1:1
- Entry box to display and set the zoom factor
- Option to keep entities, quads and vertices align with the grid during translation
- Creation of quads when no images are defined in the layer

### Changed
- Path creator tool create corner vertices by default
- Vertex smooth type is now visible directly in the list of vertices
- Updated infclass package

### Fixed
- List of quads and entities is not updated after using the stamp tool
- Two Bezier controllers displayed on end points
- Two assets can have the same name, leading to corrupted TUP file
- Delete a layer in a map change the current edited element to none, leading to an empty view
- Incoherent behaviour of the right click in the asset list
- Mouse click in the map display settings pass through the popup window 

## [0.2.0]
### Added
- Object layers in maps
- Materials
- Tool to create sprite from images
- Tools to create an object and edit its vertices
- Mirror buttons for tile layers
- Export maps to the InfClass format
- Ctrl+Left button to move the camera in the map
- Shortcuts to flip and rotate the stamp selection
- Text selection with copy/paste
- Automatic scrolling during text editing
- Statusbar to display map coordinates
- Confirmation dialog during saving
- Ctrl+S to save the current package
- Import/Export in PTU map format
- Option to align entities with on the grid
- Camera position and zoom saved in the package
- Settings in map view
- Game layer of DDNet maps support

### Changed
- Bigger toolbar in the view
- New icons for tools in the view
- Better appearance for focused boxes
- Better appearance for separators

### Removed
- Import/Export in the old InfClass map format

### Fixed
- Fix non-loaded images after deletion of an image
- Fix unusable crop tool for map layers (the tool is now disabled for layers)
- Fix incoherent behaviour for sliders
- Fix visible gui_editor package in the asset selector
- Fix wrong clipping values for exported maps
- Fix internal images exported as external
- Fix env_snow package

## [0.1.0] - 2016-11-30
- The first version of TeeUniverse
