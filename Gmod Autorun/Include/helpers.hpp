

void runLua(const char* code) {
    if (main_state) {
        const int code_result = luaL_loadstring(main_state, code);
        if( LUA_ISERR(code_result) ){
            printf("Lua Code Errored: %s Error\n", code_result == LUA_ERRSYNTAX ? "Syntax" : "Memory");
            lua_pop(main_state, 1);
        }else {
            const int pcall_result = lua_pcall(main_state, 0, 0, 0);

            if (LUA_ISERR(pcall_result)) {
                printf("Code errored in PCall\n");
                // I've been trying to fix this for way too long.. for some reason the C API Really doesn't like lua_tostring right now
                // and I really don't care anymore, so this message is all you're gonna get.
                lua_pop(main_state, 1);
            }
        }
    }
    else {
        printf("Main state not found in runLua call\n");
    }
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

/*
Windows: C:/Users/username/
Linux: /home/username/
macOS: /Users/username/
*/


namespace fs = std::filesystem;

path getHomeDir() {
    char* out;
    size_t _;
    errno_t err = _dupenv_s(&out, &_, "UserProfile"); // C:\User\<Name>
    if (err) { // Couldn't find HOME. Usually Linux / OSX
        printf("Errored when getting home directory.");
        return (path)out;
    }
    path path = out;
    free(out);
    return path;
}


path sanitizeLuaPath(const char* garry_path) {
    std::string haystack = ((string)garry_path).substr(1); // Lua paths start with an @ for some reason
    size_t pos = 0;
    while ((pos = haystack.find("/", pos)) != std::string::npos) {
        haystack.replace(pos, 1, "\\");
        pos += 2;
    }
    path ret = haystack;
    if (!ret.has_extension()) // Default file extension: .txt
        ret.replace_extension("txt");
    return ret;
}

// Recursively makes directories if they do not exist and returns an fstream of the final file name.
std::fstream getSAutorunLog(const char* garry_path) {
    // Assuming the garry path still has the @ in it.
    std::fstream new_log;
    path the_path = getHomeDir() / "sautorun" / "logs" / sanitizeLuaPath(garry_path);
    path file_name = the_path.filename();
    std::filesystem::create_directories(the_path.remove_filename());
    new_log.open(the_path / file_name, std::ios_base::app); // Append to file.
    return new_log;
}