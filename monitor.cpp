#include "monitor.h"

namespace MonitorTask {
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
		ctx.mode = args[1].compare("-c")==0 ? COMPRESS : 
						(args[1].compare("-d")==0?DECOMPRESS:NONE);
		ctx.compressCom.inputPath = ctx.decompressCom.inputPath = args[2];
		ctx.compressCom.outputPath = ctx.decompressCom.outputPath = args[3];
		pipe(ctx.compressCom.fd);
		pipe(ctx.decompressCom.fd);
		close(ctx.compressCom.fd[1]);
		close(ctx.decompressCom.fd[1]);

		if(fork() == 0) {
			CompressorTask::com = ctx.compressCom;
			DecompressorTask::com = ctx.decompressCom;
			if(ctx.mode == COMPRESS)
				CompressorTask::run();
			else
				DecompressorTask::decompress();
			exit(0);
		}
	}
	void update(){
		MonitorTask::updateScreen();
		MonitorTask::getStatusFromProcess();
	}
	void updateScreen() {
		printf("Espere o processo finalizar: %c \r", "-/|\\"[(abs(ctx.idx++)/40000)%4]);
		fflush(stdout);
	}
	void getStatusFromProcess() {
		char buffer[50];
		int fd = ctx.mode == RunMode::COMPRESS ?
					ctx.compressCom.fd[0] :
					ctx.decompressCom.fd[0];
		int result = read(fd, buffer, sizeof(buffer));	

		if(result != 0)
			std::cout << "Leu "<< result << std::endl;
		ctx.running = strlen(buffer) == 0;
		std::cout << "Read on "<<fd<<std::endl;
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