//#include <iostream>
//#include <string>
//#include <vector>
//
//#include "boost/asio.hpp"
//#include "boost/asio/ts/buffer.hpp"
//
//using namespace boost;
//
//using Socket = asio::ip::tcp::socket;
//std::vector<Socket> sockets(100);
//
//int countOfConnections = 0;
//
//void clientHandler(int index);
//
//
//int main()
//{
//	try
//	{
//		asio::io_service service;
//		asio::ip::tcp::endpoint serverIP(asio::ip::address::from_string("127.0.0.1"), 1111);
//		asio::ip::tcp::acceptor acceptor(service, serverIP);
//
//		while (true)
//		{
//			Socket newConnection(service);
//			acceptor.accept(newConnection);
//
//			std::string message = "Hello. It is my server on boost.asio";
//			int msgSize = message.size();
//
//			newConnection.write_some(asio::buffer((char*)&msgSize, sizeof(msgSize)));
//			newConnection.write_some(asio::buffer(message.c_str(), msgSize));
//
//			sockets.push_back(std::move(newConnection));
//			countOfConnections++;
//
//			std::thread(clientHandler, countOfConnections - 1).detach();
//		}
//	}
//	catch (const std::exception& ec)
//	{
//		std::cout << "Error: " << ec.what() << std::endl;
//	}
//
//	return 0;
//}
//
//void clientHandler(int index)
//{
//	try
//	{
//		int msgSize = 0;
//		sockets[index].read_some(asio::buffer((char*)&msgSize, sizeof(msgSize)));
//
//		char* message = new char[msgSize + 1];
//		message[msgSize] = '\0';
//
//		sockets[index].read_some(asio::buffer(message, msgSize));
//
//		for (int i = 0; i < sockets.size(); i++)
//		{
//			if (i == index) continue;
//
//			asio::write(sockets[i], asio::buffer((char*)&msgSize, sizeof(msgSize)));
//			asio::write(sockets[i], asio::buffer(message, msgSize));
//		}
//
//		delete[] message;
//	}
//	catch (const std::exception& ec)
//	{
//		std::cout << "Error in clientHandler: " << ec.what() << std::endl;
//	}
//}

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