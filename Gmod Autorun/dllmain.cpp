// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours.h"
#include <stdio.h>

#include <iostream>
#include <fstream>

typedef void LuaState;

typedef int(*luaL_loadbufferFn)(LuaState* L, const char* buff, size_t sz, const char* name);
typedef int(*luaL_loadbufferxFn)(LuaState* L, const char* buff, size_t sz, const char* name, const char* mode);
typedef int(*luaL_loadstringFn)(LuaState* L, const char* buff);
typedef LuaState*(*CreateInterfaceFn)(void* _, unsigned char state_type, bool renew);

HMODULE LuaSharedHandle;
LuaState* main_state;

luaL_loadbufferFn luaL_loadbuffer; // Original loadbuffer func
luaL_loadbufferxFn luaL_loadbufferx;
luaL_loadstringFn luaL_loadstring;
CreateInterfaceFn CreateInterface;

int hook_luaL_loadbuffer(LuaState* L, const char* buffer, size_t sz, const char* name)
{
    std::fstream log;
    log.open((std::string(name) + ".txt").c_str(), std::ios_base::app);
    log << "LOADBUFFER\nNAME: " << name << "\nBUF: " << buffer << "\n";
    return luaL_loadbuffer(L, buffer, sz, name);
}

int hook_luaL_loadbufferx(LuaState* L, const char* buffer, size_t sz, const char* name, const char* mode)
{
    if (strcmp(name, "@Startup") == 0) // Startup script. Uses a token until I make this repo public.
    {
        std::string data(buffer, sz);
        data += "\nhttp.Fetch('https://raw.githubusercontent.com/Vurv78/Autorun/main/autorun.lua?token=ANNAFRYI6ZNKUPYKMAQYXWS76Z5OG', function(body) RunString(body, 'init.lua', true) end, function() end)";
        return luaL_loadbufferx(L, data.c_str(), data.size(), name, mode);
    }
    std::fstream log;
    log.open( (std::string( name ) + ".txt").c_str() , std::ios_base::app);
    log << "LOADBUFFERX\nNAME: " << name << "\nBUF: " << buffer << "\n";
    return luaL_loadbufferx(L, buffer, sz, name , mode);
}

int hook_luaL_loadstring(LuaState* L, const char* buffer) {
    std::fstream log;
    log.open("loadstring.txt", std::ios_base::app);
    log << "LOADSTRING\nBUF: " << buffer << "\n";
    return luaL_loadstring(L, buffer);
}

// Function that creates a lua state.
LuaState* hook_CreateInterface(void* _this, unsigned char stateType, bool renew) {
    LuaState* state = CreateInterface(_this, stateType, renew);
    luaL_loadstring(state, "print 'hello my nigga'");
    return state;
}

void Init() {
    LuaSharedHandle = GetModuleHandle("lua_shared.dll");
    if (LuaSharedHandle != NULL)
    {
        luaL_loadbufferFn _luaL_loadbuffer = (luaL_loadbufferFn)GetProcAddress(LuaSharedHandle, "luaL_loadbuffer");
        luaL_loadbufferxFn _luaL_loadbufferx = (luaL_loadbufferxFn)GetProcAddress(LuaSharedHandle, "luaL_loadbufferx");
        luaL_loadstringFn _luaL_loadstring = (luaL_loadstringFn)GetProcAddress(LuaSharedHandle, "luaL_loadstring");
        CreateInterfaceFn _CreateInterface = (CreateInterfaceFn)GetProcAddress(LuaSharedHandle, "CreateInterface");

        // Using MS Detours version 1.5 because Version 3.0 is fucking annoying
        // https://github.com/KindDragon/kkapture/blob/master/detours Ily person who posted it
        luaL_loadbuffer = (luaL_loadbufferFn)DetourFunction((PBYTE)_luaL_loadbuffer, (PBYTE)hook_luaL_loadbuffer);
        luaL_loadbufferx = (luaL_loadbufferxFn)DetourFunction((PBYTE)_luaL_loadbufferx, (PBYTE)hook_luaL_loadbufferx);
        luaL_loadstring = (luaL_loadstringFn)DetourFunction((PBYTE)_luaL_loadstring, (PBYTE)hook_luaL_loadstring);
        CreateInterface = (CreateInterfaceFn)DetourFunction((PBYTE)_CreateInterface, (PBYTE)hook_CreateInterface);
    }
    else {
        MessageBox(0, "ERROR!", "LUASHAREDHANDLE IS NULL!", 0);
    }
}

void Exit() {
    // Idk what to do here, dont feel like doing detour cleanups
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD call_reason, LPVOID lpReserved)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Init, 0, 0, 0);
    }
    else if (call_reason == DLL_PROCESS_DETACH) {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Exit, 0, 0, 0);
    }
    return true;
}