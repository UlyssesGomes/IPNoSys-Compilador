#ifndef _TRANSLATOR_
#define _TRANSLATOR_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "IOPack.h"
#include "VariableCell.h"
#include "Preprocessor.h"
#include "FunctionSignature.h"
#include <string>
#include <sstream>
#include <set>

using std::set;
using namespace std;

struct paramName{
	stringstream param;
	stringstream name;
};

class Translator
{
 private:
	char fontName[50];														//nome do arquivo fonte C
	char pathName[300];														//caminho dos arquivos
	stringstream mainCode;													//código em PDL dos pacotes
	stringstream varCode;													//código em PDL das variáveis
	int nTabs;																//quantidade de tabs para identar
	int nCopies;															//contador para identificar as cópias
	IOPack * io;															//leitura e escrita em arquivos
	set < VariableCell*, bool(*)(VariableCell*, VariableCell *)> copies;	//cópias de variáveis no pacote
	bool(*cmp) (VariableCell *, VariableCell *);							//ponteiro para uma função
	FunctionSignature * funcSign;											//objeto para manipulação de assinaturas de funções
	
 private:
	void shift(char * u);						//shift pra esquerda pra eliminar o 1º char
	void addTab();								//faz a identação nos comandos
	void clearCopies();							//limpa a lista de cópias
	bool isNumber(char * string);				//verifica se a string é composta apenas por digitos
	void instructionTranslation(char * line);	//faz a tradução apenas de instruções (apenas uma por vez) sem se importar com blocos 

 public:
  	Translator(const char name[], const char path[]);
	~Translator();

	void createIR();					//abre o arquivo C e transforma no LLVM-IR
	void translation();					//converte o LLVM-IR em PDL
	void saveToPDL();					//salva o programa compilado em PDL
};

inline void Translator::shift(char * u)
{
	int cont = 0;
	while (u[cont] != '\0')
	{
		u[cont] = u[cont + 1];
		cont++;
	}
}

inline void Translator::addTab()
{
	for (int u = 0; u < nTabs; u++)
		mainCode << "\t";
}

inline void Translator::clearCopies()
{
	if (copies.size() > 0)
	{
		list<VariableCell *> removelist;
		for (set<VariableCell*>::iterator u = copies.begin(); u != copies.end(); u++)
			removelist.push_back(*u);

		for (list<VariableCell *>::iterator u = removelist.begin(); u != removelist.end(); u++)
			copies.erase(*u);

		removelist.clear();
	}
	copies.clear();
}

inline bool Translator::isNumber(char * string)		//verifica se a string é composta apenas por digitos
{
	int tam = strlen(string);
	for (int u = 0; u < tam; u++)
	{
		if (!isdigit(string[0]))
			return false;
	}
	return true;
}

inline void Translator::saveToPDL()
{
	char nome [300];
	strcpy(nome, "codigos/");
	strcat(nome, fontName);
	strcat(nome, ".asm");
	io->openWrite(nome);
	strcpy(nome, "PROGRAM ");
	strcat(nome, fontName);
	io->writeCode(nome);
	io->writeCode("\nDATA\n");
	io->writeCode(varCode.str());
	io->writeCode(mainCode.str());
	io->writeCode("END_PROGRAM");
}

#endif
