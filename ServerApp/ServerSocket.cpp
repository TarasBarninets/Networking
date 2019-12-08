#include"ServerSocket.h"
#include"FileBrowser.h"
#include <iostream>

ServerSocket::~ServerSocket()
{
	closesocket(m_listenSocket);
	closesocket(m_clientSocket);
	WSACleanup();
}

void ServerSocket::initializeWinsock() const
{
	WSAData wsaData;
	const WORD DLLVersion = MAKEWORD(2, 2);

	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "Error: WSAStartup() failed." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "WSAStartup() executed success. Winsock Initialized." << std::endl;
}

void ServerSocket::setServerAddress(const char* portNumber)
{
	memset(&m_hints, 0, sizeof(m_hints));
	m_hints.ai_family = AF_INET;
	m_hints.ai_socktype = SOCK_STREAM;
	m_hints.ai_protocol = IPPROTO_TCP;
	m_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, portNumber, &m_hints, &m_result) != 0)
	{
		std::cout << "Error: getaddrinfo() failed." << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "getaddrinfo() executed success. addrinfo structure initialized." << std::endl;
}

void ServerSocket::createListenSocket()
{
	m_listenSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET)
	{
		std::cout << "Error: socket() failed." << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "socket() executed success. serverSocket created." << std::endl;
}

void ServerSocket::bindListenSocket()
{
	if (bind(m_listenSocket, m_result->ai_addr, m_result->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "Error: bind() failed." << WSAGetLastError() << std::endl;
		closesocket(m_listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "bind() executed success. Binded a local address with listenSocket." <<std::endl;
}

void ServerSocket::startListening()
{
	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Error: listen() failed." << WSAGetLastError() << std::endl;
		closesocket(m_listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "listen() executed success. listenSocket in a listening state." << std::endl;
}

bool ServerSocket::acceptConnection()
{
	m_clientSocket = accept(m_listenSocket, m_result->ai_addr, (int*)&m_result->ai_addrlen);

	if (m_clientSocket == INVALID_SOCKET)
	{
		std::cout << "Error: accept() failed." << WSAGetLastError() << std::endl;
		closesocket(m_listenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "accept() executed success. Client connected." << std::endl;
	return true;
}

void ServerSocket::sendMessageToClient()
{
	do {
		std::cin >> m_msgToClient;
		{
			std::lock_guard<std::mutex> locker(m_mutexObject);  // Mutex lock shared resource - cout object, and release after using
			std::cout << "> " << m_msgToClient << std::endl;
		}
		send(m_clientSocket, m_msgToClient, sizeof(m_msgToClient), 0);
		memset(&m_msgToClient, 0, sizeof(m_msgToClient));
	} while (!m_disconnectStatus);
}

void ServerSocket::receiveMessageFromClient()
{
	do {
		recv(m_clientSocket, m_msgFromClient, sizeof(m_msgFromClient), 0);
		{
			std::lock_guard<std::mutex> locker(m_mutexObject);  // Mutex lock shared resource - cout object, and release after using
			std::cout << "Client : " << m_msgFromClient << std::endl;
		}
		FileBrowser object;
		bool checkPath = object.setWorkingDirectory(m_msgFromClient);
		if (checkPath)
		{
			std::vector<std::string> contentOfDirectory = object.listOfFilesAndFolders();
			sendContentOfDirectory(contentOfDirectory);
		}

		if (strcmp(m_disconnectCondition, m_msgFromClient) == 0)
			m_disconnectStatus = true;

		memset(&m_msgFromClient, 0, sizeof(m_msgFromClient));
	} while (!m_disconnectStatus);
}

void ServerSocket::sendContentOfDirectory(std::vector<std::string> contentOfDirectory)
{
	for (auto element : contentOfDirectory)
	{
		send(m_clientSocket, element.c_str(), static_cast<int>(element.size()), 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
