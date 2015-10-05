#ifndef _VARIABLE_
#define _VARIABLE_

#include <list>
#include <string>
#include <iostream>
using namespace std;

using std::list;
using std::string;

class VariableCell
{
public:
	string key;													//variável que contém copias no pacote
	list<string> * copies;										//rótulo das copias no pacote

public:
	VariableCell(string k, list<string> * c);					//construtor
	VariableCell(string k);										//sobrecarga do construtor
	~VariableCell();											//destrutor

	static bool lessThan (VariableCell * a, VariableCell * b );	//compara se o obj "a" é menor que o obj "b"
	int nCopies();												//números de cópias restantes
	string getCopy();											//pega uma cópia e retira da lista
};

#endif