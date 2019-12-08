#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <WinSock2.h>
#include <Ws2tcpip.h>

const size_t MAX_BUFFER_SIZE = 256;

class ServerSocket {
public:
	~ServerSocket();
	void initializeWinsock() const;
	void setServerAddress(const char* portNumber);
	void createListenSocket();
	void bindListenSocket();
	void startListening();
	bool acceptConnection();
	void sendMessageToClient();
	void receiveMessageFromClient();

private:
	void sendContentOfDirectory(std::vector<std::string> contentOfDirectory);
	std::mutex m_mutexObject;
	SOCKET m_listenSocket = INVALID_SOCKET;
	SOCKET m_clientSocket = INVALID_SOCKET;
	addrinfo* m_result = nullptr;
	addrinfo m_hints;
	char m_msgToClient[MAX_BUFFER_SIZE] = { 0 };
	char m_msgFromClient[MAX_BUFFER_SIZE] = { 0 };
	const char* m_disconnectCondition = "disconnect";
	std::atomic<bool> m_disconnectStatus = false;
};