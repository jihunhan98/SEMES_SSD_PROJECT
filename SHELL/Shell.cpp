#include "ShellManager.h"
#include <string>
#include <vector>
#include <sstream>


std::vector<std::string> split(std:: string& _s, char _deli)
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
	ShellManager _shellManager;

	while (true)
	{
		std::string input_text;
		std::getline(std::cin, input_text);
		std::vector<std::string> vec = split(input_text, ' ');

		if (vec[0] == "write") {
			if (vec.size() != 3 || !_shellManager.ValidateLba(vec[1]) 
				|| !_shellManager.ValidateHexNumber(vec[2])) 
			{
				_shellManager.PrintInvalidCommand();
				continue;
			}

			//1. 성공
			_shellManager.WriteText(std::stoi(vec[1]), vec[2]);

			std::cout << "write 성공\n";
		}

		else if (vec[0] == "read") {
			if (vec.size() != 2 || !_shellManager.ValidateLba(vec[1])) {
				_shellManager.PrintInvalidCommand();
				continue;
			}

			_shellManager.ReadText(std::stoi(vec[1]));
			std::cout << "read 성공\n";
		}

		else if (vec[0] == "exit") {
			//1. 명령어 형식(인자 갯수)
			if (vec.size() != 1) {
				_shellManager.PrintInvalidCommand();
			}
			return 0;
		}

		else if (vec[0] == "help") {
			//1. 명령어 형식(인자 갯수)
			if (vec.size() != 1) {
				_shellManager.PrintInvalidCommand();
				continue;
			}

			_shellManager.PrintInformation();
		}

		else if (vec[0] == "fullwrite") {
			//1. 명령어 형식(인자 갯수)
			if (vec.size() != 2 || !_shellManager.ValidateHexNumber(vec[1])) {
				_shellManager.PrintInvalidCommand();
				continue;
			}

			for (int i = 0; i < 100; i++) {
				_shellManager.WriteText(i, vec[1]);
			}

			std::cout << "SUCCESS\n";
		}

		else if (vec[0] == "fullread") {
			//1. 명령어 형식(인자 갯수)
			if (vec.size() != 1) {
				_shellManager.PrintInvalidCommand();
				continue;
			}
			for (int i = 0; i < 100; i++) {
				std::cout << "LBA " << i << " : " << _shellManager.ReadText(i) << "\n";
			}
		}

		else if (vec[0] == "test") {
			//1. 명령어 형식(인자 갯수)
			if (vec.size() != 2) {
				_shellManager.PrintInvalidCommand();
				continue;
			}

			std::cout << "test 성공\n";
		}

		else {
			_shellManager.PrintInvalidCommand();
		}
	}
}

