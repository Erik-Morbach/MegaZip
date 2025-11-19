/*
gcc mini_zip.c -o mz
./mz -c entrada.txt compactado.mzp     # compactar
./mz -d compactado.mzp saida.txt       # descompactar
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

static void compress(FILE *in, FILE *out){
  int c;               // caractere lido do arquivo
  int wl = 0;          // comprimento atual da palavra
  char w[256];         // buffer temporário para armazenar uma palavra
  while ((c=fgetc(in))!=EOF){                       // lê caractere
    int cl = tolower(c);
    if (isalnum(cl) || cl=='_') w[wl++]=cl;         // acumula caractere na palavra
    else{
      if (wl){ w[wl]=0; wl=0;                       // fim da palavra
        int s = word_to_sym(w);                     // tenta converter para símbolo
        if (s>=0) fputc(s,out); else fputs(w,out);  // grava símbolo ou palavra
      }
      fputc(c,out);                                 // grava separador (espaço, pontuação, etc.)
    }
  }
  if (wl){ w[wl]=0; int s=word_to_sym(w); if(s>=0) fputc(s,out); else fputs(w,out); }
}

static void decompress(FILE *in, FILE *out){
  int c;
  while ((c=fgetc(in))!=EOF){                       // lê byte do arquivo
    const char* w = sym_to_word(c);                 // tenta achar palavra correspondente
    if (w) fputs(w,out); else fputc(c,out);         // escreve palavra ou símbolo original
  }
}

int main(int argc, char **argv){
  if (argc != 4) { printf("uso: %s -c|-d entrada.txt saida.txt\n", argv[0]); return 1; }
  FILE *in = fopen(argv[2],"rb");                   // abre arquivo de entrada
  FILE *out = fopen(argv[3],"wb");                  // abre arquivo de saída
  if (argv[1][1]=='c') compress(in,out);            // modo compactar
  else decompress(in,out);                          // modo descompactar
  fclose(in); fclose(out);
  return 0;
}