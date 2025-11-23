#pragma once
#include <unistd.h>
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <mutex>
#include <ostream>
#include <sstream>
#include <queue>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils.h"

struct DecompressorTask {
	IPCData com;
	uint64_t bytes_read = 0, bytes_written = 0;
	void decompress();
};