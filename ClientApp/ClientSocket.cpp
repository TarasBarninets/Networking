#include "ClientSocket.h"
#include <iostream>

ClientSocket::~ClientSocket()
{
	closesocket(m_serverSocket);
	WSACleanup();
}

void ClientSocket::initializeWinsock() const
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

void ClientSocket::setServerAddress(const char* IP, const char* portNumber)
{
	memset(&m_hints, 0, sizeof(m_hints));
	m_hints.ai_family = AF_INET;
	m_hints.ai_socktype = SOCK_STREAM;
	m_hints.ai_protocol = IPPROTO_TCP;
	m_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(IP, portNumber, &m_hints, &m_result) != 0)
	{
		std::cout << "Error: getaddrinfo() failed." << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "getaddrinfo() executed success. addrinfo structure initialized." << std::endl;
}

void ClientSocket::createServerSocket()
{
	m_serverSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_serverSocket == INVALID_SOCKET)
	{
		std::cout << "Error: socket() failed." << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "socket() executed success. serverSocket created." << std::endl;
}

bool ClientSocket::connectToServer()
{
	if(connect(m_serverSocket, m_result->ai_addr, static_cast<int>(m_result->ai_addrlen)))
	{
		std::cout << "Error: connect() failed." << std::endl;
		return false;
	}

	std::cout << "connection() executed success. Connected to Server." << std::endl;
	return true;
}

void ClientSocket::sendMessageToServer()
{
	do {
		std::cin >> m_msgToServer;
		{
			std::lock_guard<std::mutex> locker(m_mutexObject); //Mutex lock shared resource - cout object, and release after using
			std::cout << "> " << m_msgToServer << std::endl;
		}
		send(m_serverSocket, m_msgToServer, sizeof(m_msgToServer), 0);

		if (strcmp(m_disconnectCondition, m_msgToServer) == 0)
			m_disconnectStatus = true;

		memset(&m_msgToServer, 0, sizeof(m_msgToServer));
	} while (!m_disconnectStatus);
}

void ClientSocket::receiveMessageFromServer()
{
	do {
		recv(m_serverSocket, m_msgFromServer, sizeof(m_msgFromServer), 0);
		{
			std::lock_guard<std::mutex> locker(m_mutexObject);  //Mutex lock shared resource - cout object, and release after using
			std::cout << "Server : " << m_msgFromServer << std::endl;
		}
		memset(&m_msgFromServer, 0, sizeof(m_msgFromServer));
	} while (!m_disconnectStatus);
}

