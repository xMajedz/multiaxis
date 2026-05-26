#include "netcode_server.h"
#include "api.h"

#include <iostream>

int main()
{
	raylib::SetTraceLogLevel(raylib::LOG_ERROR);
	Server::HostGameThread();
	char input;
	bool running = true;
	while (running) {
		std::cin >> input;
		char message[2] = { input, '\0' };
		Console::log(message);
		Console::Update();
		running = input != 'q';
	}
	Server::CloseThread();
}
