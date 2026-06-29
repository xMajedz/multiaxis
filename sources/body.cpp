#include "body.h"

using namespace raylib;
#include "raymath.h"
#include "rlgl.h"

Body::Body()
{
  //id_ = id;
  //name_ = name;

	dBody = nullptr;
	dGeom = nullptr;

	m_cat_bits = 0b0001;
	m_col_bits = 0b0000;

	m_color = BLACK;
	m_g_color = Fade(BLACK, 0.10);

	m_orientation = {0.00, 0.00, 0.00, 1.00};
	m_position = { 0 };
	m_offset =  { 0 };

	frame_orientation = {0.00, 0.00, 0.00, 1.00};
	frame_position = { 0 };
	frame_offset =  { 0 };

	frame_linear_vel = { 0 };
	frame_angular_vel = { 0 };

	freeze_orientation = {0.00, 0.00, 0.00, 1.00};
	freeze_position = { 0 };
	freeze_offset =  { 0 };

	freeze_linear_vel = { 0 };
	freeze_angular_vel = { 0 };

	active = false;
	m_active_color = BLACK;

	select = false;
	m_select_color = Fade(WHITE, 0.10);

	flag_ = 0;
	static_ = false;
	composite_ = false;
	interactive_ = false;

	//data_.id = id;
}

void Body::Create(dWorldID world, dSpaceID space)
{
	world_ = world;
	space_ = space;

	frame_position = m_position;
	freeze_position = m_position;

	frame_orientation = m_orientation;
	freeze_orientation = m_orientation;

	if (static_) {
		CreateStatic();
		data_.group = 1;
	} else {
		CreateDynamic();
        data_.group = 2;
		
		m_col_bits = 0b0001;
	}

	SetCatBits();
	SetColBits();
	
	if (!interactive_) {
		dGeomSetData(dGeom, nullptr);
	} else {
		data_.active = false;
		dGeomSetData(dGeom, &data_);
	}
}

void Body::CreateBody()
{
	dBody = dBodyCreate(world_);

	dBodySetPosition(dBody, m_position.x, m_position.y, m_position.z);

	dQuaternion q = { m_orientation.w, m_orientation.x, m_orientation.y, m_orientation.z };

	dBodySetQuaternion(dBody, q);

	if (mass != 0)
	    dMassAdjust(&mass_, mass);
	
	dBodySetMass(dBody, &mass_);
}

void Body::CreateGeom()
{
	switch(shape) {
	case BOX: {
		dGeom = dCreateBox(space_, sides.x, sides.y, sides.z);
		dMassSetBox(&mass_, density, sides.x, sides.y, sides.z);
	} break;
	case SPHERE: {
		dGeom = dCreateSphere(space_, radius);
		dMassSetSphere(&mass_, density, radius);
	} break;
	case CAPSULE: {
		dGeom = dCreateCapsule(space_, radius, length);
		dMassSetCapsule(&mass_, density, 1, length, radius);
	} break;
	case CYLINDER: {
		dGeom = dCreateCylinder(space_, radius, length);
		dMassSetCylinder(&mass_, density, 1, length, radius);
	} break;
	}
	
	dGeomSetPosition(
		dGeom,
		m_position.x,
		m_position.y,
		m_position.z
	);

	dQuaternion q = { m_orientation.w, m_orientation.x, m_orientation.y, m_orientation.z };
	
	dGeomSetQuaternion(dGeom, q);
}

void Body::CreateDynamic()
{
	CreateGeom();
	CreateBody();
	dGeomSetBody(dGeom, dBody);
}

void Body::CreateStatic()
{
	CreateGeom();
	dGeomSetBody(dGeom, 0);
}

void Body::CreateComposite(dBodyID b)
{
	CreateGeom();
	dGeomSetBody(dGeom, b);
}

void Body::SetOrientation(Vector4 q)
{
	dQuaternion quaternion = { q.w, q.x, q.y, q.z }; 
	dGeomSetQuaternion(dGeom, quaternion);
}

void Body::SetPosition(Vector3 p)
{
	dGeomSetPosition(dGeom, p.x, p.y, p.z);
}

void Body::SetLinearVel(Vector3 v)
{
	dBodySetLinearVel(dBody, v.x, v.y, v.z);
}

void Body::SetAngularVel(Vector3 v)
{
	dBodySetAngularVel(dBody, v.x, v.y, v.z);
}

void Body::SetColor(Color color)
{
	m_color = color;
}

