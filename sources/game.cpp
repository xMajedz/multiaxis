#include "game.h"
#include "camera.h"
#include "luau.h"
#include "api.h"

using namespace raylib;
#include "raymath.h"
#include "rlgl.h"

void Game::Init()
{
    GetSettings();

    dInitODE();
	
    cache = new Arena(cache_size);
	mod_data = new Arena(2 * 1024 * 1024);
	
    Replay::Init();

    state.time = GetTime();
    state.running = true;
}

void Game::GetSettings()
{
    std::ifstream file("settings.txt");
	
	if (file.is_open()) {
	    std::string line;

		while (std::getline(file, line)) {
		    size_t s = line.find('=');
			std::string setting = line.substr(0, s);
			std::string value   = line.substr(s + 1);
			if (setting == "ghostcache") {
			    ghost_cache_enabled = (bool)std::stoi(value);
			} else if (setting == "replaycache") {
			    replay_cache_enabled = (bool)std::stoi(value);
			} else if (setting == "turnframe_ghost") {
			    turnframe_ghost = (bool)std::stoi(value);
			}
		}

		file.close();
	}
}

void Game::Reset()
{
    if (space != nullptr) {
        dSpaceDestroy(space);
        space = nullptr;
    }
    if (world != nullptr) {
	    dWorldDestroy(world);
		world = nullptr;
    }
    if (contactgroup != nullptr) {
        dJointGroupDestroy(contactgroup);
        contactgroup = nullptr;
    }
    if (objects.size() > 0) {
        objects.clear();
        o_count = 0;
    }
    if (joint_objects.size() > 0) {
        joint_objects.clear();
        jo_count = 0;
    }
    if (players.size() > 0) {
        players.clear();
        p_count = 0;
    }
}

void Game::ImportMod()
{
    rules = Api::GetRules();

    o_count = Api::GetObjectsCount();
    jo_count = Api::GetJointObjectsCount();

    p_count = Api::GetPlayersCount();

    objects.reserve(o_count);
    objects = Api::GetObjects();

    joint_objects.reserve(jo_count);
    joint_objects = Api::GetJointObjects();

    players.reserve(p_count);
    players = Api::GetPlayers();
}

void Game::NewGame()
{
    state.game_frame = 0;

    state.freeze = true;
    state.freeze_time = GetTime();
    state.freeze_frames = 50;
    state.freeze_frame = 0;
    state.freeze_count = 0;

    ghost_cache_frames = 0;

	world = dWorldCreate();
    dWorldSetERP(world, 0.45);
    dWorldSetCFM(world, 10E-2);

    step = 1.0f / 60.0f;

    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    floor = dCreatePlane(space, 0.00, 0.00, 1.00, 0.10);

    dGeomSetCategoryBits(floor, 0b0001);
    dGeomSetCollideBits(floor, 0b0000);

    dWorldSetGravity(world, rules.gravity.x, rules.gravity.y, rules.gravity.z);

    for (auto& o : objects) {
        o.Create(world, space);
    }

    for (auto& jo : joint_objects) {
        jo.Create(world, space, objects[jo.connections[0]], objects[jo.connections[1]]);
    }

    for (int pID = 0; pID < players.size(); pID += 1) {
        auto& p = players[pID];

        player_ghosts[pID] = 1;

        p.b_count = p.body.size();
        p.j_count = p.joint.size();

        p.SetCatBits(2<<(pID + 1), 2<<pID + 1);
        p.SetColBits(255-(2<<(pID + 1)), 255-(2<<pID + 1));

        p.Create(world, space);

        //p.SetEngagedistance(rules.engagedistance, pID * (360/rules.numplayers));
        //p.SetEngageheight(rules.engageheight);

        p.SetOffset();
    }

    state.running = true;

	ResetGhostCache();

	Replay::WriteMetaData();

    Api::NewGameCallback();
}

bool Game::Running()
{
	return state.running;
}

void Game::Stop()
{
    state.running = !state.running;
}

void Game::Quit()
{
    if (state.running) {
        Reset();
        dCloseODE();
    }

    Api::Close();

    Replay::Close();
}

static void attachContact(BodyUserData* data, dBodyID b1, dBodyID b2)
{
	using namespace Game;

	if (data != nullptr && data->contact_joint == nullptr && data->active) {
		data->contact_joint = dJointCreateFixed(world, 0);
		dJointAttach(data->contact_joint, b1, b2);
		dJointSetFixed(data->contact_joint);
	}

	if (data != nullptr && data->contact_joint != nullptr && !data->active) {
		dJointDestroy(data->contact_joint);
		data->contact_joint = nullptr;
	}
}

static void nearCallback(void*, dGeomID o1, dGeomID o2)
{
	using namespace Game;

	if (o1 == nullptr || o2 == nullptr) return;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if (dAreConnected(b1, b2)) return;

	if (b1 == b2) return;
	
	uint32_t cat1 = dGeomGetCategoryBits(o1);
	uint32_t col1 = dGeomGetCollideBits(o1);
	uint32_t cat2 = dGeomGetCategoryBits(o2);
    uint32_t col2 = dGeomGetCollideBits(o2);

	if (!(cat1 & col2 || cat2 & col1)) return;

	dContact contacts[rules.max_contacts];

	for (int i = 0; i < rules.max_contacts; i += 1) {
		contacts[i].surface = (dSurfaceParameters) {
			.mode = dContactApprox1|dContactBounce|dContactRolling,
			.mu = rules.friction,
			.rho = 0,
			.bounce = rules.bounce,
		};

		m_frame_contacts[i] = contacts[i];
	}

	numcontacts = dCollide(o1, o2, rules.max_contacts, &contacts->geom, sizeof(dContact));

	for (int i = 0; i < numcontacts; i += 1) {
		dJointID c = dJointCreateContact(world, contactgroup, &contacts[i]);
		dJointAttach(c, b1, b2);
	}

	numcollisions += 1;

	attachContact((BodyUserData*)dGeomGetData(o1), b1, b2);
	attachContact((BodyUserData*)dGeomGetData(o2), b1, b2);
}

void Game::Refreeze()
{
	state.freeze_count = 0;

	for (auto& o : objects) o.Refreeze();
	for (auto& p : players) p.Refreeze();
}

void Game::Restart()
{
	for (auto& o : objects) o.Reset();
	for (auto& p : players) p.Reset();
}

void Game::Freeze()
{
	state.freeze = true;
	state.freeze_time = GetTime();

	for (auto& o : objects) o.Freeze();
	for (auto& p : players) p.Freeze();
}

void Game::Step(int frame_count)
{
	bool ready = true;

	ghost_cache_frames = 0;

	switch(state.mode)
	{
	case SELF_PLAY:
		if (state.selected_player != -1) {
			players[state.selected_player].Ready();
		}

		for (auto& p : players) {
			if (!p.IsReady()) {
				state.selected_player = p.GetID();
				ready = false;
				break;
			}
		}
	case FREE_PLAY:
		if (ready) {
			Replay::RecordFrame(state.game_frame);

			state.freeze_frame = state.game_frame + frame_count;
			state.freeze = false;

			Refreeze();
		}
	}
}

bool Game::GhostCacheEnabled()
{
    return ghost_cache_enabled;
}

bool Game::GhostCacheIsReady()
{
    return ghost_cache_frames >= ghost_length;
}

void Game::ResetGhostCache()
{
    ghost_cache_offset = 0;
    ghost_cache_frames = 0;
}

bool Game::ReplayCacheEnabled()
{
    return replay_cache_enabled;
}

bool Game::ReplayCacheIsReady()
{
    return replay_cache_frames >= Replay::GetMaxFrame() + 100;
}

void Game::ResetReplayCache()
{
    replay_cache_frames = 0;
}

bool Game::TurnFrameGhostEnabled()
{
    return turnframe_ghost;
}

