#include "camera.h"

using namespace raylib;
#include "raymath.h"

void Gamecam::Init()
{
	camera.up = { 0.00, 0.00, 1.00 };
	camera.fovy = 45.00;
	camera.projection = CAMERA_PERSPECTIVE;
	camera_offset = { 0.00, 6.00, 2.00 };
}

const Camera& Gamecam::Get()
{
	return camera;
}

const Vector3& Gamecam::GetOffset()
{
    return camera_offset;
}

void Gamecam::CameraRotateZ(float degrees)
{
	Quaternion q = QuaternionFromMatrix(MatrixRotateZ(DEG2RAD * degrees));
	camera_offset = Vector3RotateByQuaternion(camera_offset, q);
}

void Gamecam::CameraRotateX(float degrees)
{
	Quaternion q = QuaternionFromMatrix(MatrixRotateX(DEG2RAD * degrees));
	camera_offset = Vector3RotateByQuaternion(camera_offset, q);
}

void Gamecam::CameraRotateZClockwise()
{
	CameraRotateZ(10);
}

void Gamecam::CameraRotateZCClockwise()
{
	CameraRotateZ(-10);
}

void Gamecam::CameraRotateXClockwise()
{
	CameraRotateX(10);
}

void Gamecam::CameraRotateXCClockwise()
{
	CameraRotateX(-10);
}

void Gamecam::CameraZoomIn()
{
	camera_offset.x *= 0.90;
	camera_offset.y *= 0.90;
	camera_offset.z *= 0.90;
}

void Gamecam::CameraZoomOut()
{
	camera_offset.x *= 1.10;
	camera_offset.y *= 1.10;
	camera_offset.z *= 1.10;
}

void Gamecam::Update()
{
	if (IsKeyDown(KEY_LEFT_SHIFT)) {
		if (IsKeyDown(KEY_W)) {
			CameraRotateXClockwise();
		}

		if (IsKeyDown(KEY_A)) {
			CameraRotateZClockwise();
		}

		if (IsKeyDown(KEY_S)) {
			CameraRotateXCClockwise();
		}

		if (IsKeyDown(KEY_D)) {
			CameraRotateZCClockwise();
		}
	} else {
		if (IsKeyDown(KEY_W)) {
			CameraZoomIn();
		}

		if (IsKeyDown(KEY_A)) {
			CameraRotateZCClockwise();
		}

		if (IsKeyDown(KEY_S)) {
			CameraZoomOut();
		}

		if (IsKeyDown(KEY_D)) {
			CameraRotateZClockwise();
		}
	}
}

void Gamecam::UpdateSpectatorcam(bool freeze, std::vector<Player> players)
{
	float x = 0.00;
	float y = 0.00;
	float z = 0.00;

	size_t p_count = players.size();

	if (p_count > 0) {
		for (auto& p : players) {
			auto offset = p.GetOffset(freeze);
				x += offset.x;
				y += offset.y;
				z += offset.z;
		}

		x = x/p_count;
		y = y/p_count;
		z = z/p_count;
	}

	camera.position.x = camera_offset.x + x;
	camera.position.y = camera_offset.y + y;
	camera.position.z = camera_offset.z + z;

	camera.target.x = x;
	camera.target.y = y;
	camera.target.z = z;
}

void Gamecam::UpdatePlaycam(bool freeze, Player selected_player)
{
	auto offset = selected_player.GetOffset(freeze);

	camera.position.x = camera_offset.x + offset.x;
	camera.position.y = camera_offset.y + offset.y;
	camera.position.z = camera_offset.z + offset.z;

	camera.target.x = offset.x;
	camera.target.y = offset.y;
	camera.target.z = offset.z;
}
