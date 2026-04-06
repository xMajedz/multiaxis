#include "game.h"
#include "camera.h"
#include "luau.h"
#include "api.h"

#include <fstream>

using namespace raylib;
#include "raymath.h"
#include "rlgl.h"

static Shader shader;
static Model sphere;

void Game::Init()
{
	dInitODE();
	world = dWorldCreate();
	dWorldSetERP(world, 0.45);
	dWorldSetCFM(world, 10E-2);

	step = 1.0E-2;

	data = new Arena(4*1024*1024);

	API::Init();
	API::loadscript("init");

	Replay::Init();

	state.time = GetTime();
	state.running = true;

	SetExitKey(KEY_NULL);
}

void Game::Reset()
{
	if (space != nullptr) {
		dSpaceDestroy(space);
		space = nullptr;
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
	rules = API::GetRules();

	o_count = API::GetObjectsCount();
	jo_count = API::GetJointObjectsCount();

	p_count = API::GetPlayersCount();

	objects.reserve(o_count);
	objects = API::GetObjects();

	joint_objects.reserve(jo_count);
	joint_objects = API::GetJointObjects();

	players.reserve(p_count);
	players = API::GetPlayers();
}

void Game::NewGame()
{
	state.game_frame = 0;

	state.freeze = true;
	state.freeze_time = GetTime();
	state.freeze_frames = 50;
	state.freeze_frame = 0;
	state.freeze_count = 0;

	ghost_frames = 0;

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

		p.b_count = p.body.size();
		p.j_count = p.joint.size();

		//p.SetCatBits(2<<(pID + 1), 2<<pID + 1);
		//p.SetColBits(255-(2<<(pID + 1)), 255-(2<<pID + 1));

		p.Create(world, space);

		//p.SetEngagedistance(rules.engagedistance, pID * (360/rules.numplayers));
		//p.SetEngageheight(rules.engageheight);

		p.SetOffset();
	}

	state.running = true;
	
	Replay::WriteMetaData();

	API::NewGameCallback();
}

void Game::Stop()
{
	state.running = !state.running;
}

void Game::Quit()
{
	if (state.running) {
		Reset();
		dWorldDestroy(world);
		dCloseODE();
	}

	API::Close();

	Replay::Close();
}

void Game::SetBackgroundColor(uint16_t r, uint16_t g, uint16_t b, uint16_t a)
{
	background_color = {r, g, b, a};
}

