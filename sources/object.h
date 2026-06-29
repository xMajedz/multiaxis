
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
    dReal density = 0.00;
	dReal mass = 0.00; 
  
   	uint32_t flag_ = 0;
	bool static_ = false;
    bool composite_ = false;
	bool interactive_ = false;

    bool active_ = false;

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
