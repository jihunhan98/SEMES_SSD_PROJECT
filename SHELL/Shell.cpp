#include "ShellManager.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace fs = std::filesystem;

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

int ConvNumber(const std::string& fileName)
{
	try {
		size_t pos = fileName.find('_');
		if (pos != std::string::npos) {
			return std::stoi(fileName.substr(0, pos));
		}
	}
	catch (...) {
		return 0;
	}
	return 0;
}

bool isValidFormat(const std::string& name)
{
	size_t pos = name.find('_');
	if (pos == std::string::npos || pos == 0) return false;

	for (size_t i = 0; i < pos; ++i) {
		if (!std::isdigit(static_cast<unsigned char>(name[i]))) {
			return false;
		}
	}
	return true;
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

	std::ofstream fout("output.txt", std::ios::out | std::ios::app);

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
				return true;
			}

			if (vec[0] == "write") {
				if (vec.size() != 3) {
					commandResult = "INVALID COMMAND";
				}
				else if (!_shellManager.ValidateLba(vec[1]) || !_shellManager.ValidateHexNumber(vec[2])) {
					commandResult = "ERROR";
				}
				else {
					commandResult = sendRequestAndReceive(input_text);
				}
			}

			else if (vec[0] == "read") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
				}
				else if (!_shellManager.ValidateLba(vec[1])) {
					commandResult = "ERROR";
				}
				else {
					commandResult = sendRequestAndReceive(input_text);
				}
			}

			else if (vec[0] == "exit") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";

					return true;
				}
				return false;
			}

			else if (vec[0] == "help") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
				}
				else {
					commandResult =
						"help\n"
						"write [LBA] [VALUE]\n"
						"read [LBA]\n"
						"fullwrite [VALUE]\n"
						"fullread\n"
						"test [FILE]\n"
						"exit";
				}
			}

			else if (vec[0] == "fullwrite") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
				}
				else if (!_shellManager.ValidateHexNumber(vec[1])) {
					commandResult = "ERROR";
				}
				else {
					for (int i = 0; i < 100; i++) {
						std::string request = "write " + std::to_string(i) + " " + vec[1];
						commandResult = sendRequestAndReceive(request);
					}
				}
			}

			else if (vec[0] == "fullread") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
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
				}
			}

			else if (vec[0] == "test") {
				if (vec.size() != 2) {
					commandResult = "INVALID COMMAND";
				}
				else {
					std::string inputName = vec[1];
					std::string testFileName;

					for (auto& file : fs::directory_iterator(".")) {
						if (file.path().extension() != ".txt")
							continue;
						std::string fileName = file.path().stem().string();
						if (fileName.find(inputName) == 0) {
							testFileName = fileName;
							break;
						}
						else if (fileName == inputName)
						{
							testFileName = fileName;
							break;
						}
					}
					std::ifstream scriptFile(testFileName + ".txt");
					if (!scriptFile.is_open()) {
						commandResult = "ERROR";
					}
					else {
						bool isPass = true;
						std::string debugMessage;
						std::string scriptLine;
						while (std::getline(scriptFile, scriptLine)) {
							if (scriptLine.empty()) continue;
							std::vector<std::string> scriptTokens = split(scriptLine, ' ');
							if (scriptTokens.size() < 2) {
								isPass = false;
								if (debugMessage.empty()) {
									debugMessage = "INVALID SCRIPT => " + scriptLine;
								}
								continue;
							}

							std::string expectedResult;
							size_t commandTokenEnd = scriptTokens.size() - 1;
							if (scriptTokens.size() >= 2 &&
								scriptTokens[scriptTokens.size() - 2] == "INVALID" &&
								scriptTokens.back() == "COMMAND") {
								expectedResult = "INVALID COMMAND";
								commandTokenEnd = scriptTokens.size() - 2;
							}
							else {
								expectedResult = scriptTokens.back();
							}

							std::string commandInput = joinTokens(scriptTokens, 0, commandTokenEnd);
							std::string actualResult;

							if (!processCommand(commandInput, actualResult, false)) {
								return false;
							}

							if (actualResult != expectedResult) {
								isPass = false;
								//if (debugMessage.empty()) {
								//	debugMessage =
								//		"expected => " + expectedResult +
								//		"\nactual => " + actualResult;
								//}
							}
						}

						commandResult = isPass
							? "[PASS] " + testFileName
							: "[FAIL] " + testFileName;
						if (!isPass && !debugMessage.empty()) {
							fout << command << std::endl;
							commandResult += "\n" + debugMessage;
						}
					}
				}
			}

			else if (vec[0] == "testall") {
				if (vec.size() != 1) {
					commandResult = "INVALID COMMAND";
				}
				else {
					std::vector<std::string> testFiles;
					for (const auto& file : fs::directory_iterator(".")) {
						if (file.path().extension() != ".txt") continue;

						std::string fileName = file.path().stem().string();
						if (isValidFormat(fileName)) {
							testFiles.push_back(fileName);
						}
					}

					std::sort(testFiles.begin(), testFiles.end(), [](const std::string& a, const std::string& b) {
						return ConvNumber(a) < ConvNumber(b);
						});

					std::ofstream outputFile("output.txt");
					if (!outputFile.is_open()) {
						commandResult = "ERROR";
					}
					else {
						commandResult.clear();
						for (const auto& testFileName : testFiles) {
							std::string singleResult;
							if (!processCommand("test " + testFileName, singleResult, false)) {
								return false;
							}

							if (!commandResult.empty()) {
								commandResult += "\n";
							}
							commandResult += singleResult;
							//outputFile << singleResult << std::endl;

							if (singleResult.rfind("[FAIL] ", 0) == 0 || singleResult == "ERROR") {
								break;
							}
						}
					}
				}
			}

			else {
				commandResult = "INVALID COMMAND";
			}

			if (printResult) {
				std::cout << commandResult << std::endl;
				if (!commandResult.empty())
				{
					fout << command << std::endl;
					fout << commandResult << std::endl;
				}
			}

			return true;
			};

		while (true)
		{
			std::string input_text;
			std::cout << "shell> ";
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
