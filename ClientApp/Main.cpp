#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <mutex>
#include"ClientSocket.h"

//Add lib ws2_32.lib : Linker->Additional_dependencies-> "ws2_32.lib"

void setConsoleWindow()
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, 670, 600, TRUE);
}

int main()
{
	setConsoleWindow();
	std::cout << "************************************** TCP Client *************************************" << std::endl;

	const char* IP = "127.0.0.1";
	const char* portNumber = "8888";

	ClientSocket object;
	object.initializeWinsock();
	object.setServerAddress(IP, portNumber);
	object.createServerSocket();

	if (object.connectToServer())
	{
		std::thread sendData(&ClientSocket::sendMessageToServer, &object);
		std::thread receiveData(&ClientSocket::receiveMessageFromServer, &object);
		
		sendData.join();
		receiveData.join();
	}
	else
	{
		std::cout << "Error: connectToServer() failed." << std::endl;
	}
	
	return EXIT_SUCCESS;
}