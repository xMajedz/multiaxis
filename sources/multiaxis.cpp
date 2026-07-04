
#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "Game.h"

#include <iostream>

#include <cstring>
#include <cstdlib>

class Bytecode {
public:  
    int load(lua_State* L, const std::string& chunkname)
    {
	    return luau_load(L, chunkname.data(), data(), size(), 0);
    };

	char* data()
	{
		return m_data;
	};

	size_t size()
	{
		return m_size;
	};

    Bytecode(const std::string& string)
	{
		m_data = luau_compile(string.data(), string.size(), NULL, &m_size);
	};

	~Bytecode()
	{
	    std::free(m_data);
	};
private:
	char* m_data;
	size_t m_size;
};


void help(void)
{
    const char* help_message =
	  "Usage: multiaxis <flag>\n"
	  "\tmultiaxis --help -h\n"
	  "\tmultiaxis --version -v\n"
	  "\tmultiaxis --bootfile -b <file>"
	;
    std::cout << help_message << std::endl;
}

int main(int argc, char* argv[])
{
    const char* bootfile = "boot";

	for (int i = 0; i < argc; i += 1) {
	    const char* arg = argv[i];

		if ((strcmp(arg, "--bootfile") == 0) || strcmp(arg, "-b") == 0) {
            i += 1;
		    bootfile = argv[i];
	    } else if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0)) {
            i += 1;
		    help();
			return 0;
	    } else if ((strcmp(arg, "--version") == 0) || (strcmp(arg, "-v") == 0)) {
            i += 1;
			//std::cout << GAME_VERSION << std::endl;
			return 0;
        }
	}

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	Bytecode code("print(_VERSION)");
	code.load(L, "~print");
	lua_call(L, 0, 0);
	  
	Game& GameInstance = Game::GetInstance();
    bool running = true;
	while (running) {
	    GameInstance.Draw();
	    running = !WindowShouldClose();
	}
}
