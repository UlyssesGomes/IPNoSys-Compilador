#ifndef _IOPACK_
#define _IOPACK_

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;
using std::string;

class IOPack
{
 private:
	ifstream * fin;					//objeto para entrada de dados
	ofstream * fout;				//objeto para saída de dados
	char name[50];					//nome do arquivo em C e PDL
	string code;					//código completo em PDL
	bool flagFilesRead;				//indica se há arquivos abertos para leitura
	bool flagFilesWrite;			//indica se há arquivos abertos para escriva
	
	void closeReadFile();			//fecha o arquivo de leitura
	void closeWriteFile();			//fecha o arquivo de escrita
 public:
	int LINE;

 public:
	//IOPack(char n[]);				//em desuso
	IOPack();
	~IOPack();
	
	void openRead(char n[]);		//abre o aarquivo para leitura
	void openWrite(char n[]);		//abre o arquivo para escrita
	void readLn(char * line);		//lê uma linha do arquivo aberto para leitura
	void writeCode(string c);		//escreve no arquivo aberto para escrita

	void fazerCopia(char * fonte, char * copia);	//UTILIZADO APENAS NOS TESTES
};

#endif