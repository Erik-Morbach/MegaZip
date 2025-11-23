#include "monitor.h"
#include <memory>

void MonitorTask::main(std::vector<std::string> args){
	this->setup(args);
	this->finish();
}
void MonitorTask::setup(std::vector<std::string> args){
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

	if(fork() == 0) {
		close(ctx.compressCom.fd[0]);
		close(ctx.decompressCom.fd[0]);
		if(ctx.mode == COMPRESS) {
			CompressorTask task;
			task.com = this->ctx.compressCom;
			task.run();
		}
		else{
			DecompressorTask task;
			task.com = this->ctx.decompressCom;
			task.decompress();
		}
		exit(0);
	}
	else{
		close(ctx.compressCom.fd[1]);
		close(ctx.decompressCom.fd[1]);
	}
}
void MonitorTask::readAllFds() {
	char buffer[1000] = {0};
	if(ctx.mode == COMPRESS){
		read(ctx.compressCom.fd[0], buffer, sizeof(buffer));
		ctx.compressCom.buffer += buffer;
	}
	else{
		read(ctx.decompressCom.fd[0], buffer, sizeof(buffer));
		ctx.decompressCom.buffer += buffer;
	}
}
void MonitorTask::finish(){
	this->readAllFds();
	std::string name = ctx.mode == COMPRESS ? "Compressor" : "Descompressor";
	auto buff = ctx.mode == COMPRESS ? ctx.compressCom.buffer :
		ctx.decompressCom.buffer;
	std::cout << "Processo " << name << " finalizou e retornou:\n" << buff << std::endl;
}