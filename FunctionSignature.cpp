#include "FunctionSignature.h"

FunctionSignature::FunctionSignature()
{
	bool (*cmp)(signature *, signature *);
	cmp = FunctionSignature::lessThan;
	signatures = new set<signature *, bool(*)(signature *, signature *)>(cmp);
	
}

FunctionSignature::~FunctionSignature()
{
	for(set<signature *>::iterator it = signatures->begin(); it != signatures->end(); it++)
	{
		delete (*it)->name;
		delete (*it)->param;
	}
	
	signatures->clear();
	
	delete signatures;
}

bool FunctionSignature::lessThan(signature * a, signature * b)
{
	int tam = strlen(a->name);
	if(tam > strlen(b->name))
		tam = strlen(b->name);
	
	for(int u = 0; u < tam; u++)
	{
		if(a->name[u] < b->name[u])
			return true;
		else if(a->name[u] > b->name[u])
			return false;
	}

	if(strlen(a->name) < strlen(b->name))
		return true;

	return false;
}

void FunctionSignature::add(signature * s)
{
	signatures->insert(s);
}

signature * FunctionSignature::find(char * c)
{
	for(set<signature*>::iterator it = signatures->begin(); it != signatures->end(); it++)
	{
		if(strcmp((*it)->name, c))
			return (*it);
	}

	return NULL;
}
