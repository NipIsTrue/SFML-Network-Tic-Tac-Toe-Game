#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <array>
#include <string>
#include <SFML/Network.hpp>
#include <mutex>
#include "Log.hpp"

enum class BoardState {
	Empty,
	Self,
	Opponent
};

inline static void error(const std::string& message) {
	Log::logError(message);
	throw std::runtime_error(message);
}

class Client {
public:
	void connect(const std::string& ip, int port, const std::string& name);

	void place(int x, int y);

	void wait();

	std::array<std::array<BoardState, 3>, 3>  board;
	std::string opponentName;

	bool running;

private:
	bool turn = false;
	sf::TcpSocket m_socket;

	std::mutex m;
};

#endif /* CLIENT_HPP */k