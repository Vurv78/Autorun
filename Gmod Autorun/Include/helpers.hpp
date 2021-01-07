void runLua(const char* code) {
    if (main_state) {
        luaL_loadstring(main_state, code);
        int result = lua_pcall(main_state, NULL, -1, NULL); // LUA_MULTRET = -1
        printf("Ran lua code. %s\n", LUA_ISERR(result) ? "ERRORED" : "SUCCESS");
    }
    else {
        printf("Main state not found in runLua call\n");
    }
}

/*
Windows: C:/Users/username/Documents/gluasteal/
Linux: /home/username/gluasteal/
macOS: /Users/username/gluasteal/
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
    ret = ret.has_filename() ? ret : (ret / ".txt"); // If no filename, default to .txt (Doesn't work??)
    return ret;
}

// Recursively makes directories if they do not exist and returns an fstream of the final file name.
std::fstream getSAutorunLog(const char* garry_path) {
    // Assuming the garry path still has the @ in it.
    std::fstream new_log;
    path the_path = getHomeDir() / "sautorun" / sanitizeLuaPath(garry_path);
    path file_name = the_path.filename();
    std::filesystem::create_directories(the_path.remove_filename());
    new_log.open(the_path / file_name, std::ios_base::app); // Append to file.
    return new_log;
}