#include "api.h"
#include "game.h"

#include "netcode_client.h"
#include "netcode_server.h"

int main()
{

	Window::Init();

	Game::Init();

	int status = Client::Connect();

	if (status != 0) {
		Server::HostGameThread();
		Server::WaitGameThread();

		for (int attempts = 0 ; status != 0 && attempts < 5; attempts += 1) {
			status = Client::Connect();
		}
	}

	bool running = true;

	while (running)
	{
		Window::Update();

		if (raylib::WindowShouldClose()) {
			Client::Disconnect();
			running = false;
		}

		Client::Update(Game::GetTime(), Game::GetFrameTime());

		Window::Draw();

		Console::Update();
	}

	Server::CloseThread();
}
