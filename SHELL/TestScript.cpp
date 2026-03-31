#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;

// 파일명에서 앞부분의 숫자만 추출하는 함수
int ConvNumber(const std::string& fileName) {
	try {
		// 첫 번째 '_' 앞의 문자열을 숫자로 변환
		size_t pos = fileName.find('_');
		if (pos != std::string::npos) {
			return std::stoi(fileName.substr(0, pos));
		}
	}
	catch (...) {
		return 0; // 숫자가 없을 경우 예외 처리
	}
	return 0;
}

bool isValidFormat(const std::string& name) {
	size_t pos = name.find('_');

	// 1. '_'가 없거나 맨 앞에 있는 경우 제외
	if (pos == std::string::npos || pos == 0) return false;

	// 2. '_' 앞부분이 모두 숫자인지 확인
	for (size_t i = 0; i < pos; ++i) {
		if (!std::isdigit(name[i])) {
			return false;
		}
	}
	return true;
}
bool readScript(std::string& fileName)
{
	std::string Path = fileName + ".txt";
	std::ifstream file(Path);

	if (!file.is_open()) {
		std::cerr << "파일을 열 수 없습니다: " << Path << std::endl;
		return false;
	}

	std::string buf;
	while (std::getline(file, buf)) {
		std::cout << buf << std::endl;
	}
	file.close();
	return true;
}
int main() {
	std::vector<std::string> fileList;
	std::string txtSuffix = ".txt";

	//test scritp (txt) 파일 모아서 정리
	for (auto& file : fs::directory_iterator(".")) {
		if (file.path().extension() == ".txt") {
			std::string fileName = file.path().stem().string();
			if (isValidFormat(fileName)) {
				fileList.push_back(fileName);
			}
		}
	}
	std::sort(fileList.begin(), fileList.end(), [](const std::string& a, const std::string& b) {
		return ConvNumber(a) < ConvNumber(b);
		});

	std::string inputFileName;
	std::cin >> inputFileName;

	bool fileExist = false;

	for (auto& name : fileList) {
		std::cout << name << '\n';
		if (inputFileName == name)
		{
			fileExist = true;
			break;
		}
	}

	if (fileExist && readScript(inputFileName))
		std::cout << "있음";
	else
		std::cout << "없음";


	return 0;
}

