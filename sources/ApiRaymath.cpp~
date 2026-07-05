#include "api.h"

using namespace raylib;
#include "raymath.h"

static int RAYMATH_MatrixIdentity(lua_State* L)
{
	const Matrix m = MatrixIdentity();
	const float _m[16] = {
		m.m0,  m.m1,  m.m2,  m.m3,
		m.m4,  m.m5,  m.m6,  m.m7,
		m.m8,  m.m9,  m.m10, m.m11,
		m.m12, m.m13, m.m14, m.m15,
	};

	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _m[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_MatrixMultiply(lua_State* L)
{
	Matrix left, right;

	if (lua_istable(L, -1)) {
		float _left[16];
		for (int i = 0; i < 16; i += 1) {
			lua_rawgeti(L, -1 * (1 + i) , 1 + i);
			_left[i] = lua_tonumber(L, -1);
		}

		left = {
			_left[0],  _left[1],  _left[2],  _left[3],
			_left[4],  _left[5],  _left[6],  _left[7],
			_left[8],  _left[9],  _left[10], _left[11],
			_left[12], _left[13], _left[14], _left[15],
		};
	} else {
		lua_newtable(L);
		return 1;
	}

	if (lua_istable(L, -18)) {
		float _right[16];
		for (int i = 0; i < 16; i += 1) {
			lua_rawgeti(L, -1 * (18 + i) , 1 + i);
			_right[i] = lua_tonumber(L, -1);
		}

		right = {
			_right[0],  _right[1],  _right[2],  _right[3],
			_right[4],  _right[5],  _right[6],  _right[7],
			_right[8],  _right[9],  _right[10], _right[11],
			_right[12], _right[13], _right[14], _right[15],
		};
	} else {
		lua_newtable(L);
		return 1;
	}

	const Matrix result = MatrixMultiply(left, right);
		
	const float _result[16] = {
		result.m0,  result.m1,  result.m2,  result.m3,
		result.m4,  result.m5,  result.m6,  result.m7,
		result.m8,  result.m9,  result.m10, result.m11,
		result.m12, result.m13, result.m14, result.m15,
	};
	
	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _result[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_MatrixRotate(lua_State* L)
{
	if (lua_istable(L, -1)) {
		Vector3 axis;
		lua_rawgeti(L, -1, 1);
		axis.x = lua_tonumber(L, -1);
		lua_rawgeti(L, -2, 2);
		axis.y = lua_tonumber(L, -1);
		lua_rawgeti(L, -3, 3);
		axis.z = lua_tonumber(L, -1);

		float angel;
		if (lua_isnumber(L, -4)) {
			angel = lua_tonumber(L, -1);
		}

		const Matrix result = MatrixRotate(axis, angel);

		const float _result[16] = {
			result.m0,  result.m1,  result.m2,  result.m3,
			result.m4,  result.m5,  result.m6,  result.m7,
			result.m8,  result.m9,  result.m10, result.m11,
			result.m12, result.m13, result.m14, result.m15,
		};
		
		lua_newtable(L);
		for (int i = 0; i < 16; i += 1) {
			lua_pushnumber(L, _result[i]);
			lua_rawseti(L, -2, 1 + i);
		}
	}

	return 1;
}

static int RAYMATH_MatrixRotateX(lua_State* L)
{
	const float angle = lua_tonumber(L, -1);
	const Matrix result = MatrixRotateX(angle);

	const float _result[16] = {
		result.m0,  result.m1,  result.m2,  result.m3,
		result.m4,  result.m5,  result.m6,  result.m7,
		result.m8,  result.m9,  result.m10, result.m11,
		result.m12, result.m13, result.m14, result.m15,
	};
	
	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _result[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_MatrixRotateY(lua_State* L)
{
	const float angle = lua_tonumber(L, -1);
	const Matrix result = MatrixRotateY(angle);

	const float _result[16] = {
		result.m0,  result.m1,  result.m2,  result.m3,
		result.m4,  result.m5,  result.m6,  result.m7,
		result.m8,  result.m9,  result.m10, result.m11,
		result.m12, result.m13, result.m14, result.m15,
	};
	
	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _result[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_MatrixRotateZ(lua_State* L)
{
	const float angle = lua_tonumber(L, -1);
	const Matrix result = MatrixRotateZ(angle);

	const float _result[16] = {
		result.m0,  result.m1,  result.m2,  result.m3,
		result.m4,  result.m5,  result.m6,  result.m7,
		result.m8,  result.m9,  result.m10, result.m11,
		result.m12, result.m13, result.m14, result.m15,
	};
	
	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _result[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_MatrixRotateXYZ(lua_State* L)
{
	Vector3 angle;
	lua_rawgeti(L, -1, 1);
	angle.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	angle.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	angle.z = lua_tonumber(L, -1);

	const Matrix result = MatrixRotateXYZ(angle);

	const float _result[16] = {
		result.m0,  result.m1,  result.m2,  result.m3,
		result.m4,  result.m5,  result.m6,  result.m7,
		result.m8,  result.m9,  result.m10, result.m11,
		result.m12, result.m13, result.m14, result.m15,
	};
	
	lua_newtable(L);
	for (int i = 0; i < 16; i += 1) {
		lua_pushnumber(L, _result[i]);
		lua_rawseti(L, -2, 1 + i);
	}

	return 1;
}

static int RAYMATH_QuaternionFromMatrix(lua_State* L)
{
	Matrix m;
	float _m[16];
	for (int i = 0; i < 16; i += 1) {
		lua_rawgeti(L, -1 * (1 + i) , 1 + i);
		_m[i] = lua_tonumber(L, -1);
	}

	m = {
		_m[0],  _m[1],  _m[2],  _m[3],
		_m[4],  _m[5],  _m[6],  _m[7],
		_m[8],  _m[9],  _m[10], _m[11],
		_m[12], _m[13], _m[14], _m[15],
	};

	Quaternion q = QuaternionFromMatrix(m);

	lua_newtable(L);

	lua_pushnumber(L, q.w);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, q.x);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, q.y);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, q.z);
	lua_rawseti(L, -2, 4);
	return 1;
}

static int RAYMATH_QuaternionIdentity(lua_State* L)
{
	Quaternion q = QuaternionIdentity();

	lua_newtable(L);

	lua_pushnumber(L, q.w);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, q.x);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, q.y);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, q.z);
	lua_rawseti(L, -2, 4);
	return 1;
}

static int RAYMATH_QuaternionMultiply(lua_State* L)
{
	Quaternion p;
	lua_rawgeti(L, -1, 1);
	p.w = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	p.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	p.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	p.z = lua_tonumber(L, -1);

	Quaternion q;
	lua_rawgeti(L, -6, 1);
	q.w = lua_tonumber(L, -1);
	lua_rawgeti(L, -7, 2);
	q.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -8, 3);
	q.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -9, 4);
	q.z = lua_tonumber(L, -1);

	q = QuaternionMultiply(q, p);

	lua_newtable(L);
	lua_pushnumber(L, q.w);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, q.x);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, q.y);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, q.z);
	lua_rawseti(L, -2, 4);

	return 1;
}

