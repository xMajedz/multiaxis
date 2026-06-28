#pragma once
#include "common.h"

typedef int BodyID;

enum BodyShape
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER,
	COMPOSITE,
};

struct dUserData
{
    int group;
    int id;
  
	bool active;
	dContact contact;
	dJointID contact_joint = nullptr;
};

struct Body
{
	dUserData data_;

	BodyID id_;

	std::string name_;

	dMass mass_;

	dWorldID world_;
	dSpaceID space_;

	dBodyID dBody;
	dGeomID dGeom;

	BodyShape shape;

	dReal radius;
	dReal length;
	dReal density;
    dReal mass;
  
    dReal friction;
    dReal bounce;
  
    vec4<dReal> m_orientation;
	vec3<dReal> m_position;
	vec3<dReal> m_offset;
	vec3<dReal> m_sides;

	vec3<dReal> frame_linear_vel;
	vec3<dReal> frame_angular_vel;

	vec4<dReal> frame_orientation;
	vec3<dReal> frame_position;
	vec3<dReal> frame_offset;

	vec3<dReal> freeze_linear_vel;
	vec3<dReal> freeze_angular_vel;

	vec4<dReal> freeze_orientation;
	vec3<dReal> freeze_position;
	vec3<dReal> freeze_offset;

	raylib::Color m_color;
	raylib::Color m_g_color;

	raylib::Color m_select_color;
	raylib::Color m_active_color;

	bool active;
	bool select;

	bool ghost;

    uint8_t flag_;
	
	bool static_;
	bool interactive_;
    bool composite_;

    uint32_t m_cat_bits;
	uint32_t m_col_bits;

	Body() {};
    Body(BodyID id, std::string name);

    void SetOffset(vec3<dReal> offset);
	void SetColor(raylib::Color color);

	void Create(dWorldID world, dSpaceID space);

	void CreateGeom();
	void CreateBody();
	void CreateStatic();
	void CreateDynamic();
	void CreateComposite(dBodyID b);

	void Step();

	void SetCatBits(uint32_t bits);
	void SetCatBits();

	void SetColBits(uint32_t bits);
	void SetColBits();

	void SetOrientation(raylib::Vector4 q);
	void SetPosition(raylib::Vector3 p);
	void SetLinearVel(raylib::Vector3 v);
	void SetAngularVel(raylib::Vector3 v);

	void Freeze();

	void Refreeze();
	void Reset();

	void DrawObject(raylib::Color color);
	void DrawObjectWires(raylib::Color color);

	void Draw(raylib::Color color);
	void Draw(bool freeze);

	void DrawFreeze(raylib::Color color);
	void DrawSelect();

	void ToggleGhost();
	void ToggleState();

	BodyID GetID();
	std::string GetName();
};

typedef int JointID;

enum JointType
{
	BALL = 1,
	HINGE,
	dSLIDER,

	UNIVERSAL,
	HINGE2,

	FIXED,
	CONTACT,
};

enum JointState
{
	RELAX,
	HOLD,
	FORWARD,
	BACKWARD,
};

struct Joint : public Body
{
	dJointID dJoint;

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

    Joint(JointID id, std::string name);

	void Create(dWorldID world, dSpaceID space, Body b1, Body b2);

	void Draw(raylib::Color color);
	void Draw(bool freeze);

	void DrawFreeze(raylib::Color color);
	void DrawSelect();

	void DrawAxis(bool freeze);
	void DrawRange(bool freeze);

	void TriggerActiveStateAlt(dReal vel);	
	void TriggerPassiveStateAlt(dReal strength);	
	void TriggerActiveState(dReal vel);
	void TriggerPassiveState(dReal strength);	

	void TogglePassiveState();	
	void TogglePassiveStateAlt();	

	void ToggleActiveState(dReal vel);	
	void ToggleActiveState();	
	void ToggleActiveStateAlt(dReal vel);
	void ToggleActiveStateAlt();	
	void CycleState();	
	void CycleStateAlt();
  	void ReverseCycleState();	
	void ReverseCycleStateAlt();
	void Step();
};
