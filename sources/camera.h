#pragma once
#include "common.h"
#include "game.h"

enum Gamecammode
{
	PLAYER,
	SPECTATOR,
};

namespace Gamecam
{
	static raylib::Camera camera;
	static raylib::Vector3 camera_offset;

	static Gamecammode mode;
	
	void Init();

	const raylib::Camera& Get();
	const raylib::Vector3& GetOffset();

	void CameraRotateZ(float degrees);
	void CameraRotateX(float degrees);
	void CameraRotateZClockwise();
	void CameraRotateZCClockwise();
	void CameraRotateXClockwise();
	void CameraRotateXCClockwise();
	void CameraZoomIn();
	void CameraZoomOut();

	void Update();
	void UpdateSpectatorcam(bool freeze, std::vector<Player> players);
	void UpdatePlaycam(bool freeze, Player selected_player);
};

