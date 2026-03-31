#include "ShellManager.h"
#include <string>
#include <vector>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

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

unsigned long convertHexValue(const std::string& str)
{
	const char* cstr = str.c_str();
	std::stringstream convert(cstr);
	unsigned long nHexValue;
	convert >> std::hex >> nHexValue;

	return nHexValue;
}

int main()
{
	try {
		boost::asio::io_context io;
		tcp::socket socket(io);
		socket.connect(tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345));

		ShellManager _shellManager;

		while (true)
		{
			std::string input_text;
			std::getline(std::cin, input_text);
			std::vector<std::string> vec = split(input_text, ' ');

			if (vec.empty() || vec[0].empty()) {
				_shellManager.PrintInvalidCommand();
				continue;
			}

			if (vec[0] == "write") {
				if (vec.size() != 3) {
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateLba(vec[1]) || !_shellManager.ValidateHexNumber(vec[2])) {
					_shellManager.PrintError();
				}
				else {
					boost::asio::write(socket, boost::asio::buffer(input_text));

					char reply[1024] = {};
					size_t length = socket.read_some(boost::asio::buffer(reply));
					std::cout << std::string(reply, length) << std::endl;
				}
			}

			else if (vec[0] == "read") {
				if (vec.size() != 2) {
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateLba(vec[1])) {
					_shellManager.PrintError();
				}
				else {
					boost::asio::write(socket, boost::asio::buffer(input_text));

					char reply[1024] = {};
					size_t length = socket.read_some(boost::asio::buffer(reply));
					std::cout << std::string(reply, length) << std::endl;
				}
			}

			else if (vec[0] == "exit") {
				if (vec.size() != 1) {
					_shellManager.PrintInvalidCommand();
				}
				return 0;
			}

			else if (vec[0] == "help") {
				if (vec.size() != 1) {
					_shellManager.PrintInvalidCommand();
				}
				else {
					_shellManager.PrintInformation();
				}
			}

			else if (vec[0] == "fullwrite") {
				if (vec.size() != 2) {
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateHexNumber(vec[1])) {
					_shellManager.PrintError();
				}
				else {
					for (int i = 0; i < 100; i++) {
						std::string request = "write " + std::to_string(i) + " " + vec[1];
						boost::asio::write(socket, boost::asio::buffer(request));

						char reply[1024] = {};
						size_t length = socket.read_some(boost::asio::buffer(reply));
						std::cout << std::string(reply, length) << std::endl;
					}
				}
			}

			else if (vec[0] == "fullread") {
				if (vec.size() != 1) {
					_shellManager.PrintInvalidCommand();
					continue;
				}

				for (int i = 0; i < 100; i++) {
					std::string request = "read " + std::to_string(i);
					boost::asio::write(socket, boost::asio::buffer(request));

					char reply[1024] = {};
					size_t length = socket.read_some(boost::asio::buffer(reply));
					std::cout << "LBA " << i << " : " << std::string(reply, length) << "\n";
				}
			}

			else if (vec[0] == "test") {
				if (vec.size() != 2) {
					_shellManager.PrintInvalidCommand();
				}
				else {
					_shellManager.PrintSuccess();
				}
			}

			else {
				_shellManager.PrintInvalidCommand();
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
