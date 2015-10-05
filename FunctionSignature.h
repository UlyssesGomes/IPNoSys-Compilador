#ifndef _FUNCTION_SIGNATURE_
#define _FUNCTION_SIGNATURE_

#include <list>
#include <string>
#include <set>
#include <cstring>

#include <iostream>
using namespace std;

using std::string;

using std::list;
using std::set;

struct signature
{
	char * name;				//nome da função
	list<string> * param;		//lista de parâmetros em ordem
};

class FunctionSignature
{
 private:
 	set<signature *, bool(*)(signature *, signature *)> * signatures;
 	
 public:
 	FunctionSignature();
 	~FunctionSignature();
 	
 	void add(signature * s);								//adiciona uma assinatura a árvore
 	signature * find(char * c);								//encontra uma assinatura na árvore
 	static bool lessThan(signature * a, signature * b);		//comparação entre duas assinaturas
};

#endif
