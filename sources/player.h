#pragma once
#include "common.h"
#include "body.h"

typedef int PlayerID;

enum PlayerPassiveStates
{
	HOLD_ALL,
	RELAX_ALL,
	MIXED,
};

struct Player
{
	dMass mass_;

	PlayerID id_;

	std::string name_;
	
	std::vector<Body> body;
	std::vector<Joint> joint;

	size_t b_count;
	size_t j_count;

	PlayerPassiveStates passive_states;
	PlayerPassiveStates passive_states_alt;
	
	raylib::Vector3 m_offset;

	raylib::Vector3 frame_offset;
	raylib::Vector3 freeze_offset;

	vec3<dReal> engagepos;
	vec3<dReal> engagerot;

	bool use_engagepos;
	bool use_engagerot;

	uint32_t b_cat_bits;
	uint32_t b_col_bits;
	uint32_t j_cat_bits;
	uint32_t j_col_bits;
	
	bool ready;
	bool ghost;

	raylib::Color m_color;
	raylib::Color m_g_color;
	raylib::Color m_b_color;
	raylib::Color m_j_color;

	raylib::Color m_b_active_color;
	raylib::Color m_j_select_color;

	void Ready();
	bool IsReady();

	Player(PlayerID id, std::string name);
	Player(std::string name);
	Player();

	PlayerID GetID();
	std::string GetName();

	JointState GetJointState(JointID joint_id);
	JointState GetJointStateAlt(JointID joint_id);

	void Create(dWorldID world, dSpaceID space);

	void SetColors(raylib::Color b_color, raylib::Color j_color, raylib::Color g_color);

	raylib::Vector3 CalculateOffset();
	raylib::Vector3 CalculateFreezeOffset();
	raylib::Vector3 CalculateFrameOffset();

	void UpdateOffset(bool freeze);

	void SetOffset(raylib::Vector3 offset);
	void SetOffset();

	void SetOrigin();

	raylib::Vector3 GetOffset();
	raylib::Vector3 GetOffset(bool freeze);

	void SetEngageheight(float offset);
	void SetEngagedistance(float offset, float angle);

	void SetCatBits(uint32_t b_bits, uint32_t j_bits);
	void SetColBits(uint32_t b_bits, uint32_t j_bits);

	void Step();
	void Freeze();

	void Refreeze();
	void Reset();

	void Draw(bool freeze);

	void ToggleGhost();

	void TriggerPlayerPassiveStates(PlayerPassiveStates state);
	void TriggerPlayerPassiveStatesAlt(PlayerPassiveStates state);
	void TogglePassiveStates();
	void TogglePassiveStatesAlt();
	void RelaxAll();
	void RelaxAllAlt();
};
