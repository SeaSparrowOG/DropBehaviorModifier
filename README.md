# Drop Behavior Modifier
SKSE plugin that supports my model replacement mod. Replaces a given reference's model if the count of it matches a parameter.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/SeaSparrowOG/drop-behavior-modifier
cd drop-behavior-modifier
cmake --preset vs2022-windows
cmake --build build --config Release
```