// Taken from https://www.lua.org/source/5.3/lua.h.html

#define LUA_MULTRET    -1
#define LUA_OK          0
#define LUA_YIELD       1
#define LUA_ERRRUN      2
#define LUA_ERRSYNTAX   3
#define LUA_ERRMEM      4
#define LUA_ERRGCMM     5
#define LUA_ERRERR      6

#define LUA_ISERR(n) (n!=LUA_OK)

#define lua_pop(L,n)  lua_settop(L, -(n)-1)