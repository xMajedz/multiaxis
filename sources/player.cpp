#include "player.h"

using namespace raylib;
#include "raymath.h"

Player::Player(PlayerID id, std::string name)
{
	id_ = id;
	name_ = name;
	
	m_color = WHITE;
	m_b_color = WHITE;
	m_j_color = BLACK;
	m_g_color = Fade(BLACK, 0.10);

	m_b_active_color = BLACK;
	m_j_select_color = Fade(WHITE, 0.90);

	m_offset = { 0 };

	passive_states = RELAX_ALL;
	passive_states_alt = RELAX_ALL;

	use_engagepos = false;
	use_engagerot = false;

	ready = false;
	ghost = true;
}

void Player::Create(dWorldID world, dSpaceID space)
{
	if (use_engagepos) {
		m_offset.x += engagepos.x;
		m_offset.y += engagepos.y;
		m_offset.z += engagepos.z;
	}

	m_b_color = WHITE;
	m_j_color = m_color;
	m_g_color = Fade(m_color, 0.800);

	for (auto& b : body) {
		b.m_position.x += m_offset.x;
		b.m_position.y += m_offset.y;
		b.m_position.z += m_offset.z;
		
		b.frame_position = b.m_position;
		b.freeze_position = b.m_position;

		b.frame_orientation = b.m_orientation;
		b.freeze_orientation = b.m_orientation;

		b.m_cat_bits = b_cat_bits;
		b.m_col_bits = b_col_bits;

		b.Create(world, space);

		b.m_color = m_b_color;
		b.m_g_color = m_g_color;

		b.ghost = true;
		b.static_ = false;

		b.m_active_color = m_j_color;

	}

	for (auto& j : joint) {
		j.m_position.x += m_offset.x;
		j.m_position.y += m_offset.y;
		j.m_position.z += m_offset.z;

		j.frame_position = j.m_position;
		j.freeze_position = j.m_position;

		j.frame_orientation = j.m_orientation;
		j.freeze_orientation = j.m_orientation;

		j.m_cat_bits = j_cat_bits;
		j.m_col_bits = j_col_bits;

		j.Create(world, space, body[j.connections[0]], body[j.connections[1]]);

		j.m_color = m_j_color;
		j.m_g_color = m_g_color;

		j.select = false;
		j.m_select_color = m_j_select_color;

		j.state = RELAX;
		j.state_alt = RELAX;

		j.ghost = true;
		j.static_ = false;
	}
}

PlayerID Player::GetID()
{
	return id_;
}

std::string Player::GetName()
{
	return name_;
}

void Player::Ready()
{
	ready = true;
}

bool Player::IsReady()
{
	return ready;
}

JointState Player::GetJointState(JointID joint_id)
{
	return joint[joint_id].state;
}

JointState Player::GetJointStateAlt(JointID joint_id)
{
	return joint[joint_id].state_alt;
}

Vector3 Player::CalculateFreezeOffset()
{
	Vector3 res = { 0 };
	for (auto& b : body) {
		res.x += b.freeze_position.x;
		res.y += b.freeze_position.y;
		res.z += b.freeze_position.z;
	}

	for (auto& j : joint) {
		res.x += j.freeze_position.x;
		res.y += j.freeze_position.y;
		res.z += j.freeze_position.z;
	}

	return (Vector3) {
		res.x/(b_count + j_count),
		res.y/(b_count + j_count),
		res.z/(b_count + j_count)
	};
}

Vector3 Player::CalculateFrameOffset()
{
	Vector3 res = { 0 };
	for (auto& b : body) {
		res.x += b.frame_position.x;
		res.y += b.frame_position.y;
		res.z += b.frame_position.z;
	}

	for (auto& j : joint) {
		res.x += j.frame_position.x;
		res.y += j.frame_position.y;
		res.z += j.frame_position.z;
	}

	return (Vector3) {
		res.x/(b_count + j_count),
		res.y/(b_count + j_count),
		res.z/(b_count + j_count)
	};
}

Vector3 Player::CalculateOffset()
{
	Vector3 res = { 0 };
	for (auto& b : body) {
		res.x += b.m_position.x;
		res.y += b.m_position.y;
		res.z += b.m_position.z;
	}

	for (auto& j : joint) {
		res.x += j.m_position.x;
		res.y += j.m_position.y;
		res.z += j.m_position.z;
	}

	return (Vector3) {
		res.x/(b_count + j_count),
		res.y/(b_count + j_count),
		res.z/(b_count + j_count)
	};
}

void Player::UpdateOffset(bool freeze)
{
}

void Player::SetColors(Color b_color, Color j_color, Color g_color)
{
	m_b_color = b_color;
	m_j_color = j_color;
	m_g_color = g_color;
}

void Player::SetOrigin()
{
	m_offset = {-m_offset.x, -m_offset.y, -m_offset.z};
}

void Player::SetOffset(Vector3 offset)
{
	m_offset = offset;
	frame_offset = offset;
	freeze_offset = offset;
}

void Player::SetOffset()
{
	SetOffset(CalculateOffset());
}

Vector3 Player::GetOffset()
{
	return m_offset;
}

Vector3 Player::GetOffset(bool freeze)
{
	if (freeze) {
		return freeze_offset;
	} else {
		return frame_offset;
	}
}

void Player::SetEngageheight(float offset)
{
	for (auto& b : body) {
		b.m_offset.z += offset;
		//b.SetOffset();
	}

	for (auto& j : joint) {
		j.m_offset.z += offset;
		//j.SetOffset();
	}
}

