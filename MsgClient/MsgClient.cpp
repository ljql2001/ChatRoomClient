// MsgClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "Client.h"
#include "Utils.h"

static const char *ip_address = "xx.xx.xx.xx";
static const int port = 5000;

int main()
{
    Client client;
    client.Connect(ip_address, port);
    client.Recv();

    std::string message;
    while (message != "/quit" && message != "/exit") {
        std::getline(std::cin, message);
        std::string utf8 = Utils::G2U(message);
        client.Send(utf8.c_str(), utf8.size());
    }
}
