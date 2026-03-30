#pragma once
#include <iostream>
#include <vector>
class ShellManager {
private:
	std::string* text;

public:
	void WriteText(int lba, const std::string& value);
	std::string& ReadText(int lba) const;
	void PrintInvalidCommand();
	void PrintInformation();

	bool ValidateLba(const std::string s);
	bool ValidateHexNumber(const std::string s);
};