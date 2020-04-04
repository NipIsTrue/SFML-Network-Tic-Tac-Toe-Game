#include <SFML/Graphics.hpp>
#include <iostream>
#include <future>
#include <conio.h>
#include "Client.hpp"

void drawBoard(sf::RenderTarget& window, Client& client) {
	sf::RectangleShape shape;
	shape.setSize(sf::Vector2f(320, 10));
	shape.setPosition(50, 150);
	shape.setFillColor(sf::Color::Black);
	window.draw(shape);

	shape.setPosition(50, 260);
	window.draw(shape);

	shape.setSize(sf::Vector2f(10, 320));
	shape.setPosition(150, 50);
	window.draw(shape);

	shape.setPosition(260, 50);
	window.draw(shape);

	sf::CircleShape circle(40);
	for (unsigned int x = 0; x < 3; x++) {
		for (unsigned int y = 0; y < 3; y++) {
			switch (client.board[x][y]) {
			case BoardState::Empty:
				break;
			case BoardState::Self:
				circle.setPosition((x * 100) + (60 + (x * 10)), (y * 100) + (60 + (y * 10)));
				circle.setOutlineColor(sf::Color::Blue);
				circle.setOutlineThickness(4.0f);
				window.draw(circle);
				break;
			case BoardState::Opponent:
				sf::VertexArray va(sf::Quads);
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + (x * 10)), (y * 100) + 38 + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 80 + (x * 10)), (y * 100) + 38 + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 80 + (x * 10)), (y * 100) + 42 + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + (x * 10)), (y * 100) + 42 + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 38 + (x * 10)), (y * 100) + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 42 + (x * 10)), (y * 100) + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 42 + (x * 10)), (y * 100) + 80 + (60 + (y * 10))), sf::Color::Red));
				va.append(sf::Vertex(sf::Vector2f((x * 100) + (60 + 38 + (x * 10)), (y * 100) + 80 + (60 + (y * 10))), sf::Color::Red));
				sf::Transform transform;
				transform.rotate(45, sf::Vector2f((x * 100) + 40 + (60 + (x * 10)), (y * 100) + 40 + (60 + (y * 10))));
				window.draw(va, transform);
				break;
			}
		}
	}
}

int main() {
	std::cout << "Enter name: " << std::endl;
	std::string name;
	std::getline(std::cin, name);

	Log::init("log.txt");

	Client client;
	try {
		client.connect("localhost", 1010, name);
	}
	catch (std::runtime_error) {
		Log::logError("Unexpected runtime error, exiting...");
		_getch();
		return -1;
	}

	auto t = std::async(std::launch::async, &Client::wait, &client);

	sf::ContextSettings cs;
	cs.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(420, 420), "Tic Tac Toe", sf::Style::Default, cs);

	window.setFramerateLimit(60);
	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				window.close();

			else if (e.type == sf::Event::MouseButtonReleased) {
				if (e.mouseButton.button == sf::Mouse::Left && e.mouseButton.x > 50 && e.mouseButton.x < 370 &&
					e.mouseButton.y > 50 && e.mouseButton.y < 370) {
					int sx = (e.mouseButton.x - 50) / 100;
					int sy = (e.mouseButton.y - 50) / 100;
					if (sx > 2) sx = 2;
					if (sy > 2) sy = 2;

					try {
						client.place(sx, sy);
					}
					catch (std::runtime_error) {
						Log::logError("Unexpected runtime error, exiting...");
						_getch();
						return -1;
					}
				}
			}

			if (!client.running) window.close();

			window.clear(sf::Color::White);

			drawBoard(window, client);

			window.display();
		}
	}

	Log::close();
	_getch();
}