size_t Game::GetContactCount()
{
	return numcollisions;
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

	//if (o1 == nullptr || o2 == nullptr) return;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if (dAreConnected(b1, b2)) return;

	if (b1 == b2) return;
	
	//uint32_t cat1 = dGeomGetCategoryBits(o1);
	//uint32_t col1 = dGeomGetCollideBits(o1);
	//uint32_t cat2 = dGeomGetCategoryBits(o2);
	//uint32_t col2 = dGeomGetCollideBits(o2);

	//if (!(cat1 & col2 || cat2 & col1)) return;

	dContact contacts[rules.max_contacts];

	for (int i = 0; i < rules.max_contacts; i += 1) {
		contacts[i].surface = (dSurfaceParameters) {
			.mode = dContactApprox1|dContactBounce,
			.mu = rules.friction,
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

	ghost_frames = 0;

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

static bool GhostCacheEnabled()
{
  return true;
}

static bool GhostCacheReady()
{
  using namespace Game;
  
	return ghost_frames >= ghost_length;
}

static void DrawBody(PlayerID pID, BodyID bID)
{
	using namespace Game;

	auto& b = players[pID].body[bID];

	Color color = b.m_color;

	Quaternion q = {
		b.frame_orientation.x,
		b.frame_orientation.y,
		b.frame_orientation.z,
		b.frame_orientation.w,
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	rlPushMatrix();
	rlTranslatef(b.frame_position.x, b.frame_position.y, b.frame_position.z);
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	/*
	 */

	switch(b.shape)
	{
	case BOX:
		DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, b.m_sides.x, b.m_sides.y, b.m_sides.z, color);
		break;
	case SPHERE:
		DrawSphere((Vector3){ 0.0f, 0.0f, 0.0f }, b.radius, color);
		break;
	case CAPSULE:
		DrawCapsule(
				(Vector3){ 0.0f, 0.0f, -(b.length/2) },
				(Vector3){ 0.0f, 0.0f,  (b.length/2) },
				b.radius,
				16,
				16,
				color
		);
		break;
	case CYLINDER:
		DrawCylinderEx(
				(Vector3){ 0.0f, 0.0f, -(b.length/2) },
				(Vector3){ 0.0f, 0.0f,  (b.length/2) },
				b.radius,
				b.radius,
				16,
				color
		);
		break;
	}

	/*
	 */

	rlPopMatrix();
}

static void ResetGhostCache()
{
  using namespace Game;
  
  ghost_frames = 0;
}

static void RecordGhostCache()
{
	using namespace Game;

	auto buffer = (dReal*)data->buffer();

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	for (int pID = 0; pID < p_count; pID += 1) {
		j_total += players[pID].j_count;
		b_total += players[pID].b_count;
	}

	uint32_t offset = 7 * ghost_frames * (j_total + b_total);

	uint32_t j_offset = 0;
	uint32_t b_offset = 0;

	for (int pID = 0; pID < p_count; pID += 1) {
		auto& p = players[pID];
		
		offset += 7 * pID * (p.j_count + p.b_count);

		for (int jID = 0; jID < p.j_count; jID += 1) {
			auto& j = p.joint[jID];

			auto frame_buffer = (dReal*)(buffer + offset + 7 * jID);
			
			frame_buffer[0] = j.frame_orientation.x;
			frame_buffer[1] = j.frame_orientation.y;
			frame_buffer[2] = j.frame_orientation.z;
			frame_buffer[3] = j.frame_orientation.w;

			frame_buffer[4] = j.frame_position.x;
			frame_buffer[5] = j.frame_position.y;
			frame_buffer[6] = j.frame_position.z;

			j_offset += 1;
		}

		//offset += 7 * (p.j_count + pID * p.b_count);
		offset += 7 * (j_offset + pID * p.b_count);

		for (int bID = 0; bID < p.b_count; bID += 1) {
			auto& b = p.body[bID];

			auto frame_buffer = (dReal*)(buffer + offset + 7 * bID);

			frame_buffer[0] = b.frame_orientation.x;
			frame_buffer[1] = b.frame_orientation.y;
			frame_buffer[2] = b.frame_orientation.z;
			frame_buffer[3] = b.frame_orientation.w;

			frame_buffer[4] = b.frame_position.x;
			frame_buffer[5] = b.frame_position.y;
			frame_buffer[6] = b.frame_position.z;
		}
	}

	ghost_frames += 1;
}

void Game::Update(dReal dt)
{
	API::UpdateCallback(dt);

	if (!state.pause) {
		numcollisions = 0;

		for (auto& o : objects) o.Step();
		for (auto& p : players) p.Step();

		if (!state.freeze) {
			switch (state.mode)
			{
			case SELF_PLAY: case FREE_PLAY:
				//Replay::RecordFrame(state.game_frame);
				
				if (state.game_frame >= state.freeze_frame) {
					Freeze();
				} else {
					state.game_frame += 1;
				}
	
				break;
			case REPLAY_PLAY:
				auto max_frame = Replay::GetMaxFrame();
				
				if (state.game_frame >= max_frame + 100) {
					EnterMode(REPLAY_PLAY);
				}

				if (Replay::CacheReady()) {
					/*
					 * 	TODO: Play Replay From Cache
					 */
				} else {	
					if (state.game_frame < max_frame) {
						Replay::PlayFrame(state.game_frame);
					}
				}

				/*
				 * 	TODO: Update Replay Cache
				 */
	
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

				if (ghost_frames < ghost_length) {
					RecordGhostCache();
				}
	
				break;
			}
	
			state.freeze_count += 1;
		}

		if (space != nullptr) {
		  if (GhostCacheEnabled() && !GhostCacheReady()) {
		     dSpaceCollide(space, 0, nearCallback);
		     dWorldStep(world, step);
		     dJointGroupEmpty(contactgroup);
		  }
		}
	}
}

static void DrawObject(BodyShape shape, vec3 sides, dReal radius, dReal length, Color color, Quaternion q, Vector3 p)
{
	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	rlPushMatrix();
	rlTranslatef(p.x, p.y, p.z);
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	switch(shape)
	{
	case BOX:
		DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, sides.x, sides.y, sides.z, color);

		break;
	case SPHERE:
	  //DrawModel(sphere, (Vector3){0.00, 0.00, 0.00}, radius, color);
	  DrawSphere((Vector3){ 0.0f, 0.0f, 0.0f }, radius, color);

		break;
	case CAPSULE:
		DrawCapsule(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				16,
				16,
				color
		);

		break;
	case CYLINDER:
		DrawCylinderEx(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				radius,
				16,
				color
		);

		break;
	}

	rlPopMatrix();
}

void Game::DrawGhostCache(int frame)
{
	auto buffer = (dReal*)data->buffer();

	uint32_t j_total = 0;
	uint32_t b_total = 0;

	for (int pID = 0; pID < p_count; pID += 1) {
		j_total += players[pID].j_count;
		b_total += players[pID].b_count;
	}

	uint32_t offset = 7 * frame * (j_total + b_total);

	uint32_t j_offset = 0;
	uint32_t b_offset = 0;

	for (int pID = 0; pID < p_count; pID += 1) {
		auto& p = players[pID];

		offset += 7 * pID * (p.j_count + p.b_count);

		for (int jID = 0; jID < players[pID].j_count; jID += 1) {
			auto& j = p.joint[jID];

			auto frame_buffer = (dReal*)(buffer + offset + 7 * jID);

			Quaternion q = {
				frame_buffer[0],
				frame_buffer[1],
				frame_buffer[2],
				frame_buffer[3],
			};

			Vector3 p = {
				frame_buffer[4],
				frame_buffer[5],
				frame_buffer[6],
			};

			DrawObject(
				j.shape,
				j.m_sides,
				j.radius,
				j.length,
				j.m_g_color,
				q,
				p
			);

			j_offset += 1;
		}

		//offset += 7 * (p.j_count + pID * p.b_count);
		offset += 7 * (j_offset + pID * p.b_count);

		for (int bID = 0; bID < players[pID].b_count; bID += 1) {
			auto& b = p.body[bID];

			auto frame_buffer = (dReal*)(buffer + offset + 7 * bID);

			Quaternion q = {
				frame_buffer[0],
				frame_buffer[1],
				frame_buffer[2],
				frame_buffer[3],
			};

			Vector3 p = {
				frame_buffer[4],
				frame_buffer[5],
				frame_buffer[6],
			};

			DrawObject(
				b.shape,
				b.m_sides,
				b.radius,
				b.length,
				b.m_g_color,
				q,
				p
			);
		}

	}
}

void Game::DrawContacts(bool freeze)
{
	dContact* contacts;

	if (freeze) {
		contacts = m_freeze_contacts;
	} else {
		contacts = m_frame_contacts;
	}

	for (int i = 0; numcollisions != 0; i += 1) {
		auto position = contacts[i].geom.pos;
		DrawSphere((Vector3){position[0], position[1], position[2]}, 0.10, Fade(YELLOW, 0.10));
		numcollisions -= 1;
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

void Game::Draw()
{
	for (auto& o : objects) {
	  o.Draw(state.freeze);
	}

	for (PlayerID pID = 0; pID < p_count; pID += 1) {
	  players[pID].Draw(state.freeze);
	}
	
	if (ghost_frames >= rules.turnframes) {
	  DrawGhostCache(rules.turnframes);
	}
	
	if (ghost_frames >= ghost_length) {
	  DrawGhostCache(state.freeze_count);
	}

	if (state.freeze && state.selected_player != -1 && state.selected_joint != -1) {
	  players[state.selected_player].joint[state.selected_joint].DrawSelect();
	}

	DrawFloor();
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

void Game::ToggleGhosts()
{
	for (PlayerID pID = 0; pID < p_count; pID += 1) {
		if (-1 != state.selected_player && pID != state.selected_player) players[pID].ToggleGhost();
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

void rl_log(int level, const char* msg, va_list)
{
	Console::log(TextFormat("%d: %s", level, msg));
}

void Window::Init()
{
	SetTraceLogLevel(LOG_ERROR);
	SetTraceLogCallback(rl_log);

	InitWindow(width, height, "TOBAS");

	Gamecam::Init();

	sphere = LoadModel("resources/model/sphere.obj");

	shader = LoadShader(NULL, "resources/shader/tobas.fs");

	background = LoadRenderTexture(width, height);
	foreground = LoadRenderTexture(width, height);

	initialized = true;

	SetTargetFPS(60);
}

static bool MouseInput = true;

static RayCollision CollideObject(Ray ray, BodyID oID)
{
	using namespace Game;

	auto& o = objects[oID];

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

static RayCollision CollideJoint(Ray ray, PlayerID pID, JointID jID)
{
	using namespace Game;

	auto& j = players[pID].joint[jID];

	RayCollision collision = { 0 };

	switch(j.shape)
	{
	case BOX:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					j.freeze_position.x - 0.5f * j.m_sides.x,
					j.freeze_position.y - 0.5f * j.m_sides.y,
					j.freeze_position.z - 0.5f * j.m_sides.z,
				},
				(Vector3){
					j.freeze_position.x + 0.5f * j.m_sides.x,
					j.freeze_position.y + 0.5f * j.m_sides.y,
					j.freeze_position.z + 0.5f * j.m_sides.z,
				},
			}
		);

		break;
	case SPHERE:
		collision = GetRayCollisionSphere(ray,
			(Vector3){
				j.freeze_position.x,
				j.freeze_position.y,
				j.freeze_position.z,
			},
			j.radius
		);

		break;
	case CAPSULE:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					j.freeze_position.x - 0.5f * j.m_sides.x,
					j.freeze_position.y - 0.5f * j.m_sides.y,
					j.freeze_position.z - 0.5f * j.m_sides.z,
				},
				(Vector3){
					j.freeze_position.x + 0.5f * j.m_sides.x,
					j.freeze_position.y + 0.5f * j.m_sides.y,
					j.freeze_position.z + 0.5f * j.m_sides.z,
				},
			}
		);

		break;
	case CYLINDER:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					j.freeze_position.x - 0.5f * j.m_sides.x,
					j.freeze_position.y - 0.5f * j.m_sides.y,
					j.freeze_position.z - 0.5f * j.m_sides.z,
				},
				(Vector3){
					j.freeze_position.x + 0.5f * j.m_sides.x,
					j.freeze_position.y + 0.5f * j.m_sides.y,
					j.freeze_position.z + 0.5f * j.m_sides.z,
				},
			}
		);

		break;
	}

	return collision;
}