template <class T>
static void DrawObject(T o, Quaternion q, Vector3 p, Color color)
{
    using namespace Game;

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	const auto& camera = Gamecam::Get();
    auto shader = ResourceManager::GetShader(0);
	
	Vector4 normalizedColor = ColorNormalize(color);
	Vector3 objectColor = { normalizedColor.x, normalizedColor.y, normalizedColor.z };
    SetShaderValue(shader, GetShaderLocation(shader, "objectColor"), &objectColor, SHADER_UNIFORM_VEC3);
	SetShaderValue(shader, GetShaderLocation(shader, "objectAlpha"), &normalizedColor.w, SHADER_UNIFORM_FLOAT);

	BeginShaderMode(shader);
	BeginMode3D(camera);
	
	rlPushMatrix();
	rlTranslatef(
		    p.x,
		    p.y,
		    p.z
	);
	
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	/*
	 */

	switch(o.shape)
	{
	case BOX:
		DrawCube((Vector3){ 0.0f, 0.0f, 0.0f },
			 o.m_sides.x,
			 o.m_sides.y,
			 o.m_sides.z,
			 color);
		break;
	case SPHERE:
	    DrawSphere((Vector3){0.00, 0.00, 0.00}, o.radius, color);
	    break;
	case CAPSULE:
		DrawCapsule(
				(Vector3){ 0.0f, 0.0f, -(o.length/2) },
				(Vector3){ 0.0f, 0.0f,  (o.length/2) },
				o.radius,
				16,
				16,
				color
		);
		break;
	case CYLINDER:
		DrawCylinderEx(
				(Vector3){ 0.0f, 0.0f, -(o.length/2) },
				(Vector3){ 0.0f, 0.0f,  (o.length/2) },
				o.radius,
				o.radius,
				16,
				color
		);
		break;
	}
	
	/*
	 */

	rlPopMatrix();

	EndMode3D();
    EndShaderMode();
}

template<class T>
static void DrawObjectModel(T o, Quaternion q, Vector3 p, dReal s, Model model, Color color)
{
    using namespace Game;

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	rlPushMatrix();
	rlTranslatef(
		    p.x,
		    p.y,
		    p.z
	);
	
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	/*
	 */

	auto shader = ResourceManager::GetShader(0);

	auto viewPosition = Gamecam::GetOffset();
	SetShaderValue(shader, GetShaderLocation(shader, "viewPosition"), &viewPosition, SHADER_UNIFORM_VEC3);

	Vector4 normalizedColor = ColorNormalize(color);
	Vector3 objectColor = { normalizedColor.x, normalizedColor.y, normalizedColor.z };
    SetShaderValue(shader, GetShaderLocation(shader, "objectColor"), &objectColor, SHADER_UNIFORM_VEC3);
	SetShaderValue(shader, GetShaderLocation(shader, "objectAlpha"), &normalizedColor.w, SHADER_UNIFORM_FLOAT);

	for (int i = 0; i < ResourceManager::model_count; i += 1) {
	  ResourceManager::SetModelShader(i, 0);
	}

	switch(o.shape)
	{
	case BOX:
	      DrawModelEx(
				  model,
				  (Vector3){ 0.0f, 0.0f, 0.0f },
                  (Vector3){ 0.0f, 0.0f, 0.0f },
				  0.0f,
				  (Vector3){ s * o.m_sides.x, s * o.m_sides.y, s * o.m_sides.z },
				  color
		  );
		  break;
	case SPHERE:
	      DrawModel(model, (Vector3){0.0f, 0.0f, 0.0f}, s * o.radius, color);
	      break;
	case CAPSULE:
	      DrawModelEx(
				  model,
				  (Vector3){ 0.0f, 0.0f, 0.0f },
                  (Vector3){ 0.0f, 0.0f, 0.0f },
				  0.0f,
				  (Vector3){ 2 * s * o.radius, 2 * s * o.radius, s * (o.length + o.radius/2) },
				  color
	      );
		  break;
	case CYLINDER:
	      DrawModelEx(
				  model,
				  (Vector3){ 0.0f, 0.0f, 0.0f },
                  (Vector3){ 0.0f, 0.0f, 0.0f },
				  0.0f,
				  (Vector3){ 1.0, 1.0, 1.0 },
				  color
	      );
		  break;
	}

	/*
	 */

	rlPopMatrix();
}

static void RecordFrameToBuffer(uintptr_t buffer, size_t offset)
{
  	using namespace Game;

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	uint32_t player_offset[p_count] = { 0 };

	for (int pID = 0; pID < p_count; pID += 1) {
	    player_offset[pID] = sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total;
	  
		j_total += players[pID].j_count;
		b_total += players[pID].b_count;
	}

    //for (int oID = 0; oID < o_count;  oID +=1 )
	
    for (int pID = 0; pID < p_count; pID += 1) {
	    auto& p = players[pID];

		uint32_t j_offset = offset + player_offset[pID];

		for (int jID = 0; jID < p.j_count; jID += 1) {
			auto& j = p.joint[jID];

			auto* j_cache = (PlayerFrameJoint*)(buffer + j_offset + sizeof(PlayerFrameJoint) * jID);

			j_cache->position.x = j.frame_position.x;
			j_cache->position.y = j.frame_position.y;
			j_cache->position.z = j.frame_position.z;
			
			j_cache->orientation.x = j.frame_orientation.x;
			j_cache->orientation.y = j.frame_orientation.y;
			j_cache->orientation.z = j.frame_orientation.z;
			j_cache->orientation.w = j.frame_orientation.w;
		}
		
		uint32_t b_offset = offset + player_offset[pID] + sizeof(PlayerFrameJoint) * p.j_count;
		
		for (int bID = 0; bID < p.b_count; bID += 1) {
			auto& b = p.body[bID];
			
            auto* b_cache = (PlayerFrameBody*)(buffer + b_offset + sizeof(PlayerFrameBody) * bID);

			b_cache->position.x = b.frame_position.x;
			b_cache->position.y = b.frame_position.y;
			b_cache->position.z = b.frame_position.z;
			
			b_cache->orientation.x = b.frame_orientation.x;
			b_cache->orientation.y = b.frame_orientation.y;
			b_cache->orientation.z = b.frame_orientation.z;
			b_cache->orientation.w = b.frame_orientation.w;
		}
	}
}

static void RecordGhostCache()
{
    using namespace Game;
	
	uint32_t j_total = 0;
	uint32_t b_total = 0;

	uint32_t player_offset[p_count] = { 0 };

	for (int pID = 0; pID < p_count; pID += 1) {	  
		j_total += players[pID].j_count;
		b_total += players[pID].b_count;
	}

    frame_size = sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total;
  	
    RecordFrameToBuffer(cache->buffer(), ghost_cache_offset + ghost_cache_frames * frame_size);
	ghost_cache_frames += 1;
}

static void RecordReplayCache()
{
    using namespace Game;
		
	uint32_t j_total = 0;
	uint32_t b_total = 0;

	uint32_t player_offset[p_count] = { 0 };

	for (int pID = 0; pID < p_count; pID += 1) {	  
		j_total += players[pID].j_count;
		b_total += players[pID].b_count;
	}

    frame_size = sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total;
   
	RecordFrameToBuffer(cache->buffer(), replay_cache_frames * frame_size);
	replay_cache_frames += 1;
	
	ghost_cache_offset = frame_size * replay_cache_frames;
}

