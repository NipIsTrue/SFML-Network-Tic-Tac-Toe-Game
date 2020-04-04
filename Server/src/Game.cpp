#include "Game.hpp"
#include <array>

Game::Game(sf::TcpSocket* player1, std::string p1name, sf::TcpSocket* player2, std::string p2name) :
	m_player1(player1), m_p1name(p1name), m_p2name(p2name), m_player2(player2) {
	Log::logStatus("Starting game between players \"" + p1name + "\" and \"" + p2name + "\"", ConsoleColor::DarkBlue);
}

using Board = std::array<std::array<int, 3>, 3>;

bool checkWin(const Board& board, int i) {
	for (auto& row : board) {
		bool t = true;
		for (auto& space : row) {
			if (space != i) t = false;
		}
		if (t) return true;
	}

	for (int y = 0; y < 3; y++) {
		bool t = true;
		for (int x = 0; x < 3; x++) {
			if (board[x][y] != i) t = false;
		}
		if (t) return true;
	}

	bool t = true;
	for (int z = 0; z < 3; z++) {
		if (board[z][z] != i) t = false;
	}
	if (t) return true;

	return board[2][0] == i && board[1][1] == i && board[0][2] == i;
}

void Game::run() {
	sf::Packet p;
	p << m_p1name << false;
	if (m_player2->send(p) != sf::Socket::Done) {
		Log::logError("Error: could not send data to player \"" + m_p1name + "\"");
		cleanup();
		return;
	}
	p.clear();
	p << m_p2name << true;
	if (m_player1->send(p) != sf::Socket::Done) {
		Log::logError("Error: could not send data to player \"" + m_p2name + "\"");
		cleanup();
		return;
	}

	Board board {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
	Log::logStatus(std::to_string(board[0][0]));

	bool running = true;
	while (running) {
		p.clear();
		if (m_player1->receive(p) != sf::Socket::Done) {
			Log::logError("Error: could not receive data from player \"" + m_p1name + "\"");
			cleanup();
			return;
		}
		int x, y;
		if (!(p >> x >> y)) {
			Log::logError("Error: could not read data from player \"" + m_p1name + "\"");
			cleanup();
			return;
		}

		board[x][y] = 1;
		if (m_player2->send(p) != sf::Socket::Done) {
			Log::logError("Error: could not send data to player \"" + m_p2name + "\"");
			cleanup();
			return;
		}

		if (checkWin(board, 1)) {
			Log::logStatus("Player \"" + m_p1name + "\" won", ConsoleColor::Gold);
			p.clear();
			p << -1 << -1;
			if (m_player1->send(p) != sf::Socket::Done) {
				Log::logError("Error: could not send data to player \"" + m_p1name + "\"");
				cleanup();
				return;
			}
			p.clear();
			p << -2 << -2;
			if (m_player2->send(p) != sf::Socket::Done) {
				Log::logError("Error: could not send data to player \"" + m_p2name + "\"");
				cleanup();
				return;
			}
			running = false;
			break;
		}

		p.clear();
		if (m_player2->receive(p) != sf::Socket::Done) {
			Log::logError("Error: could not receive data from player \"" + m_p2name + "\"");
			cleanup();
			return;
		}
		if (!(p >> x >> y)) {
			Log::logError("Error: could not read data from player \"" + m_p2name + "\"");
			cleanup();
			return;
		}

		board[x][y] = 2;
		if (m_player1->send(p) != sf::Socket::Done) {
			Log::logError("Error: could not send data to player \"" + m_p1name + "\"");
			cleanup();
			return;
		}

		if (checkWin(board, 2)) {
			Log::logStatus("Player \"" + m_p2name + "\" won", ConsoleColor::Gold);
			p.clear();
			p << -2 << -2;
			if (m_player1->send(p) != sf::Socket::Done) {
				Log::logError("Error: could not send data to player \"" + m_p1name + "\"");
				cleanup();
				return;
			}
			p.clear();
			p << -1 << -1;
			if (m_player2->send(p) != sf::Socket::Done) {
				Log::logError("Error: could not send data to player \"" + m_p2name + "\"");
				cleanup();
				return;
			}
			running = false;
			break;
		}
	}

	cleanup();
	Log::logStatus("Game between players \"" + m_p1name + "\" and \"" + m_p2name + "\" ended", ConsoleColor::DarkBlue);
}

void Game::cleanup() {
	m_player1->disconnect();
	m_player2->disconnect();
	delete m_player1;
	delete m_player2;
}