static RayCollision CollideBody(Ray ray, PlayerID pID, BodyID bID)
{
	using namespace Game;

	auto& b = players[pID].body[bID];

	RayCollision collision = { 0 };

	switch(b.shape)
	{
	case BOX:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					b.freeze_position.x - 0.5f * b.m_sides.x,
					b.freeze_position.y - 0.5f * b.m_sides.y,
					b.freeze_position.z - 0.5f * b.m_sides.z,
				},
				(Vector3){
					b.freeze_position.x + 0.5f * b.m_sides.x,
					b.freeze_position.y + 0.5f * b.m_sides.y,
					b.freeze_position.z + 0.5f * b.m_sides.z,
				},
			}
		);

		break;
	case SPHERE:
		collision = GetRayCollisionSphere(ray,
			(Vector3){
				b.freeze_position.x,
				b.freeze_position.y,
				b.freeze_position.z,
			},
			b.radius
		);

		break;
	case CAPSULE:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					b.freeze_position.x - 0.5f * b.m_sides.x,
					b.freeze_position.y - 0.5f * b.m_sides.y,
					b.freeze_position.z - 0.5f * b.m_sides.z,
				},
				(Vector3){
					b.freeze_position.x + 0.5f * b.m_sides.x,
					b.freeze_position.y + 0.5f * b.m_sides.y,
					b.freeze_position.z + 0.5f * b.m_sides.z,
				},
			}
		);

		break;
	case CYLINDER:
		collision = GetRayCollisionBox(ray,
			(BoundingBox) {
				(Vector3){
					b.freeze_position.x - 0.5f * b.m_sides.x,
					b.freeze_position.y - 0.5f * b.m_sides.y,
					b.freeze_position.z - 0.5f * b.m_sides.z,
				},
				(Vector3){
					b.freeze_position.x + 0.5f * b.m_sides.x,
					b.freeze_position.y + 0.5f * b.m_sides.y,
					b.freeze_position.z + 0.5f * b.m_sides.z,
				},
			}
		);

		break;
	}

	return collision;
}

