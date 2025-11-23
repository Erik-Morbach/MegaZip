#pragma once
#include <unistd.h>
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <ios>
#include <iostream>
#include <limits>
#include <mutex>
#include <ostream>
#include <queue>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils.h"

struct CompressorTask {
	IPCData com;
	std::condition_variable read_cv;
	std::mutex read_mutex;
	bool done_reading = false;

	std::condition_variable compress_cv;
	std::mutex compress_mutex;
	bool done_compressing = false;

	std::queue<std::string> in_queue;
	std::queue<char> out_queue;

	uint64_t bytes_read =0, bytes_written =0;

	void run();
	void read();
	void compress();
	void write();
};