#include <SFML/Network.hpp>
#include <iostream>
#include <Log.hpp>
#include <thread>
#include "Game.hpp"

int main() {
	Log::init("server log.txt");

	//std::vector<
	sf::TcpListener listener;
	if (listener.listen(1010) != sf::Socket::Done) {
		Log::logError("Error: could not bind listener [Main.cpp:" + LINE);
		Log::close();
		return -1;
	}

	std::vector<std::pair<Game*, std::thread>> games;

	bool first = true;
	sf::TcpSocket* other = nullptr;
	std::string otherName;
	sf::TcpSocket* socket;
	while (true) {
		socket = new sf::TcpSocket;
		if (listener.accept(*socket) != sf::Socket::Done) {
			Log::logError("Error: could not accept client [Main.cpp:" + LINE);
		}
		else {
			sf::Packet p;
			if (socket->receive(p) != sf::Socket::Done) {
				Log::logError("Error: could not receive data from client [Main.cpp:" + LINE);
			}
			else {
				std::string name;
				if (p >> name) {
					Log::logStatus("Client \"" + name + "\" connected to server from " + socket->getRemoteAddress().toString(), ConsoleColor::LightPurple);
					if (first) {
						first = false;
						other = socket;
						otherName = name;
					}
					else {
						Game* game = new Game(other, otherName, socket, name);
						std::thread t(&Game::run, game);
						games.push_back(std::make_pair(game, std::move(t)));
						first = true;
					}
				}
				else {
					Log::logError("Error: could not read data from client [Main.cpp:" + LINE);
				}
			}
		}
	}

	Log::close();
	return 0;
}