void Game::Update(dReal dt)
{
	Api::UpdateCallback(dt);

	if (!state.pause) {
		for (auto& o : objects) o.Step();
		for (auto& p : players) p.Step();

		if (!state.freeze) {
			switch (state.mode)
			{
			case SELF_PLAY: case FREE_PLAY:
			    if (ReplayCacheEnabled()) {
			        RecordReplayCache();
				}
						
				if (state.game_frame >= state.freeze_frame) {
					Freeze();
				} else {
					state.game_frame += 1;
				}
	
				break;
			case REPLAY_PLAY:
				size_t max_frame = Replay::GetMaxFrame();

				if (ReplayCacheEnabled() && !ReplayCacheIsReady()) {
				    RecordReplayCache();
				}
				
				if (state.game_frame >= max_frame + 100) {
                    Replay::Begin();

					EnterMode(REPLAY_PLAY, true);
				}

				if (!ReplayCacheEnabled() && (state.game_frame < max_frame)) {
				    Replay::PlayFrame(state.game_frame);
				}
				
				state.game_frame += 1;

				break;
			}

		} else {
			switch (state.mode)
			{
			case SELF_PLAY: case FREE_PLAY:
				if (state.freeze_count >= state.freeze_frames) {
					Refreeze();
				}
	
				if (0 < rules.reaction_time) {
					state.reaction_count = GetTime() - state.freeze_time;

					if (state.reaction_count >= rules.reaction_time) {
						Step(rules.turnframes);
					}
				}
				
                if (GhostCacheEnabled() && !GhostCacheIsReady()) {
					RecordGhostCache();
				}
	
				break;
			}
	
			state.freeze_count += 1;
		}

        if (space != nullptr) {
		  	numcollisions = 0;

			switch (state.mode)
			{
			case REPLAY_PLAY:
			    if (!(ReplayCacheEnabled() && ReplayCacheIsReady())) {
  				    dSpaceCollide(space, 0, nearCallback);
                    dWorldStep(world, step);
                    dJointGroupEmpty(contactgroup);
				}
			case REPLAY_EDIT: case FREE_PLAY: case SELF_PLAY:
			    if (!(GhostCacheEnabled() && GhostCacheIsReady())) {
			        dSpaceCollide(space, 0, nearCallback);
                    dWorldStep(world, step);
                    dJointGroupEmpty(contactgroup);
				}
            }
        }
	}
}

static PlayerFrameJoint* GetPlayerJointFromBuffer(uintptr_t buffer, size_t offset, size_t frame, PlayerID pID, JointID jID)
{
    using namespace Game;

	uint32_t j_total = 0;
    uint32_t b_total = 0;

	uint32_t player_offset[p_count] = { 0 };

    for (int i = 0; i < p_count; i += 1) {
	    player_offset[i] = sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total;    

		j_total += players[i].j_count;
        b_total += players[i].b_count;
    }

	uint32_t frame_offset = offset + frame * (sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total);
   	
	uint32_t j_offset = frame_offset + player_offset[pID];

    auto j_cache = (PlayerFrameJoint*)(buffer + j_offset + sizeof(PlayerFrameJoint) * jID);

	return j_cache;

}

static PlayerFrameBody* GetPlayerBodyFromBuffer(uintptr_t buffer, size_t offset, size_t frame, PlayerID pID, BodyID bID)
{
    using namespace Game;
	
	uint32_t j_total = 0;
    uint32_t b_total = 0;

	uint32_t player_offset[p_count] = { 0 };

    for (int i = 0; i < p_count; i += 1) {
	    player_offset[i] = sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total;    

		j_total += players[i].j_count;
        b_total += players[i].b_count;
    }

	uint32_t frame_offset = offset + frame * (sizeof(PlayerFrameJoint) * j_total + sizeof(PlayerFrameBody) * b_total);

	auto& p = players[pID];

	uint32_t b_offset = frame_offset + player_offset[pID] + sizeof(PlayerFrameJoint) * p.j_count;

    auto* b_cache = (PlayerFrameBody*)(buffer + b_offset + sizeof(PlayerFrameBody) * bID);

	return b_cache;
}

static PlayerFrameJoint* GetPlayerJointFromGhostCache(uint32_t frame, PlayerID pID, JointID jID)
{
    using namespace Game;
    return GetPlayerJointFromBuffer(cache->buffer(), ghost_cache_offset, frame, pID, jID);
}

static PlayerFrameBody* GetPlayerBodyFromGhostCache(uint32_t frame, PlayerID pID, BodyID bID)
{
    using namespace Game;
    return GetPlayerBodyFromBuffer(cache->buffer(), ghost_cache_offset, frame, pID, bID);
}

static PlayerFrameJoint* GetPlayerJointFromReplayCache(uint32_t frame, PlayerID pID, JointID jID)
{
    using namespace Game;
    return GetPlayerJointFromBuffer(cache->buffer(), 0, frame, pID, jID);
}

static PlayerFrameBody* GetPlayerBodyFromReplayCache(uint32_t frame, PlayerID pID, BodyID bID)
{
    using namespace Game;
    return GetPlayerBodyFromBuffer(cache->buffer(), 0, frame, pID, bID);
}

/*
 * Draw
 */

void Game::DrawPlayerJoint(Joint j, vec4 j_q, vec3 j_p, Color color, bool draw_state)
{
    Quaternion q = { j_q.x, j_q.y, j_q.z, j_q.w };

	Vector3 p = { j_p.x, j_p.y, j_p.z };

	Vector3 dir = { 1.00, 0.00, 0.00 };
	
	Vector3 axis = { j.axis.x, j.axis.y, j.axis.z };
    axis = Vector3Normalize(axis);
	
    Quaternion q1 = QuaternionFromVector3ToVector3(dir, axis);
	q1 = QuaternionMultiply(q, q1);

	Model sphere = ResourceManager::GetModel(ResourceManager::SPHERE);
    Model sphere_slice = ResourceManager::GetModel(ResourceManager::SPHERE_SLICE);

	if (!draw_state) {
      DrawObjectModel(j, q1, p, 1.00, sphere, color);
	  return;
	}
	
	switch (j.state)
	{
	case HOLD:
	  DrawObjectModel(j, q1, p, 1.00, sphere, color);
	  break;
	case BACKWARD:
	  q = QuaternionMultiply(QuaternionFromMatrix(MatrixRotateX(DEG2RAD * 180)), q);
	  
	  dir = { -1.0, 0.0, 0.0 };
	  axis = { -j.axis.x, -j.axis.y, -j.axis.z };
      axis = Vector3Normalize(axis);
	
      q1 = QuaternionFromVector3ToVector3(dir, axis);
	  q1 = QuaternionMultiply(q, q1);
    case FORWARD:
	  DrawObjectModel(j, q1, p, 1.00, sphere_slice, color);
	default:
	  DrawObjectModel(j, q1, p, 0.90, sphere, ColorBrightness(color, 0.50));
    }
}

void Game::DrawPlayerBody(Body b, vec4 b_q, vec3 b_p, Color color)
{
    Quaternion q = { b_q.x, b_q.y, b_q.z, b_q.w };

	Vector3 p = { b_p.x, b_p.y, b_p.z };

	switch (b.shape)
	{
	case BOX:
	  DrawObjectModel(b, q, p, 1.00, ResourceManager::GetModel(ResourceManager::BOX), color);
	  break;
	case SPHERE:
	  DrawObjectModel(b, q, p, 1.00, ResourceManager::GetModel(ResourceManager::SPHERE), color);
	  break;
	case CYLINDER:
	  DrawObjectModel(b, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), color);
	  break;
	case CAPSULE:
	  DrawObjectModel(b, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), color);
	  break;
	}
}

void Game::DrawFloor()
{
    float angle;
    Vector3 axis;
    QuaternionToAxisAngle(QuaternionFromMatrix(MatrixRotateX(DEG2RAD*90)), &axis, &angle);

    rlPushMatrix();
    rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);
    DrawGrid(2, 20);
    rlPopMatrix();
}

