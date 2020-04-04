#include <iostream>
#include <string>

int main(int argc, char** argv) {
	if (argc > 1 && !strcmp(argv[1], "-vs")) {
		system("Start ../x64/Debug/Server.exe");
		system("Start ../x64/Debug/Client.exe");
		system("Start ../x64/Debug/Client.exe");
	}
	else {
		system("Start Server.exe");
		system("Start Client.exe");
		system("Start Client.exe");
	}
}