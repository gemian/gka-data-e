# GKA Data

Simple Sqlite3 database editor, using Enlightenment Foundation Libraries.

Currently supports opening and editing of sqlite databases.

Working towards keyboard friendly input as part of the http://gka.thinkglobally.org project.

## Installation

Install prerequisite packages:

> $ sudo apt-get install build-essential cmake libsqlite3-dev

### Enlightenment Foundation librarys

The project now supports building with any version of EFL from 1.7 onwards, the UI is a tiny bit nicer with the latest version, but it also has some focusing bugs.

#### Build efl from source

See http://www.enlightenment.org/

### Use e17 packages that come with Debian 9

```
sudo apt-get install e17-dev libelementary-dev
```

Copy the handyfiles into place, you only need to do this if the build complains about missing cmake files:
```
sudo mkdir /usr/local/lib/cmake/
sudo cp -r handyfiles/usr-local-lib-cmake/* /usr/local/lib/cmake/
```

## Build

To make install and run from the command line:

> $ mkdir build  
> $ cd build  
> $ cmake ..  
> $ make  
> $ sudo make install  
> $ gka-data-e  

The project also works with CLion and EDI integrated development environments.

## Keyboard navigation

### Main window

Typing regular characters enters into the search box.
* **Enter** and **Shift-Enter** move forwards and backwards between records that match this search term.
* **Ctrl-n** New file
* **Ctrl-o** Open file
* **Ctrl-q** Exit
* **Ctrl-Shift-N** New record
* **Ctrl-Shift-E** Edit record
* **Ctrl-Shift-D** Delete record
* **Ctrl-l** Edit Label Preferences
* **Ctrl-t** Edit Table Preferences

### New file

* **Enter/Return** Create new file
* **Escape** Exit without creating a new file

### Open file

Not currently keyboard navigable, efl standard widget

### New record

* **Ctrl-s** Create new record
* **Escape** Exit without creating a new record

### Edit record

* **Ctrl-s** Update record with changes
* **Escape** Exit discarding changes

### Delete record

* **Enter/Return** Delete the selected record
* **Escape** Exit without deletion

### Edit Label Preferences

* **Ctrl-u** Move selected field up
* **Ctrl-j** Move selected field down
* **Ctrl-e** Edit selected field
* **Ctrl-a** Add new field
* **Ctrl-d** Delete selected field
* **Ctrl-s** Save label changes
* **Escape** Exit discarding changes

### Edit Table Preferences

* **Ctrl-t** Set selected table as active
* **Ctrl-e** Edit selected table
* **Ctrl-a** Add new table
* **Ctrl-d** Delete selected table
* **Ctrl-s** Save label changes
* **Escape** Exit discarding changes

### Add/Edit field/table

* **Enter/Return** Action add/edit
* **Escape** Exit add/edit

### Menu

* **Alt** Opens/Closes the menu
* **Arrow Keys** Navigates the menu
* **Enter** Selects the menu item
* **Escape** Closes the menu