BodyID Body::GetID()
{
	return id_;
}

void Joint::Step()
{
	if (dJoint != nullptr) {
		switch(type)
		{
		case HINGE: {
			frame_vel = dJointGetHingeParam(dJoint, dParamVel);
		}	break;
		case dSLIDER: {
			frame_vel = dJointGetSliderParam(dJoint, dParamVel);
		}	break;
		case UNIVERSAL: {
			frame_vel = dJointGetUniversalParam(dJoint, dParamVel);
			frame_vel_alt = dJointGetUniversalParam(dJoint, dParamVel2);
		}	break;
		case HINGE2: {
			frame_vel = dJointGetHinge2Param(dJoint, dParamVel);
			frame_vel_alt = dJointGetHinge2Param(dJoint, dParamVel2);
		}	break;
		}
	}

	dQuaternion orientation = { 0 };
	dGeomGetQuaternion(dGeom, orientation);
	frame_orientation = { orientation[1], orientation[2], orientation[3], orientation[0] };

	const dReal* position = dGeomGetPosition(dGeom);
	frame_position = { position[0], position[1], position[2] };

	const dReal* offset = dGeomGetOffsetPosition(dGeom); 
	frame_offset = { offset[0], offset[1], offset[2] };
}

void Body::Step()
{
	if (interactive_) {
		data_.active = active;
	}

	if (!static_ && dBody != nullptr) {
		const dReal* linear_vel = dBodyGetLinearVel(dBody);
		const dReal* angular_vel = dBodyGetAngularVel(dBody);

		frame_linear_vel = { linear_vel[0], linear_vel[1], linear_vel[2] };
		frame_angular_vel = { angular_vel[0], angular_vel[1], angular_vel[2] };
	}

	dQuaternion orientation = { 0 };
	dGeomGetQuaternion(dGeom, orientation);
	frame_orientation = { orientation[1], orientation[2], orientation[3], orientation[0] };

	const dReal* position = dGeomGetPosition(dGeom);
	frame_position = { position[0], position[1], position[2] };

	const dReal* offset = dGeomGetOffsetPosition(dGeom); 
	frame_offset = { offset[0], offset[1], offset[2] };
}

void Body::SetCatBits(uint32_t bits)
{
	dGeomSetCategoryBits(dGeom, bits);
}

void Body::SetCatBits()
{
	SetCatBits(m_cat_bits);
}

void Body::SetColBits(uint32_t bits)
{
	dGeomSetCollideBits(dGeom, bits);
}

void Body::SetColBits()
{
	SetColBits(m_col_bits);
}

void Body::Freeze()
{
	if (!static_) {
		const dReal* linear_vel = dBodyGetLinearVel(dBody);
		const dReal* angular_vel = dBodyGetAngularVel(dBody);

		freeze_linear_vel = {linear_vel[0], linear_vel[1], linear_vel[2]};
		freeze_angular_vel = {angular_vel[0], angular_vel[1], angular_vel[2]};
	}

	dQuaternion orientation = { 0 };
	dGeomGetQuaternion(dGeom, orientation);
	freeze_orientation = {orientation[1], orientation[2], orientation[3], orientation[0]};

	const dReal* position = dGeomGetPosition(dGeom);
	freeze_position = {position[0], position[1], position[2]};
}

void Body::Refreeze()
{
	if (!static_) {
		dBodySetLinearVel(
			dBody, 
			freeze_linear_vel.x,
			freeze_linear_vel.y,
			freeze_linear_vel.z
		);
		dBodySetAngularVel(
			dBody,
			freeze_angular_vel.x,
			freeze_angular_vel.y,
			freeze_angular_vel.z
		);
	}

	dGeomSetPosition(
		dGeom,
		freeze_position.x,
		freeze_position.y,
		freeze_position.z
	);

	dQuaternion q = { freeze_orientation.w, freeze_orientation.x, freeze_orientation.y, freeze_orientation.z };

	dGeomSetQuaternion(dGeom, q);
};

void Body::Reset()
{
	if (dBody != nullptr) {
		dBodySetLinearVel(dBody, 0.00, 0.00, 0.00);
		dBodySetAngularVel(dBody, 0.00, 0.00, 0.00);

	}

	freeze_linear_vel = { 0.00, 0.00, 0.00 };
	freeze_angular_vel = { 0.00, 0.00, 0.00 };

	dGeomSetPosition(
		dGeom,
		m_position.x,
		m_position.y,
		m_position.z
	);

	dQuaternion q = { m_orientation.w, m_orientation.x, m_orientation.y, m_orientation.z };

	dGeomSetQuaternion(dGeom, q);

	frame_position = m_position;
	freeze_position = m_position;

	frame_orientation = m_orientation;
	freeze_orientation = m_orientation;
}

