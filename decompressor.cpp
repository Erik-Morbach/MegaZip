#include "decompressor.h"

void DecompressorTask::decompress() {
	std::ifstream in_file(com.inputPath.data(), std::ios_base::binary);
	FileUtils::check_file_is_open(in_file, com.inputPath);

	std::ofstream out_file(com.outputPath.data(), std::ios_base::binary);
	FileUtils::check_file_is_open(out_file, com.outputPath);

	uint16_t next_code = 0x8000;
	std::vector<std::string> code_word;
	std::unordered_map<std::string, uint16_t> word_code;
	
	std::string word;

	for (char c; in_file.get(c); ++bytes_read) {
		if (c & 0x80) {
			if (!word.empty()) {
				std::cerr << "Error decompressing file: found code indicator right after word" << std::endl;
				std::exit(1);
			}
			
			uint16_t code = static_cast<unsigned char>(c) << 8;
			if (!in_file.get(c)) {
				std::cerr << "Error decompressing file: read 1 byte for code, expected 2" << std::endl;
				std::exit(1);
			}

			code |= c & 0xFF;
			if (code >= next_code) {
				std::cerr << "Error decompressing file: invalid code" << std::endl;
				std::exit(1);
			}

			++bytes_read;
			out_file << code_word[code & 0x7FFF];
		} else {
			if (std::isalpha(c)) {
				word.push_back(c);
				continue;
			}

			const auto it = word_code.find(word);
			if (it != word_code.end()) {
				out_file << it->second;
			} else {
				if (word.size() > 2 && next_code != std::numeric_limits<uint16_t>::max()) {
					word_code[word] = next_code++;
					code_word.push_back(word);
				}
				
				out_file << word;
				word.clear();
			}

			out_file << c;
		}
	}

	bytes_written = out_file.tellp();
	std::stringstream ss;
	ss << "Input size:      " << bytes_read << " bytes\n";
	ss << "Compressed size: " << bytes_written << " bytes\n";
	ss << "Ratio:           " << ((bytes_written) / static_cast<double>(bytes_read)) << "x\n";
	std::string s = ss.str();
	write(com.fd[1], s.c_str(), s.size());
}