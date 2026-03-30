#include "ShellManager.h"
#include <algorithm>
#include <cctype>
#include <string>
void ShellManager::WriteText(int lba, const std::string& value) {
}
std::string& ShellManager::ReadText(int lba) const {
	std::string d = "wqd";
	return d;
}
void ShellManager::PrintInvalidCommand() {
	std::cout << "INVALID COMMAND" << std::endl;
}
void ShellManager::PrintInformation() {
	std::cout << "팀 이름 : 한지훈 김보성\n";
	std::cout << "팀 원 : 한지훈 김보성\n";
	std::cout << "각 명령 사용법\n";
	std::cout << "....\n";
}


bool ShellManager::ValidateLba(const std::string s) {
	if (s.empty()) return false; // 빈 문자열 예외 처리
	if (!std::all_of(s.begin(), s.end(), ::isdigit)) return false;
	int x = std::stoi(s);

	return (x >= 0 && x <= 99);
}
bool ShellManager::ValidateHexNumber(const std::string s) {
	if (s.empty() || s.size() != 10) return false; // 빈 문자열 예외 처리
	if (s[0] != '0' || s[1] != 'x') return false;
	for (int i = 2; i < s.size(); i++) {
		if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'F')) continue;
		return false;
	}

	return true;
}