void Body::DrawObject(Color color)
{
	switch(shape)
	{
	case BOX:
		DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, sides.x, sides.y, sides.z, color);
		break;
	case SPHERE:
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
}

void Body::DrawObjectWires(Color color)
{
	switch(shape)
	{
	case BOX:
		DrawCubeWires((Vector3){ 0.0f, 0.0f, 0.0f }, sides.x, sides.y, sides.z, color);
		break;
	case SPHERE:
		DrawSphereWires((Vector3){ 0.0f, 0.0f, 0.0f }, radius, 16, 16, color);
		break;
	case CAPSULE:
		DrawCapsuleWires(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				16,
				16,
				color
		);
		break;
	case CYLINDER:
		DrawCylinderWiresEx(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				radius,
				16,
				color
		);
		break;
	}
}

void Body::Draw(Color color)
{
	Quaternion q = {
		frame_orientation.x,
		frame_orientation.y,
		frame_orientation.z,
		frame_orientation.w,
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	rlPushMatrix();
	rlTranslatef(frame_position.x, frame_position.y, frame_position.z);
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	DrawObject(color);

	rlPopMatrix();
}

void Body::DrawFreeze(Color color)
{
	Quaternion q = {
		freeze_orientation.x,
		freeze_orientation.y,
		freeze_orientation.z,
		freeze_orientation.w
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);
	rlPushMatrix();
	rlTranslatef(
		freeze_position.x,
		freeze_position.y,
		freeze_position.z
	);

	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	DrawObject(color);

	rlPopMatrix();
}

void Body::DrawSelect()
{
	DrawFreeze(m_select_color);
}

void Body::Draw(bool freeze)
{
	if (freeze) {
		if (active) {
			DrawFreeze(m_active_color);
		} else {
			DrawFreeze(m_color);
		}

		if (!static_) {
			if (ghost) {
				Draw(m_g_color);
			}
		}
	} else {
		Draw(m_color);
	}
}

void Body::ToggleGhost()
{
	ghost = ghost == false;
}

void Body::ToggleState()
{
	active = active == false;
}

std::string Body::GetName()
{
	return name_;
}

Joint::Joint()
{
	state = RELAX;
	state_alt = RELAX;

	m_orientation = {0.00, 0.00, 0.00, 1.00};
	m_position = { 0 };
	m_offset =  { 0 };

	frame_orientation = {0.00, 0.00, 0.00, 1.00};
	frame_position = { 0 };
	frame_offset =  { 0 };

	frame_linear_vel = { 0 };
	frame_angular_vel = { 0 };

	freeze_orientation = {0.00, 0.00, 0.00, 1.00};
	freeze_position = { 0 };
	freeze_offset =  { 0 };

	freeze_linear_vel = { 0 };
	freeze_angular_vel = { 0 };

	select = false;
	m_select_color = Fade(WHITE, 0.10);
}

void Joint::Create(dWorldID world, dSpaceID space, Body b1, Body b2)
{
	world_ = world;
	space_ = space;

	frame_position = m_position;
	freeze_position = m_position;

	frame_orientation = m_orientation;
	freeze_orientation = m_orientation;

	dVector3 v_axis = { axis.x, axis.y, axis.z };
	dVector3 v_axis_alt = { axis_alt.x, axis_alt.y, axis_alt.z };

	if (composite_) {
		CreateComposite(b1.dBody);
		dBody = b1.dBody;
		dGeomSetOffsetWorldPosition(dGeom, m_position.x, m_position.y, m_position.z);
	}
	
	switch(type)
	{
	case HINGE:
		dJoint = dJointCreateHinge(world_, 0);
		dJointAttach(dJoint, b1.dBody, b2.dBody);
		dJointSetHingeAnchor(
			dJoint,
			m_position.x,
			m_position.y,
			m_position.z
		);

		dJointSetHingeAxis(
			dJoint,
			axis.x,
			axis.y,
			axis.z
		);

		dJointSetHingeParam(
			dJoint,
			dParamHiStop,
			range[0]
		);

		dJointSetHingeParam(
			dJoint,
			dParamLoStop,
			range[1]
		);

		dJointSetHingeParam(
			dJoint,
			dParamFudgeFactor,
			0.50
		);

		break;
	case dSLIDER:
		dJoint = dJointCreateSlider(world_, 0);
		dJointAttach(dJoint, b1.dBody, b2.dBody);
		dJointSetSliderAxis(
			dJoint,
			axis.x,
			axis.y,
			axis.z
		);

		dJointSetSliderParam(
			dJoint,
			dParamHiStop,
			range[0]
		);

		dJointSetSliderParam(
			dJoint,
			dParamLoStop,
			range[1]
		);

		break;
	case UNIVERSAL:
		dJoint = dJointCreateUniversal(world_, 0);
		dJointAttach(dJoint, b1.dBody, b2.dBody);
		dJointSetUniversalAnchor(
			dJoint,
			m_position.x,
			m_position.y,
			m_position.z
		);

		dJointSetUniversalAxis1(
			dJoint,
			axis.x,
			axis.y,
			axis.z
		);

		dJointSetUniversalAxis2(
			dJoint,
			axis_alt.x,
			axis_alt.y,
			axis_alt.z
		);

		dJointSetUniversalParam(
			dJoint,
			dParamHiStop,
			range[0]
		);

		dJointSetUniversalParam(
			dJoint,
			dParamHiStop2,
			range_alt[0]
		);

		dJointSetUniversalParam(
			dJoint,
			dParamLoStop,
			range[1]
		);

		dJointSetUniversalParam(
			dJoint,
			dParamLoStop2,
			range_alt[1]
		);

		dJointSetUniversalParam(
			dJoint,
			dParamFudgeFactor,
			0.50
		);

		break;
	case HINGE2:
		dJoint = dJointCreateHinge2(world_, 0);
		dJointAttach(dJoint, b1.dBody, b2.dBody);
		dJointSetHinge2Anchor(
			dJoint,
			m_position.x,
			m_position.y,
			m_position.z
		);

		dJointSetHinge2Axes(dJoint, v_axis, v_axis_alt);

		dJointSetHinge2Param(
			dJoint,
			dParamHiStop,
			range[0]
		);

		dJointSetHinge2Param(
			dJoint,
			dParamHiStop2,
			range_alt[0]
		);

		dJointSetHinge2Param(
			dJoint,
			dParamLoStop,
			range[1]
		);

		dJointSetHinge2Param(
			dJoint,
			dParamLoStop2,
			range_alt[1]
		);

		dJointSetHinge2Param(
			dJoint,
			dParamFudgeFactor,
			0.50
		);

		break;
	default:
		dJoint = dJointCreateFixed(world_, 0);
		dJointAttach(dJoint, b1.dBody, b2.dBody);
		dJointSetFixed(dJoint);
	}

	if (composite_) {
		SetCatBits();
		SetColBits();
	}
}

void Joint::Draw(Color color)
{
	Quaternion q = {
		frame_orientation.x,
		frame_orientation.y,
		frame_orientation.z,
		frame_orientation.w,
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);

	rlPushMatrix();
	rlTranslatef(frame_position.x, frame_position.y, frame_position.z);
	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	DrawObject(color);

	rlPopMatrix();
}

void Joint::Draw(bool freeze)
{
	if (freeze) {
		DrawFreeze(m_color);

		if (ghost) {
			Draw(m_g_color);
		}
	} else {
		Draw(m_color);
	}
}

void Joint::DrawFreeze(Color color)
{
	Quaternion q = {
		freeze_orientation.x,
		freeze_orientation.y,
		freeze_orientation.z,
		freeze_orientation.w
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);
	rlPushMatrix();
	rlTranslatef(
		freeze_position.x,
		freeze_position.y,
		freeze_position.z
	);

	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	DrawObject(color);

	rlPopMatrix();
}

void Joint::DrawSelect()
{
	Quaternion q = {
		freeze_orientation.x,
		freeze_orientation.y,
		freeze_orientation.z,
		freeze_orientation.w
	};

	float angle;
	Vector3 axis;

	QuaternionToAxisAngle(q, &axis, &angle);
	rlPushMatrix();
	rlTranslatef(
		freeze_position.x,
		freeze_position.y,
		freeze_position.z
	);

	rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);

	switch(shape)
	{
	case BOX:
		DrawCube((Vector3){ 0.0f, 0.0f, 0.0f }, sides.x, sides.y, sides.z, m_select_color);
		break;
	case SPHERE:
		DrawSphere((Vector3){ 0.0f, 0.0f, 0.0f }, radius * 1.2, m_select_color);
		break;
	case CAPSULE:
		DrawCapsule(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				16,
				16,
				m_select_color
		);
		break;
	case CYLINDER:
		DrawCylinderEx(
				(Vector3){ 0.0f, 0.0f, -(length/2) },
				(Vector3){ 0.0f, 0.0f,  (length/2) },
				radius,
				radius,
				16,
				m_select_color
		);
		break;
	}

	rlPopMatrix();

	DrawAxis(true);
	DrawRange(true);
}

