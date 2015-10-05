#include "VariableCell.h"

VariableCell::VariableCell(string k, list<string> *c)
{
	key = k;
	copies = c;
}

VariableCell::VariableCell(string k)
{
	key = k;
	copies = NULL;
}

VariableCell::~VariableCell()
{
	if(copies != NULL)
	{
		copies->clear();
		delete copies;
	}
}

bool VariableCell::lessThan(VariableCell * a, VariableCell * b)
{
	int tam = a->key.size();
	if(tam > b->key.size())
		tam = b->key.size();
	
	for(int u = 0; u < tam; u++)
	{
		if(a->key[u] < b->key[u])
			return true;
		else if(a->key[u] > b->key[u])
			return false;
	}

	if(a->key.size() < b->key.size())
		return true;

	return false;
}

int VariableCell::nCopies()
{
	return copies->size();
}

string VariableCell::getCopy()
{
	string x = copies->front();
	copies->pop_front();
	cout << "--Copia retirada: " << x << "--\n";
	return x;
}