void Game::Draw(Camera3D camera)
{
	for (int oID = 0; oID < objects.size(); oID += 1) {
	    auto& o = objects[oID];
		
		Quaternion q = { 0 };
		Vector3 p = { 0 };
        BeginMode3D(camera);
		if (state.freeze) {
		    q.x = o.freeze_orientation.x;
			q.y = o.freeze_orientation.y;
			q.z = o.freeze_orientation.z;
		    q.w = o.freeze_orientation.w;

            p.x = o.freeze_position.x;
            p.y = o.freeze_position.y;

			p.z = o.freeze_position.z;

			switch (o.shape)
	{
	case BOX:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::BOX), o.m_color);
	  break;
	case SPHERE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::SPHERE), o.m_color);
	  break;
	case CYLINDER:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_color);
	  break;
	case CAPSULE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_color);
	  break;
	}
			
		    q.x = o.frame_orientation.x;
			q.y = o.frame_orientation.y;
			q.z = o.frame_orientation.z;
		    q.w = o.frame_orientation.w;

            p.x = o.frame_position.x;
            p.y = o.frame_position.y;
            p.z = o.frame_position.z;

			switch (o.shape)
	{
	case BOX:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::BOX), o.m_g_color);
	  break;
	case SPHERE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::SPHERE), o.m_g_color);
	  break;
	case CYLINDER:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_g_color);
	  break;
	case CAPSULE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_g_color);
	  break;
	}
	    } else {
		    Quaternion q = {
                o.frame_orientation.x,
                o.frame_orientation.y,
                o.frame_orientation.z,
                o.frame_orientation.w,
            };

            Vector3 p = {
                o.frame_position.x,
                o.frame_position.y,
                o.frame_position.z,
            };
switch (o.shape)
	{
	case BOX:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::BOX), o.m_color);
	  break;
	case SPHERE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::SPHERE), o.m_color);
	  break;
	case CYLINDER:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_color);
	  break;
	case CAPSULE:
	  DrawObjectModel(o, q, p, 1.00, ResourceManager::GetModel(ResourceManager::CAPSULE), o.m_color);
	  break;
	}
	    }
		EndMode3D();
    }

    for (PlayerID pID = 0; pID < players.size(); pID += 1) {
	    auto& p = players[pID];
	    BeginMode3D(camera);
	    for (JointID jID = 0; jID < p.j_count; jID += 1) {
		    auto& j = p.joint[jID];
            //BeginMode3D(camera);
			if (state.freeze) {
			    DrawPlayerJoint(j, j.freeze_orientation, j.freeze_position, p.m_j_color, true);

		        if (GhostCacheEnabled() && GhostCacheIsReady()) {
				    if (ghost_cache_frames >= ghost_length) {
					    auto* cache_joint = GetPlayerJointFromGhostCache(state.freeze_count, pID, jID);
						DrawPlayerJoint(j, cache_joint->orientation, cache_joint->position, p.m_g_color, false);
				    }
					
					if (TurnFrameGhostEnabled() && ghost_cache_frames >= rules.turnframes) {
					    auto* cache_joint = GetPlayerJointFromGhostCache(rules.turnframes, pID, jID);	
						DrawPlayerJoint(j, cache_joint->orientation, cache_joint->position, p.m_g_color, false);
					}
		       
				} else {
				    DrawPlayerJoint(j, j.frame_orientation, j.frame_position, p.m_g_color, false);
				}
		    } else {
			    if (ReplayCacheEnabled()) {
				    auto* cache_joint = GetPlayerJointFromReplayCache(state.game_frame, pID, jID);
			        DrawPlayerJoint(j, cache_joint->orientation, cache_joint->position, p.m_j_color, false);
			    } else {
				    DrawPlayerJoint(j, j.frame_orientation, j.frame_position, p.m_j_color, true);
			    }
			}
			//EndMode3D();
	    }
	  
	    for (BodyID bID = 0; bID < p.b_count; bID += 1) {
		    auto& b = p.body[bID];
			//BeginMode3D(camera);
		    if (state.freeze) {
			    if (b.active) { 
			        DrawPlayerBody(b, b.freeze_orientation, b.freeze_position, p.m_j_color);
		        } else {
				    DrawPlayerBody(b, b.freeze_orientation, b.freeze_position, p.m_b_color);
				}
				
		        if (GhostCacheEnabled() && GhostCacheIsReady()) {	
				    if (ghost_cache_frames >= ghost_length) {
					  auto* cache_body = GetPlayerBodyFromGhostCache(state.freeze_count, pID, bID);
					  DrawPlayerBody(b, cache_body->orientation, cache_body->position, p.m_g_color);
				    }
					
					if (TurnFrameGhostEnabled() && ghost_cache_frames >= rules.turnframes) {
					  auto* cache_body = GetPlayerBodyFromGhostCache(rules.turnframes, pID, bID);
					  DrawPlayerBody(b, cache_body->orientation, cache_body->position, p.m_g_color);
                    }
		        } else {
				    DrawPlayerBody(b, b.frame_orientation, b.frame_position, p.m_g_color);
				}
		    } else {
			    if (ReplayCacheEnabled()) {
				    auto* cache_body = GetPlayerBodyFromReplayCache(state.game_frame, pID, bID);
				    DrawPlayerBody(b, cache_body->orientation, cache_body->position, p.m_b_color);
			    } else {
			        DrawPlayerBody(b, b.frame_orientation, b.frame_position, p.m_b_color);
				}
			}
			//EndMode3D();
	    }
		EndMode3D();
	}
	
    BeginMode3D(camera);
	
    if (state.freeze && state.selected_player != -1 && state.selected_joint != -1) {
	    auto& j = players[state.selected_player].joint[state.selected_joint];
	    DrawPlayerJoint(j, j.freeze_orientation, j.freeze_position, j.m_select_color, false);
    }

    DrawFloor();
    
	Api::Draw3DCallback();
	
	EndMode3D();
}

size_t Game::GetContactCount()
{
	return numcollisions;
}

void Game::SetBackgroundColor(uint16_t r, uint16_t g, uint16_t b, uint16_t a)
{
	background_color = {r, g, b, a};
}

void Game::SetGameFrame(uint32_t frame)
{
    state.game_frame = frame;
}

void Game::SetGravity(dReal x, dReal y, dReal z)
{
    rules.gravity = {x, y , z};
  	dWorldSetGravity(world, x, y, z);
}

void Game::SetMaxContacts(size_t count)
{
	rules.max_contacts = count;
}

void Game::SetFriction(dReal friction)
{
	rules.friction = friction;
}

void Game::SetBounce(dReal bounce)
{
	rules.bounce = bounce;
}

void Game::SetTurnFrames(size_t frames)
{
	rules.turnframes = frames;
}

void Game::SetReactionTime(size_t t)
{
	rules.reaction_time = t;
}

bool Game::GetPause()
{
	return state.pause;
}

bool Game::GetFreeze()
{
	return state.freeze;
}

Gamemode Game::GetGamemode()
{
	return state.mode;
}

Gamerules& Game::GetGamerules()
{
	return rules;
}

std::string_view Game::GetMod()
{
	return rules.mod;
}

size_t Game::GetMaxContacts()
{
	return rules.max_contacts;
}

size_t Game::GetGameFrame()
{
	return state.game_frame;
}

dReal Game::GetReactionTime()
{
	return rules.reaction_time;
}

dReal Game::GetReactionCount()
{
	return state.reaction_count;
}

size_t Game::GetObjectCount()
{
	return objects.size();
}

size_t Game::GetPlayerCount()
{
	return players.size();
}

size_t Game::GetPlayerBodyCount(PlayerID player_id)
{
	return players[player_id].body.size();
}

size_t Game::GetPlayerJointCount(PlayerID player_id)
{
	return players[player_id].joint.size();
}

std::vector<Body> Game::GetObjects()
{
	return objects;
}

Player& Game::GetPlayer(PlayerID player_id)
{
	return players[player_id];
}

Player& Game::GetSelectedPlayer()
{
	return players[state.selected_player];
}

PlayerID Game::GetSelectedPlayerID()
{
	return state.selected_player;
}

Joint& Game::GetJoint(PlayerID player_id, JointID joint_id)
{
	return players[player_id].joint[joint_id];
}

Body& Game::GetBody(PlayerID player_id, BodyID body_id)
{
	return players[player_id].body[body_id];
}

Joint& Game::GetSelectedJoint()
{
	return players[state.selected_player].joint[state.selected_joint];
}

JointID Game::GetSelectedJointID()
{
	return state.selected_joint;
}

dReal Game::GetSelectedJointVelocity()
{
	return players[state.selected_player].joint[state.selected_joint].velocity;
}

dReal Game::GetSelectedJointVelocityAlt()
{
	return players[state.selected_player].joint[state.selected_joint].velocity_alt;
}

std::vector<Player> Game::GetPlayers()
{
	return players;
}

double Game::GetFrameTime()
{
	return raylib::GetFrameTime();
}

double Game::GetTime()
{
	return raylib::GetTime() - state.time;
}

void Game::SetMode(Gamemode mode)
{
    state.mode = mode;
}

void Game::SetSelectedJoint()
{
	state.selected_joint = -1;
}

void Game::SetSelectedJoint(JointID joint_id)
{
	state.selected_joint = joint_id;
}

void Game::SetSelectedPlayer()
{
	state.selected_player = -1;
}