void Joint::DrawAxis(bool freeze)
{
	Vector3 v_axis = Vector3Normalize((Vector3){axis.x, axis.y, axis.z});
	if (freeze) {
		Vector3 start = {
			freeze_position.x + v_axis.x * 0.2,
			freeze_position.y + v_axis.y * 0.2,
			freeze_position.z + v_axis.z * 0.2,
		};
	
		Vector3 end = {
			freeze_position.x - v_axis.x * 0.2,
			freeze_position.y - v_axis.y * 0.2,
			freeze_position.z - v_axis.z * 0.2,
		};

		DrawLine3D(start, end, BLACK);
	} else {
		Vector3 start = {
			frame_position.x + v_axis.x * 0.2,
			frame_position.y + v_axis.y * 0.2,
			frame_position.z + v_axis.z * 0.2,
		};
	
		Vector3 end = {
			frame_position.x - v_axis.x * 0.2,
			frame_position.y - v_axis.y * 0.2,
			frame_position.z - v_axis.z * 0.2,
		};

		DrawLine3D(start, end, BLACK);
	}
}

void Joint::DrawRange(bool freeze)
{
	Vector3 v_axis = Vector3Normalize((Vector3){axis.x, axis.y, axis.z});
	Vector3 v = Vector3Perpendicular(v_axis);
	if (freeze) {
		Vector3 start = {
			freeze_position.x + v.x * 0.2,
			freeze_position.y + v.y * 0.2,
			freeze_position.z + v.z * 0.2,
		};
	
		Vector3 end = {
			freeze_position.x - v.x * 0.2,
			freeze_position.y - v.y * 0.2,
			freeze_position.z - v.z * 0.2,
		};

		DrawLine3D(start, end, RED);
	} else {
		Vector3 start = {
			frame_position.x + v.x * 0.2,
			frame_position.y + v.y * 0.2,
			frame_position.z + v.z * 0.2,
		};
	
		Vector3 end = {
			frame_position.x - v.x * 0.2,
			frame_position.y - v.y * 0.2,
			frame_position.z - v.z * 0.2,
		};

		DrawLine3D(start, end, RED);
	}
}

