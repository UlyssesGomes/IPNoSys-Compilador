#ifndef _PREPROC_
#define _PREPROC_

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <map>
#include <list>
#include "IOPack.h"

using std::map;
using std::list;
using namespace std;

class Preprocessor
{
 private:
	IOPack * io;						//objeto para leitura e escrita em arquivos
	char nameProgram[100];				//nome do programa a ser processado
	stringstream code;					//código do programa após o processamento

 private:
 	void shift(char * u)				//shift pra esquerda pra eliminar o 1º char
	{
		int cont = 0;
		while(u[cont] != '\0')
		{
			u[cont] = u[cont+1];
			cont++;
		}
	}

	static bool compare(char * a, char * b); //função para comparar se uma string é maior que outra (a < b).

 public:

 	Preprocessor(const char name[]);
 	~Preprocessor();

 	void optimize();					//faz as otimizações removendo jumps e labels desnecessários
};

#endif