void Game::SetSelectedPlayer(PlayerID player_id)
{
	state.selected_player = player_id;
}

void Game::SetBodyState(PlayerID player_id, BodyID body_id, bool state)
{
	players[player_id].body[body_id].active = state;
}

void Game::TogglePause()
{
	state.pause = state.pause == false;
}

void Game::ToggleGhostCache()
{
    ghost_cache_enabled = ghost_cache_enabled == false;
}

void Game::ToggleTurnFrameGhost()
{
    turnframe_ghost = turnframe_ghost == false;
}

void Game::ToggleReplayCache()
{
    replay_cache_enabled = replay_cache_enabled == false;
}

void Game::ToggleGhosts()
{
  for (PlayerID pID = 0; pID < p_count; pID += 1) {
    if (state.selected_player == -1) {
      player_ghosts[pID] = player_ghosts[pID] != 0;
      //players[pID].ToggleGhost();
    } else if (pID != state.selected_player) {
      player_ghosts[pID] = player_ghosts[pID] != 0;
      //players[pID].ToggleGhost();
    }
  }
  
  Refreeze();
}


void Game::TogglePlayerPassiveStatesAlt(PlayerID player_id)
{
	players[player_id].TogglePassiveStatesAlt();

	ResetGhostCache();
}

void Game::TogglePlayerPassiveStates(PlayerID player_id)
{
	players[player_id].TogglePassiveStates();

	ResetGhostCache();
}

void Game::ToggleSelectedPlayerPassiveStatesAlt()
{
	players[state.selected_player].TogglePassiveStatesAlt();

	ResetGhostCache();
}

void Game::ToggleSelectedPlayerPassiveStates()
{
	players[state.selected_player].TogglePassiveStates();

	ResetGhostCache();
}

void Game::TriggerPlayerJointState(PlayerID player_id, JointID joint_id, JointState state)
{
	switch(state)
	{
	case RELAX:
		players[player_id].joint[joint_id].TriggerPassiveState(0.00);
		break;
	case HOLD:
		players[player_id].joint[joint_id].TriggerPassiveState(players[player_id].joint[joint_id].strength);
		break;
	case FORWARD:
		players[player_id].joint[joint_id].TriggerActiveState(1.00 * players[player_id].joint[joint_id].velocity);
		break; 
	case BACKWARD:
		players[player_id].joint[joint_id].TriggerActiveState(-1.00 * players[player_id].joint[joint_id].velocity);
		break;
	}
}

void Game::TriggerPlayerJointStateAlt(PlayerID player_id, JointID joint_id, JointState state)
{
	switch(state)
	{
	case RELAX:
		players[player_id].joint[joint_id].TriggerPassiveStateAlt(0.00);
		break;
	case HOLD:
		players[player_id].joint[joint_id].TriggerPassiveStateAlt(players[player_id].joint[joint_id].strength_alt);
		break;
	case FORWARD:
		players[player_id].joint[joint_id].TriggerActiveStateAlt(1.00 * players[player_id].joint[joint_id].velocity_alt);
		break; 
	case BACKWARD:
		players[player_id].joint[joint_id].TriggerActiveStateAlt(-1.00 * players[player_id].joint[joint_id].velocity_alt);
		break;
	}
}

void Game::TriggerPlayerJoint(PlayerID player_id, JointID joint_id, JointState state, dReal vel)
{
    players[player_id].joint[joint_id].state = state;
  
	switch(state)
	{
	case RELAX:
		players[player_id].joint[joint_id].TriggerPassiveState(0.00);
		break;
	case HOLD:
		players[player_id].joint[joint_id].TriggerPassiveState(players[player_id].joint[joint_id].strength);
		break;
	case FORWARD: case BACKWARD:
		players[player_id].joint[joint_id].TriggerActiveState(vel);
		break;
	}
}

void Game::TriggerPlayerJointAlt(PlayerID player_id, JointID joint_id, JointState state, dReal vel)
{
    players[player_id].joint[joint_id].state_alt = state;

	switch(state)
	{
	case RELAX:
		players[player_id].joint[joint_id].TriggerPassiveStateAlt(0.00);
		break;
	case HOLD:
		players[player_id].joint[joint_id].TriggerPassiveStateAlt(players[player_id].joint[joint_id].strength_alt);
		break;
	case FORWARD: case BACKWARD:
		players[player_id].joint[joint_id].TriggerActiveStateAlt(vel);
		break;
	}
}

void Game::UndoSelectedPlayerMove()
{
	Replay::PlayFrame(state.game_frame);

	ResetGhostCache();
}

void Game::ToggleBodyState(BodyID body_id)
{
	players[state.selected_player].body[body_id].ToggleState();

	ResetGhostCache();
}

void Game::ToggleSelectedBodyState()
{
	players[state.selected_player].body[state.selected_body].ToggleState();

	ResetGhostCache();
}

void Game::ToggleSelectedPlayerBodyStates()
{
	for (auto& b : players[state.selected_player].body)
	{
		if (b.m_interactive)
			b.ToggleState();
	}

	ResetGhostCache();
}

void Game::ToggleJointActiveStateAlt(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].ToggleActiveStateAlt();

	ResetGhostCache();
}

void Game::ToggleJointActiveState(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].ToggleActiveState();

	ResetGhostCache();
}

void Game::ToggleJointPassiveStateAlt(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].TogglePassiveStateAlt();

	ResetGhostCache();
}

void Game::ToggleJointPassiveState(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].TogglePassiveState();

	ResetGhostCache();
}

void Game::CycleJointStateAlt(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].CycleStateAlt();

	ResetGhostCache();
}

void Game::CycleJointState(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].CycleState();

	ResetGhostCache();
}

void Game::ReverseCycleJointStateAlt(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].ReverseCycleStateAlt();

	ResetGhostCache();
}

void Game::ReverseCycleJointState(JointID joint_id)
{
	players[state.selected_player].joint[joint_id].ReverseCycleState();

	ResetGhostCache();
}

void Game::TriggerSelectedJointActiveStateAlt(dReal vel)
{
	players[state.selected_player].joint[state.selected_joint].TriggerActiveStateAlt(vel);

	ResetGhostCache();
}

void Game::TriggerSelectedJointActiveState(dReal vel)
{
	players[state.selected_player].joint[state.selected_joint].TriggerActiveState(vel);

	ResetGhostCache();
}

void Game::ToggleSelectedJointActiveStateAlt(dReal vel)
{
	players[state.selected_player].joint[state.selected_joint].ToggleActiveStateAlt(vel);

	ResetGhostCache();
}

void Game::ToggleSelectedJointActiveState(dReal vel)
{
	players[state.selected_player].joint[state.selected_joint].ToggleActiveState(vel);

	ResetGhostCache();
}

void Game::ToggleSelectedJointActiveStateAlt()
{
	players[state.selected_player].joint[state.selected_joint].ToggleActiveStateAlt();

	ResetGhostCache();
}

void Game::ToggleSelectedJointActiveState()
{
	players[state.selected_player].joint[state.selected_joint].ToggleActiveState();

	ResetGhostCache();
}

void Game::ToggleSelectedJointPassiveStateAlt()
{
	players[state.selected_player].joint[state.selected_joint].TogglePassiveStateAlt();

	ResetGhostCache();
}

void Game::ToggleSelectedJointPassiveState()
{
	players[state.selected_player].joint[state.selected_joint].TogglePassiveState();

	ResetGhostCache();
}

void Game::CycleSelectedJointStateAlt()
{
	players[state.selected_player].joint[state.selected_joint].CycleStateAlt();

	ResetGhostCache();
}

void Game::CycleSelectedJointState()
{
	players[state.selected_player].joint[state.selected_joint].CycleState();

	ResetGhostCache();
}

void Game::ReverseCycleSelectedJointStateAlt()
{
	players[state.selected_player].joint[state.selected_joint].ReverseCycleStateAlt();

	ResetGhostCache();
}

void Game::ReverseCycleSelectedJointState()
{
	players[state.selected_player].joint[state.selected_joint].ReverseCycleState();

	ResetGhostCache();
}

static bool mouse_input = true;
static bool keyboard_input = true;