void Joint::TriggerActiveStateAlt(dReal vel)
{
	switch(type)
	{
	case UNIVERSAL:
		dJointSetUniversalParam(dJoint, dParamFMax2, strength_alt);
		dJointSetUniversalParam(dJoint, dParamVel2, vel);
		break;
	case HINGE2:
		dJointSetHinge2Param(dJoint, dParamFMax2, strength_alt);
		dJointSetHinge2Param(dJoint, dParamVel2, vel);
		break;
	}
}

void Joint::TriggerPassiveStateAlt(dReal strength)
{
	switch(type)
	{
	case UNIVERSAL:
		dJointSetUniversalParam(dJoint, dParamFMax2, strength);
		dJointSetUniversalParam(dJoint, dParamVel2, 0.00);
		break;
	case HINGE2:
		dJointSetHinge2Param(dJoint, dParamFMax2, strength);
		dJointSetHinge2Param(dJoint, dParamVel2, 0.00);
		break;
	}
}

void Joint::TriggerActiveState(dReal vel)
{
	switch(type)
	{
	case HINGE:
		dJointSetHingeParam(dJoint, dParamFMax, strength);
		dJointSetHingeParam(dJoint, dParamVel, vel);
		break;
	case dSLIDER:
		dJointSetSliderParam(dJoint, dParamFMax, strength);
		dJointSetSliderParam(dJoint, dParamVel, vel);
		break;
	case UNIVERSAL:
		dJointSetUniversalParam(dJoint, dParamFMax, strength);
		dJointSetUniversalParam(dJoint, dParamVel, vel);
		break;
	case HINGE2:
		dJointSetHinge2Param(dJoint, dParamFMax, strength);
		dJointSetHinge2Param(dJoint, dParamVel, vel);
		break;
	}
}