static int RAYMATH_Vector3RotateByQuaternion(lua_State* L)
{
	Quaternion q;
	lua_rawgeti(L, -1, 1);
	q.w = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	q.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	q.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	q.z = lua_tonumber(L, -1);

	Vector3 v;
	lua_rawgeti(L, -6, 1);
	v.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -7, 2);
	v.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -8, 3);
	v.z = lua_tonumber(L, -1);

	v = Vector3RotateByQuaternion(v, q);

	lua_newtable(L);
	lua_pushnumber(L, v.x);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, v.y);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, v.z);
	lua_rawseti(L, -2, 3);
	return 1;
}

static const luaL_Reg ApiRaymath[] {
	{"MatrixIdentity", RAYMATH_MatrixIdentity},
	{"MatrixMultiply", RAYMATH_MatrixMultiply},
	{"MatrixRotate", RAYMATH_MatrixRotate},
	{"MatrixRotateX", RAYMATH_MatrixRotateX},
	{"MatrixRotateY", RAYMATH_MatrixRotateY},
	{"MatrixRotateZ", RAYMATH_MatrixRotateZ},
	{"MatrixRotateXYZ", RAYMATH_MatrixRotateXYZ},
	{"QuaternionFromMatrix", RAYMATH_QuaternionFromMatrix},
	{"QuaternionIdentity", RAYMATH_QuaternionIdentity},
	{"QuaternionMultiply", RAYMATH_QuaternionMultiply},
	{"Vector3RotateByQuaternion", RAYMATH_Vector3RotateByQuaternion},
	{NULL, NULL},
};

int luaopenApiRaymath(lua_State* L)
{
	luaL_register(L, "RAYMATH", ApiRaymath);

	lua_getglobal(L, "RAYMATH");
	lua_pushnumber(L, PI);
	lua_setfield(L, -2, "PI");
	lua_pushnumber(L, EPSILON);
	lua_setfield(L, -2, "EPSILON");
	lua_pushnumber(L, DEG2RAD);
	lua_setfield(L, -2, "DEG2RAD");
	lua_pushnumber(L, RAD2DEG);
	lua_setfield(L, -2, "RAD2DEG");
	lua_pop(L, 1);
	return 1;
}
