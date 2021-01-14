#include "pch.h"
#include "Include/Mod.h"

using std::string;
using std::filesystem::path;

typedef void LuaState;

// Modules / DLLs
HMODULE LuaSharedHandle;
HMODULE Tier0Handle;

LuaState* main_state;

// Global functions
luaL_loadbufferFn luaL_loadbuffer; // Original loadbuffer func
luaL_loadbufferxFn luaL_loadbufferx;
luaL_loadstringFn luaL_loadstring;
lua_pcallFn lua_pcall;
lua_tostringFn lua_tostring;
lua_settopFn lua_settop;

MsgFn Msg;

#include "Include/helpers.hpp"

int hook_luaL_loadbuffer(LuaState* L, const char* buffer, size_t sz, const char* name)
{
    std::fstream log = getSAutorunLog(name);
    log << "LOADBUFFER\nNAME: " << name << "\nBUF: " << buffer << "\n";
    printf("LoadBuffer Call[ Name: %s ]\n", name);
    main_state = L;
    return luaL_loadbuffer(L, buffer, sz, name);
}

int hook_luaL_loadbufferx(LuaState* L, const char* buffer, size_t sz, const char* name, const char* mode)
{
    if (strcmp(name, "@lua/includes/init.lua") == 0) { // Should run before autorun?
        // Don't read from here.
        runLua(" print('Hooked luaL_loadbufferx.') ");
    }
    std::fstream log = getSAutorunLog(name);
    log << "LOADBUFFERX\nNAME: " << name << "\nBUF: " << buffer << "\n";
    printf("LoadBufferX Call[ Name: %s ]\n", name);
    main_state = L;
    return luaL_loadbufferx(L, buffer, sz, name , mode);
}

int hook_luaL_loadstring(LuaState* L, const char* buffer) {
    std::fstream log = getSAutorunLog("loadstring.txt");
    log << "LOADSTRING\nBUF: " << buffer << "\n";
    printf("Loadstring Call[ Code Length: %d ]\n", strlen(buffer));
    main_state = L;
    return luaL_loadstring(L, buffer);
}

// For now, this will only handle executing lua, but you could definitely customize it to run specific commands like a server crasher :v
void handleCommands() {
    while (true) {
        string command;
        std::cin >> command;

        const char* cmd_c = command.c_str();

        if (strcmp(cmd_c, "lua_run") == 0) {
            string code;
            getline(std::cin, code);
            std::cout << code.substr(1).c_str() << "\n";
            runLua(code.substr(1).c_str());
        }
        else if (strcmp(cmd_c, "lua_dofile") == 0) {
            printf("Todo..");
        }
    }
}

void Init() {
    LuaSharedHandle = GetModuleHandle("lua_shared.dll");
    Tier0Handle = GetModuleHandle("tier0.dll");
    HMODULE EngineHandle = GetModuleHandle("engine.dll");
    if (LuaSharedHandle != NULL)
    {
        luaL_loadbufferFn _luaL_loadbuffer = (luaL_loadbufferFn)GetProcAddress(LuaSharedHandle, "luaL_loadbuffer");
        luaL_loadbufferxFn _luaL_loadbufferx = (luaL_loadbufferxFn)GetProcAddress(LuaSharedHandle, "luaL_loadbufferx");
        luaL_loadstringFn _luaL_loadstring = (luaL_loadstringFn)GetProcAddress(LuaSharedHandle, "luaL_loadstring");
        CreateInterfaceFn _CreateInterface = (CreateInterfaceFn)GetProcAddress(LuaSharedHandle, "CreateInterface");


        // Debug Console
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

        // Functions we just want to use, no need to detour. (Well actually there would be a need if we wanted to be undetected..)
        lua_pcall = (lua_pcallFn)GetProcAddress(LuaSharedHandle, "lua_pcall");
        lua_tostring = (lua_tostringFn)GetProcAddress(LuaSharedHandle, "lua_tostring");
        lua_settop = (lua_settopFn)GetProcAddress(LuaSharedHandle, "lua_settop");

        Msg = (MsgFn)GetProcAddress(Tier0Handle, "Msg");

        printf("Successfully loaded all interfaces.\n");

        printgm("-------------------------\n C++ Autorun :D \n-------------------------");

        // Using MS Detours version 1.5 because Version 3.0 is fucking annoying
        // https://github.com/KindDragon/kkapture/blob/master/detours Ily person who posted it
        luaL_loadbuffer = (luaL_loadbufferFn)DetourFunction((PBYTE)_luaL_loadbuffer, (PBYTE)hook_luaL_loadbuffer);
        luaL_loadbufferx = (luaL_loadbufferxFn)DetourFunction((PBYTE)_luaL_loadbufferx, (PBYTE)hook_luaL_loadbufferx);
        luaL_loadstring = (luaL_loadstringFn)DetourFunction((PBYTE)_luaL_loadstring, (PBYTE)hook_luaL_loadstring);
    }
    else {
        MessageBox(0, "ERROR!", "LUASHAREDHANDLE IS NULL!", 0);
    }

    handleCommands();
}

void Exit() {
    // Idk what to do here, dont feel like doing detour cleanups
    FreeConsole();
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
