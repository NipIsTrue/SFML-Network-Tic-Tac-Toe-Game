#include "Client.hpp"
#include <conio.h>

void Client::connect(const std::string& ip, int port, const std::string& name) {
	Log::logStatus("Connecting to server at " + ip + " on port " + std::to_string(port), ConsoleColor::DarkBlue);

	if (m_socket.connect(ip, port) != sf::Socket::Done) {
		error("Error: could not connect to server [Client.cpp:" + LINE);
	}
	sf::Packet p;
	p << name;
	if (m_socket.send(p) != sf::Socket::Done) {
		error("Error: could not send data to server [Client.cpp:" + LINE);
	}
	Log::logStatus("Successfully connected to server", ConsoleColor::DarkBlue);

	p.clear();
	if (m_socket.receive(p) != sf::Socket::Done) {
		error("Error: could not find a match [Client.cpp:" + LINE);
	}
	if (!(p >> opponentName >> turn)) {
		error("Error: could not turn data from server [Client.cpp" + LINE);
	}
	Log::logStatus("Opponent found!", ConsoleColor::Yellow);
	Log::logStatus("You have been matched with \"" + opponentName + "\"", ConsoleColor::Yellow);
	Log::logStatus(turn ? "Your turn" : opponentName + "'s turn", ConsoleColor::Gold);

	for (auto& row : board) {
		for (auto& space : row) {
			space = BoardState::Empty;
		}
	}
}

void Client::place(int x, int y) {
	if (board[x][y] != BoardState::Empty) {
		Log::logStatus("That spot is already taken!", ConsoleColor::Pink);
		return;
	}
	if (!turn) {
		Log::logStatus("It's not your turn!", ConsoleColor::Pink);
		return;
	}

	board[x][y] = BoardState::Self;
	Log::logStatus("Square [" + std::to_string(x) + ", " + std::to_string(y) + "] selected", ConsoleColor::NeonBlue);
	sf::Packet p;
	p << x << y;
	if (m_socket.send(p) != sf::Socket::Done) {
		error("Error: could not send data to server [Client.cpp:" + LINE);
	}

	turn = false;
}

void Client::wait() {
	while (running) {
		sf::Packet p;
		if (m_socket.receive(p) != sf::Socket::Done) {
			Log::logError("Error: could not receive data from server [Client.cpp:" + LINE);
			m_socket.disconnect();
			_getch();
			exit(-1);
		}
		int x, y;
		if (!(p >> x >> y)) {
			Log::logError("Error: could not read data from server [Client.cpp:" + LINE);
			m_socket.disconnect();
			_getch();
			exit(-1);
		}

		if (x == -1 && y == -1) {
			running = false;
			Log::logStatus("You won!");
			break;
		}
		else if (x == -2 && y == -2) {
			running = false;
			Log::logStatus("You lost");
			break;
		}

		m.lock();
		board[x][y] = BoardState::Opponent;
		turn = true;
		m.unlock();
	}
}