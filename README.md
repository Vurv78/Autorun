# Autorun
Simple, Small script that I autorun for gmod.


## Features:
- Basic lua dumping (hooked loadbuffer, loadbufferx, loadstring)
  - Dumps indiscriminately right now, no server folders
  - In C:\Users\\<Name\>\sautorun

- Run lua script(s) before autorun.
  - Ignore autorun.lua in the project for now..
  
- Separate AllocConsole that allows for running commands (currently only lua_run) (Cause I'm too retarded to hook ConCommand, and either way it's safer)
