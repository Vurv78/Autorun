# Autorun [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Simple, Small script that dumps lua files indiscriminantly.
I made this as a learning experience and to make a smaller version of https://github.com/lewisclark/glua-steal. (That repo is pretty large for what little it does..)  

This has been succeeded by [Autorun-rs](https://github.com/Vurv78/Autorun-rs) which is written in Rust and has more features alongside pre-built dlls. (It is also much easier to build yourself thanks to cargo.)

## Features:
- Basic lua dumping (hooked loadbuffer, loadbufferx, loadstring)
  - Dumps indiscriminately right now, no server folders
  - In C:\Users\\<Name\>\sautorun

- Run lua script(s) before autorun.
  - Ignore autorun.lua in the project for now..

- Separate AllocConsole that allows for running commands (currently only lua_run) (Cause I'm too retarded to hook ConCommand, and either way it's safer)

## Dependencies:
- This requires Microsoft Detours v1.5
  - I found it here https://github.com/KindDragon/kkapture/blob/master/detours.
  - I have removed it from the github since I was a bit paranoid.
  - Drag detours.h into /Include/ in the project, and detours.lib into the same folder as dllmain.cpp.
  - See the updated version: https://github.com/microsoft/Detours

Also you of course need an injector, any one will do. Just make sure the script builds to the injector target (32 or 64 bit)
Same applies to your gmod version. If you use 32 bit, build to 32 bit, vice versa