static int selected_object = -1;
static int selected_player = -1;
static int selected_joint = -1;
static int selected_body = -1;

void Window::Init()
{
    GetSettings();
	
    SetTraceLogLevel(LOG_ERROR);
	
	InitWindow(width, height, "MultiAxis");

	SetExitKey(KEY_NULL);

	SetTargetFPS(60);

	if (fullscreen_mode) ToggleFullscreen();
	
	Gamecam::Init();

	InputManager::Init();

	ResourceManager::Init();

	Game::background_color = BLACK;

	background = LoadRenderTexture(width, height);
	foreground = LoadRenderTexture(width, height);
}

void ResourceManager::Init()
{
    const char* DefaultModelList[] = {
        "resources/model/sphere.obj",
	    "resources/model/sphere-slice.obj",
	    "resources/model/box.obj",
	    "resources/model/capsule.obj",
    };

    for (int i = 0; i < ResourceManager::COUNT; i += 1) {
        ResourceManager::LoadModel(DefaultModelList[i]);
    }

	LoadShader("resources/shader/base.vs", "resources/shader/base.fs");
}

void ResourceManager::DestroyAll()
{
  for (int i = 0; i < shader_count; i += 1) {
	  UnloadShader(shaders[i]);
  }

  for (int i = 0; i < mesh_count; i += 1) {
	  UnloadMesh(meshes[i]);
  }

  for (int i = 0; i < model_count; i += 1) {
	  UnloadModel(models[i]);
  }
  
  for (int i = 0; i < texture_count; i += 1) {
      UnloadTexture(textures[i]);
  }
}

int ResourceManager::GenMeshPlane(float width, float length, int resX, int resZ)
{
    int id = mesh_count;
    meshes[id] = raylib::GenMeshPlane(width, length, resX, resZ);
	mesh_count += 1;
	return id;
}

int ResourceManager::LoadModelFromMesh(int mesh_id)
{
    int id = model_count;
    models[id] = raylib::LoadModelFromMesh(GetMesh(mesh_id));
	model_count += 1;
	return id;  
}

int ResourceManager::LoadShader(const char* vs, const char* fs)
{
    int id = shader_count;
    shaders[id] = raylib::LoadShader(vs, fs);
    shader_count += 1;
    return id;
}

int ResourceManager::LoadModel(const char* model_path)
{
    int id = model_count;
    models[id] = raylib::LoadModel(model_path);
	model_count += 1;
	return id;
}

int ResourceManager::LoadTexture(const char* texture_path)
{
    int id = texture_count;
    textures[id] = raylib::LoadTexture(texture_path);
	texture_count += 1;
	return id;
}

Shader ResourceManager::GetShader(uint32_t id)
{
     return shaders[id];
}

Mesh ResourceManager::GetMesh(uint32_t id)
{
     return meshes[id];
}

Model ResourceManager::GetModel(uint32_t id)
{
     return models[id];
}

Texture ResourceManager::GetTexture(uint32_t id)
{
     return textures[id];
}

void ResourceManager::SetModelShader(uint32_t model_id, uint32_t shader_id)
{
    models[model_id].materials[0].shader = shaders[shader_id];
}

void ResourceManager::SetModelTexture(uint32_t model_id, uint32_t texture_id)
{
    models[model_id].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textures[texture_id];
}

void ResourceManager::DrawTexture(uint32_t texture_id, int posX, int posY, Color tint)
{
    DrawTexture(GetTexture(texture_id), posX, posY, tint);
}

void ResourceManager::DrawModel(uint32_t model_id)
{
    DrawModelEx(GetModel(model_id), (Vector3){ 0 }, (Vector3){ 0.0, 1.0, 0.0 }, 180.0f, (Vector3){ 1.0f, 1.0f, 1.0f }, WHITE);
}

static void ActionTogglePause(void)
{
    Game::TogglePause();
}

void InputManager::Init()
{
  //key_events[0] = RegisterKeyEvent(2, 2, 2, KEY_P, ActionTogglePause);
}

void InputManager::Update()
{
    Api::UpdateHotKeys();
}

void Window::GetSettings()
{
    std::ifstream file("settings.txt");
	
	if (file.is_open()) {
	    std::string line;

		while (std::getline(file, line)) {
		    size_t s = line.find('=');
			std::string setting = line.substr(0, s);
			std::string value   = line.substr(s + 1);
			if (setting == "resolution") {
			    s = value.find(',');
				width  = std::stoi(value.substr(0, s));
				height = std::stoi(value.substr(s + 1));
			} else if (setting == "fullscreen") {
			    fullscreen_mode = (bool)std::stoi(value);
			}
		}

		file.close();
	}
}

template <class T>
static RayCollision CollideObject(Ray ray, T o)
{
	using namespace Game;

	RayCollision collision = { 0 };

	switch(o.shape)
	{
	case BOX:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					o.freeze_position.x - 0.5f * o.m_sides.x,
					o.freeze_position.y - 0.5f * o.m_sides.y,
					o.freeze_position.z - 0.5f * o.m_sides.z,
				},
				(Vector3){
					o.freeze_position.x + 0.5f * o.m_sides.x,
					o.freeze_position.y + 0.5f * o.m_sides.y,
					o.freeze_position.z + 0.5f * o.m_sides.z,
				},
			}
		);

		break;
	case SPHERE:
		collision = GetRayCollisionSphere(ray,
			(Vector3){
				o.freeze_position.x,
				o.freeze_position.y,
				o.freeze_position.z,
			},
			o.radius
		);

		break;
	case CAPSULE:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					o.freeze_position.x - 0.5f * o.m_sides.x,
					o.freeze_position.y - 0.5f * o.m_sides.y,
					o.freeze_position.z - 0.5f * o.m_sides.z,
				},
				(Vector3){
					o.freeze_position.x + 0.5f * o.m_sides.x,
					o.freeze_position.y + 0.5f * o.m_sides.y,
					o.freeze_position.z + 0.5f * o.m_sides.z,
				},
			}
		);

		break;
	case CYLINDER:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					o.freeze_position.x - 0.5f * o.m_sides.x,
					o.freeze_position.y - 0.5f * o.m_sides.y,
					o.freeze_position.z - 0.5f * o.m_sides.z,
				},
				(Vector3){
					o.freeze_position.x + 0.5f * o.m_sides.x,
					o.freeze_position.y + 0.5f * o.m_sides.y,
					o.freeze_position.z + 0.5f * o.m_sides.z,
				},
			}
		);

		break;
	}

	return collision;
}

static void gSelector(Camera3D camera)
{
    using namespace Game;

    Ray ray = GetMouseRay(GetMousePosition(), camera);

    RayCollision col1 = { 0 };
    RayCollision col2 = { 0 };
    
    for (BodyID oID = 0; oID < o_count; oID += 1) {
        col1 = CollideObject(ray, objects[oID]);

        if (col1.hit && (col2.distance == 0 || col2.distance > col1.distance)) {
            selected_object = oID;
        }
    }

    bool hit = false;

    for (PlayerID pID = 0; pID < p_count; pID += 1) {
        for (JointID jID = 0; jID < players[pID].j_count; jID += 1) {
            col1 = CollideObject(ray, players[pID].joint[jID]);

            if (col1.hit && (col2.distance == 0 || col2.distance > col1.distance)) {
                col2 = col1;

                selected_player = pID;
                selected_joint = jID;

                hit = true;
            }
        }

        if (state.selected_player != selected_player)
            selected_joint = -1;

        if (hit) break;

        selected_player = -1;
        selected_joint = -1;

        for (BodyID bID = 0; bID < players[pID].b_count; bID += 1) {
            col1 = CollideObject(ray, players[pID].body[bID]);

            if (col1.hit && (col2.distance == 0 || col2.distance > col1.distance)) {
                col2 = col1;

                selected_player = pID;
                selected_body = bID;

                hit = true;
            }
        }

        if (hit) break;

        selected_player = -1;
        selected_body = -1;
    }
}

