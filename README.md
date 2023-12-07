# Belief
Ninja game on NES. Believe it!
## What You Need First
I have only tested on Windows using PowerShell 7. Any previous version of PowerShell will not work.
I think Linux should work too, but I don't use Linux all that much.
- [llvm-mos SDK](https://github.com/llvm-mos/llvm-mos-sdk#download)
  - Needs to be easily searchable by CMake. Set CMAKE_PREFIX_PATH to `path/to/llvm-mos` or add `path/to/llvm-mos/bin` to your PATH.

The rest of these should be in your PATH environment variable too:
- [CMake](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)
- [Python 3](https://www.python.org/downloads/)
  - [Pillow](https://pillow.readthedocs.io/en/stable/) - for converting PNG to CHR
## How to Build
In Linux, simply run `build.sh` in the terminal. In Windows, simply run `build.ps1` in PowerShell 7 or later.
CMake will be configured and the ROM will be compiled to `./build/belief.nes`.
Arguments will be passed to Ninja like this:
```powershell
./build.ps1 run
```
Build type can be set like this:
```powershell
BUILD_TYPE=Release ./build.ps1
```
Default is RelWithDebInfo.
## How to Run
Load the ROM in the `build` directory in your NES emulator of choice ([Mesen](https://github.com/SourMesen/Mesen2) recommended) or flashcart of choice.
The mapper used is the Action 53 mapper, so make sure your emulator is new enough to support that.
You can also:
```powershell
./build.ps1 run
```
Normally this will try to run the .NES file directly through what program your system associates with the file.
You can also set the `EMULATOR` environment variable to a path to your NES emulator of choice.
