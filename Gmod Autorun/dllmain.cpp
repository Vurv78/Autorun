// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Include/detours.h"
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <string>


#include "Include/enums.h" // LUA_ stuff that is useful.
#include "Include/func_def.h"

using std::string;

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

CreateInterfaceFn CreateInterface;
MsgFn Msg;

int hook_luaL_loadbuffer(LuaState* L, const char* buffer, size_t sz, const char* name)
{
    std::fstream log;
    log.open((string(name) + ".txt").c_str(), std::ios_base::app);
    log << "LOADBUFFER\nNAME: " << name << "\nBUF: " << buffer << "\n";
    printf("LoadBuffer Call[ Name: %s ]\n", name);
    main_state = L;
    return luaL_loadbuffer(L, buffer, sz, name);
}

int hook_luaL_loadbufferx(LuaState* L, const char* buffer, size_t sz, const char* name, const char* mode)
{
    if (strcmp(name, "@lua/includes/init.lua") == 0) { // Should run before autorun?
        string data(buffer, sz);
        // Needs token until I make this repo public.
        data += "HTTP({url='https://raw.githubusercontent.com/Vurv78/Autorun/main/autorun.lua?token=ANNAFRYI6ZNKUPYKMAQYXWS76Z5OG',success=function(_,body) RunString(body,'init.lua',true) end})";
        return luaL_loadbufferx(L, data.c_str(), data.size(), name, mode);
    }
    std::fstream log;
    log.open( (string( name ) + ".txt").c_str() , std::ios_base::app);
    log << "LOADBUFFERX\nNAME: " << name << "\nBUF: " << buffer << "\n";
    printf("LoadBufferX Call[ Name: %s ]\n", name);
    main_state = L;
    return luaL_loadbufferx(L, buffer, sz, name , mode);
}

int hook_luaL_loadstring(LuaState* L, const char* buffer) {
    std::fstream log;
    log.open("loadstring.txt", std::ios_base::app);
    log << "LOADSTRING\nBUF: " << buffer << "\n";
    printf("Loadstring Call[ Code Length: %d ]\n", strlen(buffer));
    main_state = L;
    return luaL_loadstring(L, buffer);
}

// Function that creates a lua state.
LuaState* hook_CreateInterface(void* _this, unsigned char stateType, bool renew) {
    LuaState* state = CreateInterface(_this, stateType, renew);
    main_state = state;
    printf("CreateInterface Call, set main state!\n");
    return state;
}

// Just prints to gmod console. (It's that white info text so it's pretty hard to notice)
// Woag!! It's printgm from that one rust repo!!!
void printgm( const char* fmt, ... ) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(buf, (string(fmt) + "\n").c_str(), args);
    va_end(args);
    Msg(buf);
}

void runLua(const char* code) {
    if (main_state) {
        luaL_loadstring(main_state, code);
        int result = lua_pcall(main_state, NULL, -1, NULL); // LUA_MULTRET = -1
        printf("Ran lua code. %s\n", LUA_ISERR(result) ? "ERRORED" : "SUCCESS" );
    }
    else {
        printf("Main state not found in runLua call\n");
    }
}

// For now, this will only handle executing lua.
void handleCommands() {
    while (true) {
        string command;
        std::cin >> command;

        const char* cmd_c = command.c_str();

        if (strcmp(cmd_c, "lua_run") == 0) {
            string code;
            getline(std::cin, code);
            runLua( code.substr(1).c_str() );
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
        
        //ExecuteClientCmd _ExecuteClientCmd = (ExecuteClientCmd)GetProcAddress(EngineHandle, "ExecuteClientCmd");

        // Debug Console
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

        // Functions we just want to use, no need to detour. (Well actually there would be a need if we wanted to be undetected..)
        lua_pcall = (lua_pcallFn)GetProcAddress(LuaSharedHandle, "lua_pcall");
        Msg = (MsgFn)GetProcAddress(Tier0Handle, "Msg");

        printf("Successfully loaded all interfaces.\n");

        printgm("-------------------------\n C++ Autorun :D \n-------------------------");

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

    handleCommands();
    //std::thread first (handleCommands);
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