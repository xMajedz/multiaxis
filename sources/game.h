#pragma once
#include "common.h"
#include "player.h"
#include "mem.h"
#include "game_version.h"

enum Gamemode
{
	FREE_PLAY,
	SELF_PLAY,
	REPLAY_PLAY,
	REPLAY_EDIT,
};

struct Gamerules
{
	std::string mod = "nomod";

	int numplayers;
	int turnframes;
	int max_contacts = 8;
	dReal reaction_time = 0;
	dReal engagedistance;
	dReal engageheight;
	dReal friction;
	dReal bounce;

	vec3<dReal> gravity;
};

struct Gamestate
{
	Gamemode mode = FREE_PLAY;

	double time = 0;

	int game_frame = 0;

	dReal reaction_count = 0;

	dReal freeze_time = 0;
	int freeze_frames = 0;
	int freeze_frame = 0;
	int freeze_count = 0;

	dReal step_time = 0;

	BodyID  selected_object  = -1;

	PlayerID selected_player = -1;
	JointID  selected_joint  = -1;
	BodyID  selected_body  = -1;

	bool running = false;
	bool freeze = false;
	bool pause = false;
};

struct EnvPhysicsObject
{
    dReal friction;
    dReal bounce;
  
    dGeomID geom_ = nullptr;
    dUserData data_;

};
  
struct EnvPlane : EnvPhysicsObject
{
    vec4<dReal> param;
};

struct EnvObject : EnvPhysicsObject
{
  	raylib::Color color;
	raylib::Color g_color;

    BodyShape shape;
	
    vec4<dReal> orientation;
	vec3<dReal> position;
	vec3<dReal> offset;
	vec3<dReal> sides;

    dReal radius;
	dReal length;
	dReal mass;
  
   	uint32_t flag_;
	bool static_;
	bool interactive_;
    bool composite_;

    dMass mass_;

    dBodyID body_ = nullptr;
};

struct EnvJoint : public EnvObject
{
    JointType type;
	JointState state;
	JointState state_alt;

	BodyID connections[2];

    vec3<dReal> axis;
    vec3<dReal> axis_alt;

	dReal range[2];
	dReal range_alt[2];
	dReal strength;
	dReal strength_alt;
	dReal velocity;
	dReal velocity_alt;

	dReal frame_vel;
	dReal frame_vel_alt;

    dJointID joint_ = nullptr;
};

struct StaticEnv
{
    std::vector<EnvPlane> planes;
    std::vector<EnvObject> objects;
};

struct DynamicEnv
{
    std::vector<EnvObject> objects;
    std::vector<EnvJoint> joints;
};

struct ModData
{
    Gamerules rules;
    StaticEnv static_env;
    DynamicEnv dynamic_env;
};

struct PlayerFrameJoint
{
    vec3<dReal> position;
    vec4<dReal> orientation;
};

struct PlayerFrameBody
{
    vec3<dReal> position;
    vec4<dReal> orientation;
};

class Game {
public:	
    static Game& GetInstance()
    {
        static Game instance;
        return instance;
    }
  
    void Start();
	void Restart();
    void Reset();
 	void Quit();

	bool Running();

 	Gamestate state;
	Gamerules rules;

	dWorldID world = nullptr;
	dSpaceID space = nullptr;
	dJointGroupID contactgroup = nullptr;

	dMass mass;
	dReal step;
	
	dGeomID floor = nullptr;
	dReal floor_friction = 10E3;
	dReal floor_bounce = 0;
	dUserData floor_data;

    std::vector<EnvPlane> planes;
	std::vector<Body> objects;

	std::vector<Body> dynamic_objects;
	std::vector<Body> static_objects;
	std::vector<Joint> joint_objects;

	std::vector<Player> players;
	int player_ghosts[16];

	Arena* cache = nullptr;
	size_t cache_size = 4 * 1024 * 1024;

	size_t frame_size = 0;
	
	bool ghost_cache_enabled = false;
	uint32_t ghost_cache_offset = 0;
	uint32_t ghost_cache_frames = 0;

	uint32_t ghost_length = 50;
	uint8_t  ghost_transparency = 255;
    bool turnframe_ghost = false;
 
	bool replay_cache_enabled = false;
	uint32_t replay_cache_frames = 0;

	size_t o_count;
	size_t jo_count;

	size_t p_count;

	dContact m_frame_contacts[1024];
	dContact m_freeze_contacts[1024];