void Joint::TriggerPassiveState(dReal strength)
{
	switch(type)
	{
	case HINGE:
		dJointSetHingeParam(dJoint, dParamFMax, strength);
		dJointSetHingeParam(dJoint, dParamVel, 0.00);
		break;
	case dSLIDER:
		dJointSetSliderParam(dJoint, dParamFMax, strength);
		dJointSetSliderParam(dJoint, dParamVel, 0.00);
		break;
	case UNIVERSAL:
		dJointSetUniversalParam(dJoint, dParamFMax, strength);
		dJointSetUniversalParam(dJoint, dParamVel, 0.00);
		break;
	case HINGE2:
		dJointSetHinge2Param(dJoint, dParamFMax, strength);
		dJointSetHinge2Param(dJoint, dParamVel, 0.00);
		break;
	}
}

void Joint::TogglePassiveState()
{
	switch(state)
	{
	case HOLD:
		state = RELAX;
		TriggerPassiveState(0.00);
		break;
	default:
		state = HOLD;
		TriggerPassiveState(strength);
	}
}

void Joint::TogglePassiveStateAlt()
{

	switch(state_alt)
	{
	case HOLD:
		state_alt = RELAX;
		TriggerPassiveStateAlt(0.00);
		break;
	default:
		state_alt = HOLD;
		TriggerPassiveStateAlt(strength);
	}
}

void Joint::ToggleActiveState(dReal vel)
{
	switch(state)
	{
	case FORWARD:
		state = BACKWARD;
		TriggerActiveState(-1.00 * vel);
		break;
	default:
		state = FORWARD;
		TriggerActiveState(1.00 * vel);
	}
}

void Joint::ToggleActiveStateAlt(dReal vel)
{
	switch(state_alt)
	{
	case FORWARD:
		state_alt = BACKWARD;
		TriggerActiveStateAlt(-1.00 * vel);
		break;
	default:
		state_alt = FORWARD;
		TriggerActiveStateAlt(1.00 * vel);
	}
}

void Joint::ToggleActiveState()
{
	ToggleActiveState(velocity);
}

void Joint::ToggleActiveStateAlt()
{
	ToggleActiveStateAlt(velocity_alt);
}

void Joint::CycleState()
{
	switch(state)
	{
	case FORWARD:
		state = BACKWARD;
		TriggerActiveState(-1.00 * velocity);
		break;
	case BACKWARD:
		state = HOLD;
		TriggerPassiveState(strength);
		break;
	case HOLD:
		state = RELAX;
		TriggerPassiveState(0.00);
		break;
	default:
		state = FORWARD;
		TriggerActiveState(1.00 * velocity);
	}
}

void Joint::CycleStateAlt()
{
	switch(state_alt)
	{
	case FORWARD:
		state_alt = BACKWARD;
		TriggerActiveStateAlt(-1.00 * velocity_alt);
		break;
	case BACKWARD:
		state_alt = HOLD;
		TriggerPassiveStateAlt(strength_alt);
		break;
	case HOLD:
		state_alt = RELAX;
		TriggerPassiveStateAlt(0.00);
		break;
	default:
		state_alt = FORWARD;
		TriggerActiveStateAlt(1.00 * velocity_alt);
	}
}

void Joint::ReverseCycleState()
{
	switch(state)
	{
	case FORWARD:
		state = RELAX;
		TriggerPassiveState(0.00);
		break;
	case BACKWARD:
		state = FORWARD;
		TriggerActiveState(1.00 * velocity);
		break;
	case HOLD:
		state = BACKWARD;
		TriggerActiveState(-1.00 * velocity);
		break;
	default:
		state = HOLD;
		TriggerPassiveState(strength);
	}
}

void Joint::ReverseCycleStateAlt()
{
	switch(state_alt)
	{
	case FORWARD:
		state_alt = RELAX;
		TriggerPassiveStateAlt(0.00);
		break;
	case BACKWARD:
	  	state_alt = FORWARD;
		TriggerActiveStateAlt(1.00 * velocity_alt);
		break;
	case HOLD:
	  	state_alt = BACKWARD;
		TriggerActiveStateAlt(-1.00 * velocity_alt);
		break;
	default:
	    state_alt = HOLD;
		TriggerPassiveStateAlt(strength_alt);
	}
}
