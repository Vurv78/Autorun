typedef void LuaState;

// lua_shared.dll
typedef int(*luaL_loadbufferFn)(LuaState* L, const char* buff, size_t sz, const char* name);
typedef int(*luaL_loadbufferxFn)(LuaState* L, const char* buff, size_t sz, const char* name, const char* mode);
typedef int(*luaL_loadstringFn)(LuaState* L, const char* buff);
typedef int(*lua_pcallFn)(LuaState* L, int nargs, int nresults, int errfunc);

typedef LuaState*(*CreateInterfaceFn)(void* _, unsigned char state_type, bool renew);

// tier0.dll
typedef void(*MsgFn)(char* buff);

// engine.dll
typedef void(*ExecuteClientCmd)(const char* cmd);