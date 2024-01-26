#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "boost/asio/ts/buffer.hpp"
#include "boost/asio/ts/internet.hpp"

using namespace boost;

asio::io_service service;
asio::ip::tcp::socket connection(service);

void clientHandler();

int main()
{
	try
	{
		asio::ip::tcp::endpoint serverIP(asio::ip::address::from_string("127.0.0.1"), 1111);
		connection.connect(serverIP);

		std::cout << "Connected!" << std::endl;

		std::thread(clientHandler).detach();
		std::string message;
		while (true)
		{
			std::getline(std::cin, message);
			int msgSize = message.size();

			asio::write(connection, asio::buffer(&msgSize, sizeof(msgSize)));
			asio::write(connection, asio::buffer(message.c_str(), msgSize));

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	catch (const std::exception& ec)
	{
		std::cout << "Error: " << ec.what() << std::endl;
	}
}

void clientHandler()
{
	try
	{
		int msgSize = 0;
		while (true)
		{
			asio::read(connection, asio::buffer(&msgSize, sizeof(msgSize)));

			char* message = new char[msgSize + 1];
			message[msgSize] = '\0';

			asio::read(connection, asio::buffer(message, msgSize));

			std::cout << message << std::endl;
			delete[] message;
		}
	}
	catch (const std::exception& ec)
	{
		std::cout << "Error in clientHandler: " << ec.what() << std::endl;
	}
}
