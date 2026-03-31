#include"SSD.h"
#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<vector>
#include<boost/asio.hpp>
#include"../SHELL/ShellManager.h"
using boost::asio::ip::tcp;
std::string FileName = "NAND.txt";

//enum class CommandType {
//	Write, Read
//};
//struct payload {
//	CommandType type;
//	int LBA;
//	int VALUE;
//};


std::vector<std::string> split(std::string& _s, char _deli)
{
	std::vector<std::string> rst;
	std::istringstream istream(_s);
	std::string buffer;

	while (getline(istream, buffer, _deli))
	{
		rst.push_back(buffer);
	}

	return rst;
}

void MakeNand() {
	std::fstream fout;
	fout.open(FileName, std::ios::out);
	for (int line = 0; line < 100; line++)
	{
		fout << "00000000\n";
	}
	fout.close();
}

std::string Write(int LBA, std::string VALUE)
{
	std::string ret = "ERROR";
	std::fstream fout;
	fout.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
	if (!fout.is_open())
		return ret;
	fout.seekp(LBA * 10);
	fout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << VALUE.substr(2);
	ret = "SUCCESS";
	fout.close();
	return ret;

}

std::string Read(int LBA)
{
	std::ifstream fin;
	std::string buf;
	fin.open(FileName);
	if (!fin.is_open())
		return "ERROR";
	fin.seekg(LBA * 10);
	std::getline(fin, buf);
	std::string ret = "0x";
	ret += buf;
	return ret;
}

int main()
{
	std::ifstream NAND(FileName);
	if (!NAND.is_open())
	{
		MakeNand();
	}
	try {
		boost::asio::io_context io;

		tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));

		while (true) {
			tcp::socket socket(io);
			acceptor.accept(socket);

			std::cout << "Client connected\n";

			while (true) {
				char data[1024] = {};

				//client에게 입력 받음
				boost::system::error_code ec;
				size_t length = socket.read_some(boost::asio::buffer(data), ec);

				if (ec == boost::asio::error::eof)
					break;
				else if (ec)
					throw boost::system::system_error(ec);
				std::cout << std::string(data, length) << std::endl;
				// echo
				// client에게 전송
				std::string command(data);
				std::vector<std::string> vec = split(command, ' ');
				std::string result = "ERROR";
				if (vec[0] == "write")
				{
					result = Write(stoi(vec[1]), vec[2]);
				}
				else if (vec[0] == "read")
				{
					result = Read(stoi(vec[1]));
					std::cout << result << std::endl;
				}
				boost::asio::write(socket, boost::asio::buffer(result));
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
