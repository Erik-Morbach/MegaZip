#include "compressor.h"

namespace CompressorTask {
	std::condition_variable read_cv;
	std::mutex read_mutex;
	bool done_reading = false;

	std::condition_variable compress_cv;
	std::mutex compress_mutex;
	bool done_compressing = false;

	std::queue<std::string> in_queue;
	std::queue<char> out_queue;

	uint64_t bytes_read, bytes_written;
	void read() {
		std::ifstream in_file(com.inputPath.data(), std::ios_base::binary);
		FileUtils::check_file_is_open(in_file, com.inputPath);
		
		std::string word;

		for (char c; in_file.get(c); ++bytes_read) {
			if (c & 0x80) {
				std::cerr << "Error reading file: this program only works with pure ASCII files" << std::endl;
				std::exit(1);
			}
			
			if (std::isalpha(c)) {
				word.push_back(c);
				continue;
			}

			read_mutex.lock();

		if (!word.empty()) {
				in_queue.push(word);
				word.clear();
			}

			in_queue.emplace(1, c);

			read_mutex.unlock();
			read_cv.notify_one();
		}

		read_mutex.lock();

		done_reading = true;

		read_mutex.unlock();
		read_cv.notify_one();
	}

	void compress() {
		uint16_t next_code = 0x8000;
		std::unordered_map<std::string, uint16_t> word_code;

		while (true) {
			std::unique_lock<std::mutex> lock(read_mutex);
			read_cv.wait(lock, []{ return !in_queue.empty() || done_reading; });

			if (done_reading && in_queue.empty()) break;
			
			const std::string word(std::move(in_queue.front()));
			in_queue.pop();

			lock.unlock();
			
			compress_mutex.lock();

			const auto it = word_code.find(word);
			if (it != word_code.end()) {
				const uint16_t code = it->second;
				out_queue.push(code >> 8);
				out_queue.push(code & 0xFF);
			} else {
				if (word.size() > 2 && next_code != std::numeric_limits<uint16_t>::max()) {
					word_code[word] = next_code++;
				}
				
				for (const char c : word) {
					out_queue.push(c);
				}
			}

			compress_mutex.unlock();
			compress_cv.notify_one();
		}

		compress_mutex.lock();

		done_compressing = true;

		compress_mutex.unlock();
		compress_cv.notify_one();
	}

	void write() {
		std::ofstream out_file(com.outputPath.data(), std::ios_base::binary);
		FileUtils::check_file_is_open(out_file, com.outputPath);

		while (true) {
			std::unique_lock<std::mutex> lock(compress_mutex);
			compress_cv.wait(lock, []{ return !out_queue.empty() || done_compressing; });

			if (done_compressing && out_queue.empty()) break;

			out_file.put(out_queue.front());
			out_queue.pop();
		}

		bytes_written = out_file.tellp();
	}
};