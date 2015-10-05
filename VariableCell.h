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
	string key;													//vari�vel que cont�m copias no pacote
	list<string> * copies;										//r�tulo das copias no pacote

public:
	VariableCell(string k, list<string> * c);					//construtor
	VariableCell(string k);										//sobrecarga do construtor
	~VariableCell();											//destrutor

	static bool lessThan (VariableCell * a, VariableCell * b );	//compara se o obj "a" � menor que o obj "b"
	int nCopies();												//n�meros de c�pias restantes
	string getCopy();											//pega uma c�pia e retira da lista
};

#endif