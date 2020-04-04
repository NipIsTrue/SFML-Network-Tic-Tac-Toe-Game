#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Network.hpp>
#include <Log.hpp>

class Game {
public:
	Game(sf::TcpSocket* player1, std::string p1name, sf::TcpSocket* player2, std::string p2name);

	void run();
	void cleanup();

private:
	//void send(sf::TcpSocket* socket, sf::Packet& packet);

	sf::TcpSocket* m_player1;
	sf::TcpSocket* m_player2;

	std::string m_p1name;
	std::string m_p2name;
};

#endif /* GAME_HPP */