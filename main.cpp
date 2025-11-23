#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>



#include "monitor.h"


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
	MonitorTask task;
	task.main(args);
	return 0;
}