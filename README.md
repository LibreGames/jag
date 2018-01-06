# JAG

![Screenshot](/screenshot.jpg "Screenshot")

JAG is a match-three 2D puzzle game which runs on Linux, Windows and FreeBSD.
JAG was created using only free development software and free resources.

XlabSoft & Industrial Infosystems 2009-2012

## Compilation

Note that the version in this repo can also be built with Qt 5.
It requires the following Qt5 components:

- Qt5 OpenGL
- Qt5 X11Extras
- Qt5 Xml

Building can be done with the following commands:

```
qmake
make
```

The binary will be built in the bin folder.

## Supported Platforms

Since JAG is based on Qt5 (or Qt4) and SDL frameworks, all the platforms
supported by them should be able to build and launch the game.

In particular:

- MS Windows 7, Vista, XP
- All Linux OS
- FreeBSD

## License

This software is free and is distributed according to GPL and compatible licenses.
Full text of GPL and more you can read here:
  http://www.fsf.org/licensing/licenses/gpl.html
