#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <vector>

using namespace boost;

std::vector<asio::ip::tcp::socket> Connections;
int Counter = 0;

void ClientHandler(int index);

int main()
{
    asio::io_service service;
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 1111);
    asio::ip::tcp::acceptor acceptor(service, endpoint);

    std::cout << "Listening on port 1111..." << std::endl;

    while (true)
    {
        asio::ip::tcp::socket newConnection(service);
        acceptor.accept(newConnection);

        std::cout << "Client connected" << std::endl;

        std::string msg;
        int msgSize = msg.size();

        asio::write(newConnection, asio::buffer((char*)&msgSize, sizeof(int)));
        asio::write(newConnection, asio::buffer(msg.c_str(), msgSize));

        Connections.push_back(std::move(newConnection));
        Counter++;

        std::thread(ClientHandler, Counter - 1).detach();
    }

    return 0;
}

void ClientHandler(int index)
{
    int msgSize;

    while (true)
    {
        Connections[index].read_some(asio::buffer((char*)&msgSize, sizeof(int)));

        char* msg = new char[msgSize + 1];
        msg[msgSize] = '\0';

        Connections[index].read_some(asio::buffer(msg, msgSize));

        for (int i = 0; i < Counter; i++)
        {
            if (i == index)
                continue;

            asio::write(Connections[i], asio::buffer((char*)&msgSize, sizeof(int)));
            asio::write(Connections[i], asio::buffer(msg, msgSize));
        }

        delete[] msg;
    }
}
