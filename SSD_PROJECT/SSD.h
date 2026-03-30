#pragma once
#include <iostream>
#include "WriteRequest.h"
class SSD {
	std::string WriteText(WriteRequest req);
	std::string ReadText(int req);
};