void Player::SetEngagedistance(float offset, float angle)
{
	Quaternion q = QuaternionFromMatrix(MatrixRotateZ(DEG2RAD * angle));
	for (auto& b : body) {
		/*Vector3 offsetv = {
			b.m_position.x,
			b.m_position.y + offset,
			b.m_position.z,
		};

		offsetv = Vector3RotateByQuaternion(offsetv, q);

		b.m_position = offsetv;

		b.m_position.x += m_offset.x;
		b.m_position.y += m_offset.y;
		b.m_position.z += m_offset.z;

		b.frame_position = b.m_position;
		b.freeze_position = b.m_position;

		b.m_orientation = QuaternionMultiply(q, b.m_orientation);
		b.frame_orientation = b.m_orientation;
		b.freeze_orientation = b.m_orientation;*/
	}

	for (auto& j : joint) {
		/*Vector3 offsetv = {
			j.m_position.x,
			j.m_position.y + offset,
			j.m_position.z,
		};

		offsetv = Vector3RotateByQuaternion(offsetv, q);

		j.m_position = offsetv;

		j.m_position.x += m_offset.x;
		j.m_position.y += m_offset.y;
		j.m_position.z += m_offset.z;

		j.frame_position = j.m_position;
		j.freeze_position = j.m_position;

		j.m_orientation = QuaternionMultiply(q, j.m_orientation);
		j.frame_orientation = j.m_orientation;
		j.freeze_orientation = j.m_orientation;*/
	}
}

void Player::SetColBits(uint32_t b_bits, uint32_t j_bits)
{
	b_col_bits = b_bits;
	j_col_bits = j_bits;
}

void Player::SetCatBits(uint32_t b_bits, uint32_t j_bits)
{
	b_cat_bits = b_bits;
	j_cat_bits = j_bits;
}

void Player::Step()
{
	for (auto& b : body) {
		b.Step();
	}

	for (auto& j : joint) {
		j.Step();
	}

	frame_offset = CalculateFrameOffset();
}

void Player::Freeze()
{
	for (auto& b : body) {
		b.Freeze();
	}

	for (auto& j : joint) {
		j.Freeze();
	}

	freeze_offset = CalculateFreezeOffset();

	ready = false;
}

void Player::Refreeze()
{
	for (auto& b : body) {
		b.Refreeze();
	}
}

void Player::Reset()
{
	for (auto& b : body) {
		b.Reset();
	}
}

void Player::Draw(bool freeze)
{
	for (auto& b : body) {
		b.Draw(freeze);
	}

	for (auto& j : joint) {
		j.Draw(freeze);
	}
}

void Player::ToggleGhost()
{
	for (auto& b : body) {
		b.ToggleGhost();
	}

	for (auto& j : joint) {
		j.ToggleGhost();
	}
}


void Player::TriggerPlayerPassiveStates(PlayerPassiveStates state)
{
	dReal strength = 0.00;
	for (auto& j : joint) {
		j.state = RELAX;
		if (state == HOLD_ALL) {
			strength = j.strength;
			j.state = HOLD;
		}
		switch(j.type)
		{
		case HINGE: {
			dJointSetHingeParam(j.dJoint, dParamFMax, strength);
			dJointSetHingeParam(j.dJoint, dParamVel, 0.00);
		} break;
		case dSLIDER: {
			dJointSetSliderParam(j.dJoint, dParamFMax, strength);
			dJointSetSliderParam(j.dJoint, dParamVel, 0.00);
		} break;
		case UNIVERSAL: {
			dJointSetUniversalParam(j.dJoint, dParamFMax, strength);
			dJointSetUniversalParam(j.dJoint, dParamVel, 0.00);
		} break;
		case HINGE2: {
			dJointSetHinge2Param(j.dJoint, dParamFMax, strength);
			dJointSetHinge2Param(j.dJoint, dParamVel, 0.00);
		} break;
		}
	}
}

void Player::TriggerPlayerPassiveStatesAlt(PlayerPassiveStates state)
{
	dReal strength = 0.00;
	for (auto& j : joint) {
		j.state_alt = RELAX;
		if (state == HOLD_ALL) {
			strength = j.strength_alt;
			j.state_alt = HOLD;
		}
		switch(j.type)
		{
		case UNIVERSAL: {
			dJointSetUniversalParam(j.dJoint, dParamFMax2, strength);
			dJointSetUniversalParam(j.dJoint, dParamVel2, 0.00);
		} break;
		case HINGE2: {
			dJointSetHinge2Param(j.dJoint, dParamFMax2, strength);
			dJointSetHinge2Param(j.dJoint, dParamVel2, 0.00);
		} break;
		}
	}
}

void Player::TogglePassiveStates()
{
	Refreeze();

	if (passive_states == RELAX_ALL) {
		TriggerPlayerPassiveStates(HOLD_ALL);
		passive_states = HOLD_ALL;
	} else {
		TriggerPlayerPassiveStates(RELAX_ALL);
		passive_states = RELAX_ALL;
	}
}

void Player::TogglePassiveStatesAlt()
{
	Refreeze();

	if (passive_states_alt == RELAX_ALL) {
		TriggerPlayerPassiveStatesAlt(HOLD_ALL);
		passive_states_alt = HOLD_ALL;
	} else {
		TriggerPlayerPassiveStatesAlt(RELAX_ALL);
		passive_states_alt = RELAX_ALL;
	}
}

void Player::RelaxAll()
{
	Refreeze();

	TriggerPlayerPassiveStates(RELAX_ALL);
	passive_states = RELAX_ALL;
}

void Player::RelaxAllAlt()
{
	Refreeze();

	TriggerPlayerPassiveStatesAlt(RELAX_ALL);
	passive_states_alt = RELAX_ALL;
}
