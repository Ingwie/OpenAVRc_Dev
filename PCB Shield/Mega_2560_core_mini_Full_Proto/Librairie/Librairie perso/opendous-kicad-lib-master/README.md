# Opendous KiCad library

This repo has the exact content of `KiCad_Libraries-2012-10-18.zip` which can be found at https://code.google.com/archive/p/opendous/downloads

It has been cloned here solely for the purposes of easily including it into other repos as a Git submodule.

Following is the original content of the readme file.

## Adding KiCad Libraries To a Specific Project:

A visual tutorial for adding KiCad Libraries to a project is available at:

http://code.google.com/p/opendous/wiki/KiCADTutorialAddingLibraries

## Adding KiCad Libraries Permanently:

To use Opendous Inc.'s KiCad footprints, you must copy `opendous.dcm` and
`opendous.lib` to your `\KiCad\share\library` directory and copy `opendous.mod` and
`opendous.mdc` to your `\KiCad\share\modules` directory.  For older version of
KiCad, these directories are `\KiCad\library` and `\KiCad\modules`, respectively.

You must then edit your `\KiCad\share\template\kicad.pro` file to inform
KiCad that new library and module files are available.  In older versions of
KiCad, this file is in `\KiCad\template\kicad.pro`.

Open the file in a text editor that does not add formatting, such as
Windows' Notepad, and look for `[pcbnew/libraries]`.  Add a line for
`opendous` to the end of the `[pcbnew/libraries]` list:

```
[pcbnew/libraries]
LibDir=
LibName1=supports
LibName2=connect
LibName3=discret
...
LibName9=opendous
```

Now look for `[eeschema/libraries]` and add `opendous` to that list:

```
[eeschema/libraries]
LibName1=power
LibName2=device
LibName3=transistors
...
LibName29=opendous
```

Now when you run KiCad, Opendous Inc.'s footprints will be available.

`opendous.dcm`, `opendous.lib`, `opendous.mod`, and `opendous.mdc`
are hereby deeded by Opendous Inc. into the Public Domain
under the Creative Commons CC0 Public Domain Dedication:
http://creativecommons.org/publicdomain/zero/1.0/