	int numcontacts;
	int numcollisions;

	void TogglePause();

    void GetSettings();

	bool GetPause();
	bool GetFreeze();
	
	double GetTime();
	double GetFrameTime();

	void SetMode(Gamemode mode);
	void SetGameFrame(uint32_t frame);
	void SetBackgroundColor(uint16_t r, uint16_t g, uint16_t b, uint16_t a);

	void SetSelectedPlayer();
	void SetSelectedPlayer(PlayerID pID);

	Player& GetPlayer(PlayerID pID);
	Player& GetSelectedPlayer();
	PlayerID GetSelectedPlayerID();

	void SetSelectedJoint();
	void SetSelectedJoint(JointID jID);

	Joint& GetSelectedJoint();
	JointID GetSelectedJointID();

	dReal GetSelectedJointVelocity();
	dReal GetSelectedJointVelocityAlt();

	Joint& GetJoint(PlayerID pID, JointID jID);
	Body& GetBody(PlayerID pID, BodyID bID);

	void SetBodyState(PlayerID pID, BodyID bID, bool state);

	Gamemode GetGamemode();
	Gamerules& GetGamerules();

	std::vector<Player>& GetPlayers();
	std::vector<Body>& GetObjects();
	
	void NearCallback(dGeomID o1, dGeomID o2);

	void SetGravity(dReal x, dReal y, dReal z);
	void SetMaxContacts(size_t count);
	void SetFriction(dReal frction);
	void SetBounce(dReal bounce);
	void SetTurnFrames(size_t frames);
	void SetReactionTime(size_t t);

	std::string GetMod();

	size_t GetContactCount();
	size_t GetObjectCount();
	size_t GetPlayerCount();
	size_t GetPlayerBodyCount(PlayerID player_id);
	size_t GetPlayerJointCount(PlayerID player_id);

	size_t GetMaxContacts();
	size_t GetGameFrame();

	dReal GetReactionTime();
	dReal GetReactionCount();
	
	void ImportMod();
    void CreateDynamicObject(EnvObject& object);
    void CreateStaticObject(EnvObject& object);

    void CreatePlane(EnvPlane& plane);  
	void NewGame();
	
	void ToggleGhosts();

	void TriggerPlayerJointState(PlayerID player_id, JointID joint_id, JointState state);
	void TriggerPlayerJointStateAlt(PlayerID player_id, JointID joint_id, JointState state);

	void TriggerPlayerJoint(PlayerID player_id, JointID joint_id, JointState state, dReal vel);
	void TriggerPlayerJointAlt(PlayerID player_id, JointID joint_id, JointState state, dReal vel);

	void TogglePlayerPassiveStatesAlt(PlayerID player_id);
	void TogglePlayerPassiveStates(PlayerID player_id);
	void ToggleSelectedPlayerPassiveStatesAlt();
	void ToggleSelectedPlayerPassiveStates();

	void ToggleJointActiveStateAlt(JointID selected_joint_id);
	void ToggleJointActiveState(JointID selected_joint_id);
	void ToggleJointPassiveStateAlt(JointID selected_joint_id);
	void ToggleJointPassiveState(JointID selected_joint_id);
	void CycleJointStateAlt(JointID selected_joint_id);
	void CycleJointState(JointID selected_joint_id);
	void ReverseCycleJointStateAlt(JointID joint_id);
    void ReverseCycleJointState(JointID joint_id);

	void TriggerSelectedJointActiveStateAlt(dReal vel);
	void TriggerSelectedJointActiveState(dReal vel);

	void ToggleSelectedJointActiveStateAlt(dReal vel);
	void ToggleSelectedJointActiveStateAlt();
	void ToggleSelectedJointActiveState(dReal vel);
	void ToggleSelectedJointActiveState();
	void ToggleSelectedJointPassiveStateAlt();
	void ToggleSelectedJointPassiveState();

	void ToggleBodyState(BodyID body_id);
	void ToggleSelectedBodyState();
	void ToggleSelectedPlayerBodyStates();

	void CycleSelectedJointStateAlt();
	void CycleSelectedJointState();
	void ReverseCycleSelectedJointStateAlt();
	void ReverseCycleSelectedJointState();

	void UndoSelectedPlayerMove();

	void Update(dReal dt);

	void DrawContacts(bool freeze);
	void DrawFloor();
    void DrawPlane(EnvPlane& plane);

