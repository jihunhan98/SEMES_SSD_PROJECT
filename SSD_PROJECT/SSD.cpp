#include"SSD.h"
#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<boost/asio.hpp>
using boost::asio::ip::tcp;
std::string FileName = "NAND.txt";

enum class CommandType {
	Write, Read
};
struct payload {
	CommandType type;
	int LBA;
	int VALUE;
};

void MakeNand() {
	std::fstream fout;
	fout.open(FileName, std::ios::out);
	for (int line = 0; line < 100; line++)
	{
		fout << "00000000\n";
	}
	fout.close();
}

bool Write(int LBA, int VALUE)
{
	std::fstream fout;
	fout.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
	fout.seekp(LBA * 10);
	fout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << VALUE;
	return true;
}

unsigned int Read(int LBA)
{
	std::ifstream fin;
	std::string buf;
	fin.open(FileName);
	fin.seekg(LBA * 10);
	std::getline(fin, buf);
	int ret = 0;
	for (int i = 0; i < 8; i++)
	{
		ret *= 16;
		if (isascii(buf[i]))
			ret += buf[i] - '0';
		else
			ret += buf[i] = 'A' + 10;
	}
	return ret;
}

int main()
{
	std::ifstream NAND(FileName);
	if (!NAND.is_open())
	{
		std::cout << "파일 없음" << std::endl;
		MakeNand();
	}
	else
		std::cout << "파일 존재";
	try {
		boost::asio::io_context io;

		tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));

		//std::cout << "Server started on port 12345\n";

		while (true) {
			tcp::socket socket(io);
			acceptor.accept(socket);

			//std::cout << "Client connected\n";

			while (true) {
				char data[1024] = {};

				//client에게 입력 받음
				boost::system::error_code ec;
				size_t length = socket.read_some(boost::asio::buffer(data), ec);

				if (ec == boost::asio::error::eof)
					break; // 정상 종료
				else if (ec)
					throw boost::system::system_error(ec);
				std::cout << data;
				// echo
				// client에게 전송
				boost::asio::write(socket, boost::asio::buffer(data, length));
			}

			//std::cout << "Client disconnected\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}