#pragma once
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <fstream>
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

namespace DecompressorTask {
	IPCData com;
	void decompress();
};