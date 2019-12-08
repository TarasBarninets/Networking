#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <fileapi.h>
#include"FileBrowser.h"
#include"ServerSocket.h"

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
	std::cout << "************************************** TCP Server *************************************" << std::endl;

	const char* portNumber = "8888";

	ServerSocket object;
	object.initializeWinsock();
	object.setServerAddress(portNumber);
	object.createListenSocket();
	object.bindListenSocket();
	object.startListening();

	if (object.acceptConnection())
	{
		std::thread sendData(&ServerSocket::sendMessageToClient, &object);
		std::thread receiveData(&ServerSocket::receiveMessageFromClient, &object);

		sendData.join();
		receiveData.join();
	}
	else
	{
		std::cout << "Error: acceptConnection() failed." << std::endl;
	}
		
	return EXIT_SUCCESS;
}