void Window::Update()
{
	if (IsWindowResized()) {		
		width = GetScreenWidth();
		height = GetScreenHeight();

		UnloadRenderTexture(background);
		background = LoadRenderTexture(width, height);

		UnloadRenderTexture(foreground);
		foreground = LoadRenderTexture(width, height);
	}

	SetWindowTitle(TextFormat("MultiAxis %dFPS", GetFPS()));

	const auto& camera = Gamecam::Get();

	gSelector(camera);

	InputManager::Update();
	
	if (Game::GetSelectedPlayerID() != -1)
		Game::SetSelectedJoint(selected_joint);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		Game::SetSelectedPlayer(selected_player);

	if (0 > Game::GetSelectedPlayerID()) {
		Gamecam::UpdateSpectatorcam(Game::GetFreeze(), Game::GetPlayers());
	} else {
		Gamecam::UpdatePlaycam(Game::GetFreeze(), Game::GetSelectedPlayer());
	}

	Gamecam::Update();

	if (IsFileDropped()) {
		FilePathList files = LoadDroppedFiles();
		Api::FileDroppedCallback(files);
		UnloadDroppedFiles(files);
	}
}

void Window::RenderBackground(Camera3D camera)
{
	using namespace Game;

	BeginTextureMode(background);
	    ClearBackground(background_color);
		Game::Draw(camera);
	EndTextureMode();
}

void Window::RenderForeground(Camera3D camera)
{
	BeginTextureMode(foreground);
	    BeginMode3D(camera);
	        ClearBackground(Fade(BLACK, 0.10));
	    EndMode3D();
	EndTextureMode();
}

void Window::Draw()
{
    const auto& camera = Gamecam::Get();

	BeginDrawing();

	ClearBackground(RAYWHITE);

	DrawTextureRec(background.texture, {0, 0, width, -height}, {0, 0}, WHITE);
	DrawTextureRec(foreground.texture, {0, 0, width, -height}, {0, 0}, WHITE);	
	
	RenderBackground(camera);
	RenderForeground(camera);

	Api::DrawCallback();

	EndDrawing();
}

void Window::Close()
{
    ResourceManager::DestroyAll();
	
	UnloadRenderTexture(background);
	UnloadRenderTexture(foreground);
	
	CloseWindow();
}

float Window::GetWidth()
{
	return width;
}

float Window::GetHeight()
{
	return height;
}

void Game::EnterMode(Gamemode mode, bool reset)
{	
	auto prev_mode = FREE_PLAY;

	if (mode == REPLAY_EDIT) {
		prev_mode = mode;
	}
	
	if (reset) {
		state.game_frame = 0;

		Restart();

		for (auto& p : players) {
	     	p.RelaxAll();
	    	p.RelaxAllAlt();

		    for (auto& b : p.body) {
			    b.active = false;
		    }
	    }
	} else {
	    Refreeze();
	}

	state.reaction_count = 0;
	state.freeze_count = 0;
	
	switch(mode)
	{
	case REPLAY_PLAY:
		state.mode = mode;
		state.freeze = false;
		
		ResetGhostCache();

		break;
	case REPLAY_EDIT: case SELF_PLAY: case FREE_PLAY:
		if (mode == REPLAY_EDIT) {
			mode = prev_mode;
		}

		ResetReplayCache();
		
		Replay::Reset();
		Replay::Begin();

		state.mode = mode;

		Freeze();

		break;
	}
}

void Replay::Init()
{
	data = new Arena(8*1024*1024);
}

void Replay::Close()
{
	delete data;
}

void Replay::Destroy()
{
	chunk = 0;
	chunk_count = 0;
	data->clear();
}

void Replay::WriteMetaData()
{
	auto mod = Game::GetMod();
	auto p_count = Game::GetPlayerCount();
	std::string meta = "M ";
	std::string details = "";
	
	meta.append(TextFormat("%s %d %d", mod.data(), Game::GetObjectCount(), p_count));

	for (int i = 0; i < p_count; i += 1) {
		details.append(TextFormat(" %d %d", Game::GetPlayerJointCount(i), Game::GetPlayerBodyCount(i)));
	}

	meta.append(TextFormat("%s", details.data()));

	std::ofstream tempreplayfile("replays/tempreplayfile.txt");
	tempreplayfile << meta << std::endl;
	tempreplayfile.close();

	Destroy();

	auto buffer = (uint32_t*)data->allocate(sizeof(uint32_t) * (2 + 2 * p_count));

	buffer[0] = Game::GetObjectCount();
	buffer[1] = p_count;

	uint32_t o = 2;

	for (uint32_t i = 0; i < p_count; i += 1) {
		buffer[o + 0] = Game::GetPlayerJointCount(i);
		buffer[o + 1] = Game::GetPlayerBodyCount(i);
		o += 2;
	}
}

void Replay::WriteFrameData(std::string data)
{
	auto mod = Game::GetMod();
	auto p_count = Game::GetPlayerCount();
	std::string meta = "M ";
	std::string details = "";
	
	meta.append(TextFormat("%s %d %d", mod.data(), Game::GetObjectCount(), p_count));

	for (int i = 0; i < p_count; i += 1) {
		details.append(TextFormat(" %d %d", Game::GetPlayerJointCount(i), Game::GetPlayerBodyCount(i)));
	}

	meta.append(TextFormat("%s\n%s", details.data(), data.data()));

	std::ofstream tempframefile("replays/tempframefile.txt");
	tempframefile << meta << std::endl;
	tempframefile.close();
}

void Replay::WriteReplayData(std::string data)
{
	std::ofstream tempreplayfile("replays/tempreplayfile.txt", std::ios::app);
	tempreplayfile << data << std::endl;
	tempreplayfile.close();
}

bool Replay::CacheReady()
{
	return false;
}

void Replay::RecordFrame(int game_frame)
{
	using namespace Game;

	std::string tempframe = "F ";
	tempframe.append(TextFormat("%d\n", game_frame));

	max_frames = game_frame + rules.turnframes;

	uint32_t* buffer = (uint32_t*)data->buffer();
	uint32_t o_count = buffer[0];
	uint32_t p_count = buffer[1];

	uint32_t p_offset = 2;

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	for (uint32_t p_id = 0; p_id < p_count; p_id += 1) {
		j_total += buffer[p_offset + 0];
		b_total += buffer[p_offset + 1];
		p_offset += 2;
	}

	uint32_t chunk_size = 5 * p_count * (j_total + b_total) * sizeof(uint32_t);

	buffer[p_offset + chunk_size * chunk_count] = (uint32_t)game_frame;

	uint32_t chunk_start = sizeof(uint32_t) * (p_offset + 1) + chunk_size * chunk_count;

	std::string Js = "Js";
	std::string Jv = "Jv";

	std::string B = "B";
	std::string P = "P";
	std::string Q = "Q";
	std::string L = "L";
	std::string A = "A";

	for (uint32_t p_id = 0; p_id < p_count; p_id += 1) {
		uint32_t* p_buffer = (buffer + 2);

		uint32_t j_count = p_buffer[p_id * p_count + 0];

		for (uint32_t j_id = 0; j_id < j_count; j_id += 1) {
			const auto& j = Game::GetJoint(p_id, j_id);
			uint8_t* state_buffer = (uint8_t*)(buffer + (chunk_start + p_id * j_count));
			uint8_t state_byte = j.state + (j.state_alt << 2);
			state_buffer[j_id] = state_byte;

			double* vel_buffer = (double*)(buffer + (chunk_start + p_count * j_total + 4 * p_id * j_count));
			vel_buffer[2 * j_id + 0] = j.frame_vel;
			vel_buffer[2 * j_id + 1] = j.frame_vel_alt;

			Js.append(TextFormat(" %d", state_byte));
			Jv.append(TextFormat(" %f %f", j.frame_vel, j.frame_vel_alt));
		}

		uint32_t b_count = p_buffer[p_id * p_count + 1];
		
		for (uint32_t b_id = 0; b_id < b_count; b_id += 1) {
			const auto& b = Game::GetBody(p_id, b_id);
			uint8_t* state_buffer = (uint8_t*)(buffer + (chunk_start + 5 * p_count * j_total + p_id * b_count));
			state_buffer[b_id] = (uint8_t)b.active;

			B.append(TextFormat(" %d", b.active));

			/*
			Q.append(TextFormat(" %f %f %f %f",
				b.frame_orientation.w,
				b.frame_orientation.x,
				b.frame_orientation.y,
				b.frame_orientation.z
			));

			P.append(TextFormat(" %f %f %f",
				b.frame_position.x,
				b.frame_position.y,
				b.frame_position.z
			));

			L.append(TextFormat(" %f %f %f",
				b.frame_linear_vel.x,
				b.frame_linear_vel.y,
				b.frame_linear_vel.z
			));

			A.append(TextFormat(" %f %f %f",
				b.frame_angular_vel.x,
				b.frame_angular_vel.y,
				b.frame_angular_vel.z
			));
			*/
		}
	}

	chunk_count += 1;

	tempframe.append(TextFormat("%s\n", Js.c_str()));
	tempframe.append(TextFormat("%s\n", Jv.c_str()));

	tempframe.append(TextFormat("%s", B.c_str()));
	//tempframe.append(TextFormat("%s\n", P.c_str()));
	//tempframe.append(TextFormat("%s\n", Q.c_str()));
	//tempframe.append(TextFormat("%s\n", L.c_str()));
	//tempframe.append(TextFormat("%s\n", A.c_str()));

	WriteFrameData(tempframe);
	WriteReplayData(tempframe);
}

