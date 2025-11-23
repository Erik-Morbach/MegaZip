#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

#include "utils.h"
#include "compressor.h"
#include "decompressor.h"

struct MonitorTask {
	enum RunMode {
		COMPRESS, DECOMPRESS, NONE
	};
	struct Ctx {
		IPCData compressCom;
		IPCData decompressCom;
		bool running;
		RunMode mode;
		int idx;
	};

	Ctx ctx;
	void main(std::vector<std::string> args);
	void setup(std::vector<std::string> args);
	void update();
	void finish();
	void updateScreen();
	void getStatusFromProcess();
	void readAllFds();
};