#pragma once

#include <boost/thread/thread.hpp>

class Client
{
public:
    Client();
    ~Client();

private:
    boost::thread m_recvThread;
    SOCKET m_socket;
    void RecvThread();
    int ReadMessage(SOCKET &sock, char *buf, int buf_size);
    int ReadMessagePeek(SOCKET &sock, char *buf, int buf_size);
    void CloseSocket();

public:
    int Connect(const char *ip, const int port);
    int Send(const char *buf, const int buf_size);
    void Recv();
};

