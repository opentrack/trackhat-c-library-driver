# The TrackHat DLL driver

This is C library as driver for the TrackHat camera connected via USB dedicated for Windows
systems.

## Prerequisities

To work with the library, the following components are required:

* cmake (3.10+)
* Microsoft Visual Studio 2017 with "Desktop development with C++" package
* Inno Setup 6

## Compilation

This library can be compiled using:

1. Qt creator:
File -> Open File or Project... -> <Project Directory>/CMakeLists.txt
2. Using Natlive Tools Command Prompt for VS 2017:
Go to project repository and use a command:

```bash
    scripts\\build_all_windows.bat
```

After the compilation the library will be stored in the <Project Directory>\build\src\Release