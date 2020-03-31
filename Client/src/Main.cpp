#include <SFML/Network.hpp>
#include <iostream>

int main() {
	std::cout << sf::IpAddress::getPublicAddress() << std::endl;
}