static int selected_object = -1;
static int selected_player = -1;
static int selected_joint = -1;
static int selected_body = -1;

static void gSelector(Camera3D camera)
{
	using namespace Game;

	Ray ray = GetMouseRay(GetMousePosition(), camera);

	RayCollision col1 = { 0 };
	RayCollision col2 = { 0 };
	
	for (BodyID oID = 0; oID < o_count; oID += 1) {
		col1 = CollideObject(ray, oID);

		if (col1.hit && (col2.distance == 0 || col2.distance > col1.distance)) {
			selected_object = oID;
		}
	}

	bool hit = false;

	for (PlayerID pID = 0; pID < p_count; pID += 1) {
		for (JointID jID = 0; jID < players[pID].j_count; jID += 1) {
			col1 = CollideJoint(ray, pID, jID);

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
			col1 = CollideBody(ray, pID, bID);

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

	SetWindowTitle(TextFormat("TOBAS %dFPS", GetFPS()));

	const auto& camera = Gamecam::Get();

	gSelector(camera);

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
		FilePathList dropped_files = LoadDroppedFiles();
		UnloadDroppedFiles(dropped_files);
	}
}

void Window::RenderBackground(Camera3D camera)
{
	using namespace Game;

	BeginTextureMode(background);
	ClearBackground(background_color);
	BeginMode3D(camera);
	Game::Draw();
	EndMode3D();
	EndTextureMode();
}

void Window::RenderForeground(Camera3D camera)
{
	BeginTextureMode(foreground);
	BeginMode3D(camera);
	ClearBackground(Fade(WHITE, 0.00));
	EndMode3D();
	EndTextureMode();
}

void Window::Draw()
{
	const auto& camera = Gamecam::Get();

	BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginShaderMode(shader);

	DrawTextureRec(background.texture, {0, 0, width, -height}, {0, 0}, WHITE);
	DrawTextureRec(foreground.texture, {0, 0, width, -height}, {0, 0}, WHITE);	
	
	RenderBackground(camera);
	RenderForeground(camera);

	EndShaderMode();
	API::DrawCallback();

	EndDrawing();
}

void Window::Close()
{
	UnloadModel(sphere);
	UnloadShader(shader);
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

void Game::EnterMode(Gamemode mode)
{	
	auto prev_mode = FREE_PLAY;

	if (mode == REPLAY_EDIT) {
		prev_mode = mode;
	} else {
		state.game_frame = 0;
		Restart();
	}

	for (auto& p : players) {
		p.RelaxAll();
		p.RelaxAllAlt();

		for (auto& b : p.body) {
			b.active = false;
		}
	}

	state.reaction_count = 0;
	state.freeze_count = 0;
	
	switch(mode)
	{
	case REPLAY_PLAY:
		state.mode = mode;
		state.freeze = false;
		Replay::Begin();

		break;
	case REPLAY_EDIT: case SELF_PLAY: case FREE_PLAY:
		if (mode == REPLAY_EDIT) {
			mode = prev_mode;
		}

		Replay::Reset();
		Replay::Begin();

		state.mode = mode;

		Freeze();

		break;
	}
}

bool Game::Running ()
{
	return state.running;
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
			Game::SetBodyState(p_id, b_id, (bool)state_buffer[b_id]);
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