    void DrawPlayerJoint(Joint j, vec4<dReal> q, vec3<dReal> p, raylib::Color color, bool draw_state);
	void DrawPlayerBody(Body b, vec4<dReal> q, vec3<dReal> p, raylib::Color color);

	void DrawPlayer(PlayerID pID, raylib::Color j_color, raylib::Color b_color);
	void DrawPlayerFreeze(PlayerID pID);
	void DrawPlayerGhostCache(PlayerID pID, uint32_t frame);
	
	bool GhostCacheEnabled();
	bool GhostCacheIsReady();
	void ToggleGhostCache();
	void ResetGhostCache();
	
	bool ReplayCacheEnabled();
	bool ReplayCacheIsReady();
	void ResetReplayCache();
	void ToggleReplayCache();
	
	bool TurnFrameGhostEnabled();
	void ToggleTurnFrameGhost();

	void Draw(raylib::Camera3D camera);

	void EnterMode(Gamemode mode, bool reset);

	void Step(int frame_count);
	void Freeze();
	void Refreeze();

    void attachContact(dUserData* data, dBodyID b1, dBodyID b2);

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
private:
	Game();
	~Game();
};

namespace Window
{
	static bool fullscreen_mode = false;

	static float  width = 800; 
	static float height = 450;

	static raylib::RenderTexture background;
	static raylib::RenderTexture foreground;

	void Init();
	void Close();
    void GetSettings();
	void Update();

	void RenderBackground(raylib::Camera3D camera);
	void RenderForeground(raylib::Camera3D camera);

	void Draw();

	float GetWidth ();
	float GetHeight();
};

namespace ResourceManager
{
    enum DefaultModels
    {
        SPHERE,
        SPHERE_SLICE,
	    BOX,
	    CAPSULE,
        COUNT,
    };
	
	static raylib::Shader shaders[8] = { 0 };
    static size_t shader_count =  0;
	
    static raylib::Mesh meshes[8] = { 0 };
    static size_t mesh_count =  0;
	
	static raylib::Model models[8] = { 0 };
    static size_t model_count = 0;
	  
	static raylib::Texture textures[8] = { 0 };
	static size_t texture_count = 0;

	void Init();

	int GenMeshPlane(float width, float length, int resX, int resZ);
	int LoadModelFromMesh(int mesh_id);

    int LoadShader(const char* vs, const char* fs);
	int LoadModel(const char* model_path);
    int LoadTexture(const char* texture_path);

	raylib::Shader GetShader(uint32_t id);
	raylib::Mesh GetMesh(uint32_t id);
	raylib::Model GetModel(uint32_t id);
	raylib::Texture GetTexture(uint32_t id);

	void SetModelTexture(uint32_t model_id, uint32_t texture_id);
	void SetModelShader(uint32_t model_id, uint32_t shader_id);

	void DrawTexture(uint32_t texture_id, int posX, int posY, raylib::Color tint);
	void DrawModel(uint32_t model_id);

	void DestroyAll();
};

namespace Replay 
{
	static std::string mod;

	static Arena* data = nullptr;
	static Arena* cache = nullptr;

	static size_t chunk = 0;
	static size_t chunk_count = 0;
	static uint32_t max_frames = 0;

	void Init();
	void Close();
	
	void Reset();
	void Begin();

	void WriteMetaData();
	void WriteFrameData(std::string data);
	void WriteReplayData(std::string data);
	
	bool CacheReady();

	void RecordFrame(int game_frame);
	//void RecallFrame
	void PlayFrame(int game_frame);

	void Import(std::string replay_name);
	void Export(std::string replay_name);

	void Destroy();

	std::string GetMod();

	size_t GetMaxFrame();
}

typedef void (*InputCallback)(void);

struct MouseEvent
{
  uint8_t freeze;
  uint8_t shift;
  uint8_t ctrl;
  uint8_t btn;
  
  InputCallback fn;
};

struct KeyEvent
{
  uint8_t freeze;
  uint8_t shift;
  uint8_t ctrl;
  uint8_t key;

  InputCallback fn;
};

namespace InputManager
{
  static KeyEvent* key_events[4];
  static uint32_t key_event_count = 0;

  void Init();
  void Update();
  
  MouseEvent* RegisterMouseEvent(uint8_t freeze, uint8_t ctrl, uint8_t shift, uint8_t btn, InputCallback fn);
  KeyEvent* RegisterKeyEvent(uint8_t freez, uint8_t ctrl, uint8_t shift, uint8_t key, InputCallback fn);
}
