#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <WinSock2.h>
#include <Ws2tcpip.h>


const size_t MAX_BUFFER_SIZE = 256;

class ClientSocket {
public:
	~ClientSocket();
	void initializeWinsock() const;
	void setServerAddress(const char* IP, const char* portNumber);
	void createServerSocket();
	bool connectToServer();
	void sendMessageToServer();
	void receiveMessageFromServer();

private:
	std::mutex m_mutexObject;
	SOCKET m_serverSocket = INVALID_SOCKET;
	addrinfo* m_result = nullptr;
	addrinfo m_hints;
	char m_msgToServer[MAX_BUFFER_SIZE] = { 0 };
	char m_msgFromServer[MAX_BUFFER_SIZE] = { 0 };
	const char* m_disconnectCondition = "disconnect";
	std::atomic<bool> m_disconnectStatus = false;
};