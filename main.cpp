#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>

struct Com {
	int fd[2];
	std::string inputPath;
	std::string outputPath;
	std::string buffer;
};
namespace CompressorUtils {

};

namespace CompressTask {
	Com com;
	void run() { 

	}
	void read(){

	}

	void compress(){

	}

	void write(){

	}
};

namespace DecompressTask {
	Com com;
	void decompress(){

	}
};

namespace MonitorTask {
	enum RunMode {
		COMPRESS, DECOMPRESS, NONE
	};
	struct Ctx {
		Com compressCom;
		Com decompressCom;
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

	void main(std::vector<std::string> args){
		MonitorTask::setup(args);
		while(ctx.running){
			MonitorTask::update();
		}
		MonitorTask::finish();
	}
	void setup(std::vector<std::string> args){
		ctx.idx = 0;
		ctx.compressCom.buffer = "";
		ctx.decompressCom.buffer = "";
		ctx.running = true;
		ctx.mode = args[0].compare("-c") ? COMPRESS : 
						(args[0].compare("-d")?DECOMPRESS:NONE);
		ctx.compressCom.inputPath = ctx.decompressCom.inputPath = args[1];
		ctx.compressCom.outputPath = ctx.decompressCom.outputPath = args[2];
		pipe(ctx.compressCom.fd);
		pipe(ctx.decompressCom.fd);
		close(ctx.compressCom.fd[1]);
		close(ctx.decompressCom.fd[1]);

		if(fork() == 0) {
			CompressTask::com = ctx.compressCom;
			DecompressTask::com = ctx.decompressCom;
			if(ctx.mode == COMPRESS)
				CompressTask::run();
			else
				DecompressTask::decompress();
			exit(0);
		}
	}
	void update(){
		MonitorTask::updateScreen();
		MonitorTask::getStatusFromProcess();
	}
	void updateScreen() {
		printf("Espere o processo finalizar: %c \r", "-/|\\"[((ctx.idx++)/40000)%4]);
		fflush(stdout);
	}
	void getStatusFromProcess() {
		char buffer[50];
		int fd = ctx.mode == RunMode::COMPRESS ?
					ctx.compressCom.fd[0] :
					ctx.decompressCom.fd[0];
		int result = read(fd, buffer, sizeof(buffer));	

		ctx.running = strlen(buffer) == 0;
		if(!ctx.running){
			(
				ctx.mode == RunMode::COMPRESS ? 
				ctx.compressCom.buffer :
				ctx.decompressCom.buffer
			) += buffer;
		}
	}
	void readAllFds() {
		char buffer[1000];
		read(ctx.compressCom.fd[0], buffer, sizeof(buffer));
		ctx.compressCom.buffer += buffer;
		read(ctx.decompressCom.fd[0], buffer, sizeof(buffer));
		ctx.decompressCom.buffer += buffer;
	}
	void finish(){
		MonitorTask::readAllFds();
		std::string name = ctx.mode == COMPRESS ? "Compressor" : "Descompressor";
		const char *buff = ctx.mode == COMPRESS ? ctx.compressCom.buffer.data() :
			ctx.decompressCom.buffer.data();
		printf("Processo %s finalizou e retornou:\n%s", name.data(), buff);
	}
};

int main(int argc, const char *argv[]){
	if (argc != 4) {
		printf("uso: %s -c|-d entrada.txt saida.txt\n", argv[0]);
		return 1;
	}
	std::vector<std::string> args;
	for(int i=0;i<argc;i++){
		args.push_back(argv[i]);
	}
	MonitorTask::main(args);
	return 0;
}