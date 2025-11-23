#pragma once
#include <string>
struct IPCData {
	int fd[2];
	std::string inputPath;
	std::string outputPath;
	std::string buffer;
};

namespace FileUtils{
	template<class File>
	void check_file_is_open(const File& f, const std::string_view path) {
		if (!f.is_open()) {
			std::cerr << "Could not open file: " << path << std::endl;
			std::exit(1);
		}
	}
};