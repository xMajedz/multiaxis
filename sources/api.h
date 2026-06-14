#pragma once
#include "common.h"
#include "luau.h"
#include "game.h"

namespace Api
{
	static lua_State* ML;

	static Gamerules rules;

	static std::vector<Body>   o_vector;
	static std::vector<Joint>   oj_vector;

	static std::vector<Player> p_vector;
	 
	static size_t o_count;
	static size_t oj_count;

	static size_t p_count;

	static Body*   o  = nullptr;
	static Joint*  oj = nullptr;

	static Player* p  = nullptr;

	std::vector<Joint> GetJointObjects();
	size_t GetJointObjectsCount();
	
	static std::map<std::string_view, BodyID> o_map;

	static std::map<std::string_view, BodyID> b_map;

	static std::vector<Body>  b_vector;
	static std::vector<Joint> j_vector;

	static size_t b_count;
	static size_t j_count;
	
	static Body*  b = nullptr;
	static Joint* j = nullptr;
	
	void Init();
	void Boot(const char* filename);
	
	void Reset();
	void Close();
	
	Gamerules GetRules();

	std::vector<Body> GetObjects();
	std::vector<Player> GetPlayers();

	size_t GetObjectsCount();
	size_t GetPlayersCount();
	
	int UpdateCallback(dReal dt);
	int DrawCallback();
	int Draw3DCallback();
	int NewGameCallback();
	int FreezeCallback();
	int StepCallback();

	int FileDroppedCallback(std::string_view dropped_file);
	int ConsoleCallback(const char* message);

	void SetCallback(const char* event, const char* handle, lua_CFunction function);
	lua_CFunction GetCallback(const char* event, const char* handle);

	int loadmod(std::string_view modpath);
	int loadscript(lua_State* L, std::string_view scriptpath);
}

typedef void (*ConsoleCallback_t)(const char*);

namespace Console
{
	static size_t message_count = 0;
	static size_t message_length = 256;
	static size_t message_buffer_offset = 0;
	static char   message_buffer[1000 * 256];
	static char*  messages[256];
	static char   last_message[1024];
	static bool   has_message;

	static ConsoleCallback_t m_callback = nullptr;

	void SetCallback(ConsoleCallback_t callback);
    void Update();
    void log(const char* message);
};

void log_raylib(int logLevel, const char* text, va_list args);

void luaopenApi(lua_State* L);

int luaopenApiGame(lua_State* L);

int luaopenApiNet(lua_State* L);

int luaopenApiReplay(lua_State* L);

int luaopenApiRaylib(lua_State* L);

int luaopenApiRaygui(lua_State* L);

int luaopenApiRaymath(lua_State* L);

int luaopenApiExpermental(lua_State* L);
