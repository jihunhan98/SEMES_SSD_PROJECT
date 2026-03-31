#include "ShellManager.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
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

std::string joinTokens(const std::vector<std::string>& tokens, size_t start, size_t end)
{
	std::string result;
	for (size_t i = start; i < end; ++i) {
		if (i > start) {
			result += " ";
		}
		result += tokens[i];
	}
	return result;
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
		auto sendRequestAndReceive = [&](const std::string& request) -> std::string {
			boost::asio::write(socket, boost::asio::buffer(request));

			char reply[1024] = {};
			size_t length = socket.read_some(boost::asio::buffer(reply));
			return std::string(reply, length);
		};
		std::function<bool(const std::string&, std::string&, bool)> processCommand;

		processCommand = [&](const std::string& command, std::string& commandResult, bool printResult) -> bool {
			std::string input_text = command;
			std::vector<std::string> vec = split(input_text, ' ');

			if (vec.empty() || vec[0].empty()) {
				commandResult = "INVALID COMMAND";
				_shellManager.PrintInvalidCommand();
				return true;
			}

			if (vec[0] == "write") {
				if (vec.size() != 3) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateLba(vec[1]) || !_shellManager.ValidateHexNumber(vec[2])) {
					commandResult = "ERROR";
					_shellManager.PrintError();
				}
				else {
					commandResult = sendRequestAndReceive(input_text);
				}
			}

			else if (vec[0] == "read") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateLba(vec[1])) {
					commandResult = "ERROR";
					_shellManager.PrintError();
				}
				else {
					commandResult = sendRequestAndReceive(input_text);
					if (printResult) {
						std::cout << commandResult << std::endl;
					}
				}
			}

			else if (vec[0] == "exit") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
					return true;
				}
				return false;
			}

			else if (vec[0] == "help") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
				}
				else {
					commandResult = "HELP";
					_shellManager.PrintInformation();
				}
			}

			else if (vec[0] == "fullwrite") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
				}
				else if (!_shellManager.ValidateHexNumber(vec[1])) {
					commandResult = "ERROR";
					_shellManager.PrintError();
				}
				else {
					for (int i = 0; i < 100; i++) {
						std::string request = "write " + std::to_string(i) + " " + vec[1];
						commandResult = sendRequestAndReceive(request);
					}
					if (printResult) {
						std::cout << commandResult << std::endl;
					}
				}
			}

			else if (vec[0] == "fullread") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
					return true;
				}

				commandResult.clear();
				for (int i = 0; i < 100; i++) {
					std::string request = "read " + std::to_string(i);
					std::string response = sendRequestAndReceive(request);
					std::string lineResult = "LBA " + std::to_string(i) + " : " + response;
					if (!commandResult.empty()) {
						commandResult += "\n";
					}
					commandResult += lineResult;
					if (printResult) {
						std::cout << lineResult << "\n";
					}
				}
			}

			else if (vec[0] == "test") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
					_shellManager.PrintInvalidCommand();
				}
				else {
					std::ifstream scriptFile(vec[1] + ".txt");
					if (!scriptFile.is_open()) {
						commandResult = "ERROR";
						_shellManager.PrintError();
					}
					else {
						std::string scriptLine;
						while (std::getline(scriptFile, scriptLine)) {
							if (scriptLine.empty()) continue;
							std::vector<std::string> scriptTokens = split(scriptLine, ' ');
							if (scriptTokens.size() < 2) {
								_shellManager.PrintInvalidCommand();
								continue;
							}

							std::string expectedResult = scriptTokens.back();
							std::string commandInput = joinTokens(scriptTokens, 0, scriptTokens.size() - 1);
							std::string actualResult;


							if (!processCommand(commandInput, actualResult, false)) {
								return false;
							}

						}
					}
				}
			}

			else {
				commandResult = "INVALID COMMAND";
				_shellManager.PrintInvalidCommand();
			}

			return true;
		};

		while (true)
		{
			std::string input_text;
			std::getline(std::cin, input_text);
			std::string commandResult;
			if (!processCommand(input_text, commandResult, true)) {
				return 0;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
