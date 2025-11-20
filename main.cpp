#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

struct Com {
	int fd[2];
	std::string inputPath;
	std::string outputPath;
	std::string buffer;
};
namespace CompressorUtils {
	static const unsigned char SYM[] =
		{'@','#','$','%','^','&','*','(',')','_','+','=', '{','}','[',']','<','>','/','\\','|','~','`',':',';','?','!','\"','\'','0','1','2','3','4','5','6','7','8','9'};
	static const char* WORD[] =
		{"de","que","para","com","nao","por","em","uma","quando","muito","o","as",
		"ao","mais","como","se","e","foi","tem","tambem","programa","processos","threads","arquivo",
		"arquivos","texto","dicionario","palavras","compressao","exemplo","objetivo","leitura",
		"gravar","relatorio","entrada","saida","aluno","testes","atividade"};
	static const int N = sizeof(SYM)/sizeof(SYM[0]);

	static int word_to_sym(const char *w){               // palavra → símbolo
		for (int i=0;i<N;i++) if (!strcmp(WORD[i], w)) return SYM[i];
		return -1;
	}
	static const char* sym_to_word(int c){               // símbolo → palavra
		for (int i=0;i<N;i++) if (SYM[i]==(unsigned char)c) return WORD[i];
		return NULL;
	}
};

namespace CompressTask {
	Com com;
	void run() { 
		std::cout << std::endl<<"Teste aqui 2"<<std::endl;

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
	static void decompress(){
		close(com.fd[0]);
		FILE *in = fopen(com.inputPath.c_str(), "rb");
		FILE *out = fopen(com.outputPath.c_str(), "wb");
    	auto decompressStarted = std::chrono::high_resolution_clock::now();
    	int c;
    	int inFileLen = 0;
    	int outFileLen = 0;
    	
    	while ((c=fgetc(in))!=EOF){                       // lê byte do arquivo
    		const char* w = CompressorUtils::sym_to_word(c);                 // tenta achar palavra correspondente
    		if (w)
    		{
    		    fputs(w,out);
    		    outFileLen += strlen(w);
    		}
    		else {
    		    fputc(c,out);         // escreve palavra ou símbolo original
    		    outFileLen++;
    		}
    		inFileLen++;
    	}
	    
	    auto decompressFinished = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = (decompressFinished - decompressStarted);
		char buffer[200];
		sprintf(buffer, "Tempo para a descompressão: %.6lf\n", duration.count());
		std::cout << "Writed on "<<com.fd[1]<<std::endl;
		write(com.fd[1], buffer, sizeof(buffer));
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
		ctx.mode = args[1].compare("-c")==0 ? COMPRESS : 
						(args[1].compare("-d")==0?DECOMPRESS:NONE);
		ctx.compressCom.inputPath = ctx.decompressCom.inputPath = args[2];
		ctx.compressCom.outputPath = ctx.decompressCom.outputPath = args[3];
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

int main(int argc, const char *argv[]){
	if (argc != 4) {
		printf("uso: %s -c|-d entrada.txt saida.txt\n", argv[0]);
		return 1;
	}
	std::vector<std::string> args;
	for(int i=0;i<argc;i++){
		args.push_back(argv[i]);
	}

	std::cout << "Parametros: ";
	for(std::string w: args){
		std::cout <<"|" <<w << "|";
	}
	MonitorTask::main(args);
	return 0;
}