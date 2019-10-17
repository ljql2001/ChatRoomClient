#include "pch.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include "Client.h"
#include "Utils.h"

#define MAXDATASIZE 1024 * 32

Client::Client():
    m_socket(INVALID_SOCKET)
{
}


Client::~Client()
{
    m_recvThread.interrupt();
    m_recvThread.join();
    CloseSocket();
}

int Client::Connect(const char *ip, const int port) {
    int r = 0;
    SOCKADDR_IN srv;
    ZeroMemory(&srv, sizeof(srv));

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    std::stringstream ss;
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        ss.str("");
        ss << "server error,failed to init socket: "
            << "ip=" << ip << "|"
            << "port=" << port << "|"
            << "error=" << GetLastError() << "|";
        std::cout << ss.str().c_str() << std::endl;
        r = -1;
        goto ExitHere;
    }

    srv.sin_addr.S_un.S_addr = inet_addr(ip);
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    if (connect(m_socket, (SOCKADDR*)&srv, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        ss.str("");
        ss << "server error,failed to connect server: "
            << "ip=" << ip << "|"
            << "port=" << port << "|";
        std::cout << ss.str().c_str() << std::endl;
        r = -1;
        goto ExitHere;
    }

ExitHere:
    return r;
}

void Client::Recv() {
    if (m_socket != INVALID_SOCKET) m_recvThread = boost::thread((boost::bind(&Client::RecvThread, this)));
}

int Client::Send(const char *buf, const int buf_size) {
    //--- checks
    if (m_socket == INVALID_SOCKET) return FALSE;
    //--- send data
    if ((send(m_socket, buf, buf_size, 0)) < 1) {
        m_socket = INVALID_SOCKET; return FALSE;
    }
    return TRUE;
}

void Client::RecvThread() {
    std::stringstream ss;
    char recvbuf[MAXDATASIZE];
    try {
        int retval = 0;
        while (m_socket != INVALID_SOCKET) {
            ZeroMemory(recvbuf, sizeof(recvbuf));
            if (ReadMessage(m_socket, recvbuf, MAXDATASIZE) > 0) {
                ss.str("");
                std::string message = recvbuf;
                std::string gb2312 = Utils::U2G(message);
                ss << gb2312;
                std::cout << ss.str().c_str() << std::endl;
            } else {//socket disconnect
                ss.str("");
                ss << "failed to receive message.";
                std::cout << ss.str().c_str() << std::endl;
                break;
            }
        }
    } catch (...) {
        ss.str("");
        ss << "failed to receive quote message.";
        std::cout << ss.str().c_str() << std::endl;
        CloseSocket();
    }
    ss.str("");
    ss << "receive message thread stop.";
    std::cout << ss.str().c_str() << std::endl;
}

//
int Client::ReadMessage(SOCKET &sock, char *buf, int buf_size) {
    return recv(sock, buf, buf_size, MSG_PARTIAL);
}
int Client::ReadMessagePeek(SOCKET &sock, char *buf, int buf_size) {
    return recv(sock, buf, buf_size, MSG_PEEK);
}

void Client::CloseSocket() {
    try {
        if (m_socket != INVALID_SOCKET) {
            shutdown(m_socket, 2);
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        WSACleanup();
    } catch (...) {
        std::stringstream ss;
        ss << "failed to close socket";
        std::cout << ss.str().c_str() << std::endl;
    }
}