void Replay::Reset()
{
	chunk = 0;
	chunk_count = 0;
}

void Replay::Begin()
{
	chunk = 0;
}

void Replay::PlayFrame(int game_frame)
{
	if (chunk_count < 1) return;

	uint32_t* buffer = (uint32_t*)data->buffer();
	uint32_t o_count = buffer[0];
	uint32_t p_count = buffer[1];

	uint32_t p_offset = 2;

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	for (uint32_t p_id = 0; p_id < p_count; p_id += 1) {
		j_total += buffer[p_offset + 0];
		b_total += buffer[p_offset + 1];
		p_offset += 2;
	}

	uint32_t chunk_size = 5 * p_count * (j_total + b_total) * sizeof(uint32_t);

	uint32_t chunk_frame = buffer[p_offset + chunk_size * chunk];

	if (chunk_frame != game_frame) return;

	uint32_t chunk_start = sizeof(uint32_t) * (p_offset + 1) + chunk_size * chunk;

	for (uint32_t p_id = 0; p_id < p_count; p_id += 1) {
		uint32_t* p_buffer = (buffer + 2);

		uint32_t j_count = p_buffer[p_id * p_count + 0];

		for (uint32_t j_id = 0; j_id < j_count; j_id += 1) {
			const auto& state_buffer = (uint8_t*)(buffer + (chunk_start + p_id * j_count));
			uint8_t state_byte = state_buffer[j_id];
			uint8_t state_alt = state_byte >> 2;
			uint8_t state = state_byte - (state_alt << 2);
			
			double* vel_buffer = (double*)(buffer + (chunk_start + p_count * j_total + 4 * p_id * j_count));
			Game::TriggerPlayerJoint(p_id, j_id, (JointState)state, vel_buffer[2 * j_id + 0]);
			Game::TriggerPlayerJointAlt(p_id, j_id, (JointState)state_alt, vel_buffer[2 * j_id + 1]);
		}

		uint32_t b_count = p_buffer[p_id * p_count + 1];

		for (uint32_t b_id = 0; b_id < b_count; b_id += 1) {
			uint8_t* state_buffer = (uint8_t*)(buffer + (chunk_start + 5 * p_count * j_total + p_id * b_count));
            bool state = (bool)state_buffer[b_id];
			Game::SetBodyState(p_id, b_id, state);
			//LOG(b_id << " " << (int)state_buffer[b_id])

			//auto& Qw = *((double*)p.Q + b_id * 4 + 0);
			//auto& Qx = *((double*)p.Q + b_id * 4 + 1);
			//auto& Qy = *((double*)p.Q + b_id * 4 + 2);
			//auto& Qz = *((double*)p.Q + b_id * 4 + 3);

			//auto& Px = *((double*)p.P + b_id * 3 + 0);
			//auto& Py = *((double*)p.P + b_id * 3 + 1);
			//auto& Pz = *((double*)p.P + b_id * 3 + 2);

			//auto& Lx = *((double*)p.L + b_id * 3 + 0);
			//auto& Ly = *((double*)p.L + b_id * 3 + 1);
			//auto& Lz = *((double*)p.L + b_id * 3 + 2);

			//auto& Ax = *((double*)p.A + b_id * 3 + 0);
			//auto& Ay = *((double*)p.A + b_id * 3 + 1);
			//auto& Az = *((double*)p.A + b_id * 3 + 2);

			//Game::SetBodyLinearVel(p_id, b_id, p.L[p_id*b_id], p.L[p_id * b_id + 1], p.L[p_id * b_id + 2]);
			//Game::SetBodyAngularVel(p_id, b_id, p.A[p_id*b_id], p.A[p_id * (b_id + 1)], p.A[p_id * (b_id + 2)]);
			//dBodySetLinearVel(b.dBody, b.frame_linear_vel.x, b.frame_linear_vel.y, b.frame_linear_vel.z);
			//dBodySetAngularVel(b.dBody, b.frame_angular_vel.x, b.frame_angular_vel.y, b.frame_angular_vel.x);
		}
	}

	chunk += 1;
}

void Replay::Import(std::string replay_name)
{
	std::string replay = "replays/";
	replay.append(replay_name);

	std::ifstream savedreplayfile(replay, std::ios::binary);

	char c;

	char mod_name[1024] = { 0 };

	for (int i = 0; savedreplayfile.get(c); i += 1) {
		if (c == '\0') {
			break;
		}

		mod_name[i] = c;
	}
	
	mod = mod_name;

	uint8_t max_frames_buffer[4];

	for (int i = 0; i < 4; i += 1) {
		savedreplayfile.get(c);
		max_frames_buffer[i] = c;
	}

	max_frames = *((uint32_t*)max_frames_buffer);

	uint8_t chunk_count_buffer[4];

	for (int i = 0; i < 4; i += 1) {
		savedreplayfile.get(c);
		chunk_count_buffer[i] = c;
	}

	chunk_count = *((uint32_t*)chunk_count_buffer);

	auto buffer = (uint8_t*)data->buffer();

	int i = 0;

	for (i = 0; savedreplayfile.get(c); i += 1) {
		buffer[i] = (uint8_t)c;
	}

	savedreplayfile.close();
}

void Replay::Export(std::string replay_name)
{
	std::string replay = "replays/";
	replay.append(replay_name);

	std::ofstream savedreplayfile(replay.append(".rpl"), std::ios::binary);

	auto mod = Game::GetMod();
	
	for (int i = 0; i < mod.size(); i += 1) {
		if (mod.data()[i] != '\0') {
			savedreplayfile << mod.data()[i];
		}
	}

	savedreplayfile << '\0';

	uint8_t* max_frames_buffer = (uint8_t*)&max_frames;

	for (int i = 0; i < 4; i += 1) {
		savedreplayfile << max_frames_buffer[i];
	}

	uint8_t* chunk_count_buffer = (uint8_t*)&chunk_count;

	for (int i = 0; i < 4; i += 1) {
		savedreplayfile << chunk_count_buffer[i];
	}

	auto buffer_u32  = (uint32_t*)data->buffer();

	uint32_t o_count = buffer_u32[0];
	uint32_t p_count = buffer_u32[1];

	uint32_t p_offset = 2;

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	for (uint32_t p_id = 0; p_id < p_count; p_id += 1) {
		j_total += buffer_u32[p_offset + 0];
		b_total += buffer_u32[p_offset + 1];
		p_offset += 2;
	}

	auto buffer = (uint8_t*)data->buffer();

	uint32_t chunk_size = 5 * p_count * (j_total + b_total) * sizeof(uint32_t);

	for (int i = 0; i < 2 + 2 * p_count + chunk_size * chunk_count; i += 1) {
		savedreplayfile << buffer[i];
	}

	savedreplayfile.close();
}

std::string_view Replay::GetMod()
{
	return mod;
}

size_t Replay::GetMaxFrame()
{
	return max_frames;
}
