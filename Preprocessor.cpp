#include "Preprocessor.h"

Preprocessor::Preprocessor(const char name[])
{
	strcpy(nameProgram, name);
	io = new IOPack();

	//io->fazerCopia("loop - Copia.ll", nameProgram);		//utilizado apenas na fase de testes.
}

Preprocessor::~Preprocessor()
{
	delete io;
}

bool Preprocessor::compare(char * a, char * b)
{
	int tam = strlen(a);
	if(tam > strlen(b))
		tam = strlen(b);
	
	for(int u = 0; u < tam; u++)
	{
		if(a[u] < b[u])
			return true;
		else if(a[u] > b[u])
			return false;
	}

	if(strlen(a) < strlen(b))
		return true;

	return false;
}

void Preprocessor::optimize()
{
	char * linha = new char[io->LINE];			//armazena a linha lida do arquivo nessa variável
	char * linhaCopia = new char[io->LINE];		//cópia da variável linha
	char * tok;									//token para pegar comandos
	char * aux;									//token de auxilio
	bool flag = false;							//flag indicando se a linha seguinte foi consumida

	//--Etapa Zero -> Remoção de linhas vazias
	io->openRead(nameProgram);
	do
	{
		io->readLn(linha);
		if(strlen(linha) > 0)
			code << linha << endl;

	}while(strcmp(linha, "!end") != 0);
	io->openWrite(nameProgram);
	io->writeCode(code.str());
	code.str("");

	//--Primeira etapa	
	io->openRead(nameProgram);					//abre o arquivo para leitura

	do{
		io->readLn(linha);
		strcpy(linhaCopia, linha);
		aux = strtok(linha, " ");


		if(strcmp(aux, "br") == 0)
		{
			tok = strtok(NULL, " ");
			if(strcmp(tok, "label") == 0)
			{
				tok = strtok(NULL, " ");
				shift(tok);
				char tokenaux[20];
				strcpy(tokenaux, tok);

				io->readLn(linha);
				char * linhaAux = new char[io->LINE];
				strcpy(linhaAux, linha);
				aux = strtok(linha, " ");
				if(strcmp(aux, ";") == 0)
				{
					char * tok2 = strtok(NULL, " ");
					if(tok2[0] == '<' && tok2[1] == 'l')		//identifica a declaração de um label
					{
						tok2 = strtok(tok2, ":");
						tok2 = strtok(NULL, ":");

						if(strcmp(tokenaux, tok2) == 0)
						{
							//detectado um jump para linha imediatamente seguinte
							strcpy(linhaCopia, linhaAux);
						}
						else
						{
							strcpy(linha, linhaAux);
							flag = true;
						}
					}
				}
			}
		}

		code << linhaCopia << "\n";
		if(flag)
		{
			code << linha << "\n";
			flag = false;
		}

	}while(strcmp(linha, "!end") != 0);
	io->openWrite(nameProgram);
	io->writeCode(code.str());

	cout << "Primeira etapa finalizada.\n";

	//--Segunda etapa

	//Segunda etapa parte 1: eliminar os jamps
	bool (*cmp) (char*, char*);
	cmp = compare;
	map<char*, int, bool(*)(char*, char*)> * labels = new map<char*, int, bool(*)(char*, char*)>(cmp);		//map que contém o label e a quantidade de vezes que é referênciado no código
	io->openRead(nameProgram);
	code.str("");
	do{
		io->readLn(linha);
		aux = strtok(linha, " ");

		if(strcmp(aux, "br") == 0)		//br i1 %4, label %5, label %13
		{
			tok = strtok(NULL, " ");
			if(strcmp(tok, "i1") == 0)
			{
				tok = strtok(NULL, " ");
				tok = strtok(NULL, " ");
				tok = strtok(NULL, " ");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				shift(tok);
				int x = strlen(tok);
				tok[x-1] = '\0';
				strcat(nameLabel, tok);
				map<char*, int>::iterator it = labels->find(nameLabel);
				if(it == labels->end())
				{
					char * labelAux = new char[20];
					strcpy(labelAux, nameLabel);
					labels->insert(pair<char*, int>(labelAux, 1));
				} 
				else
				{
					it->second++;
				}

				tok = strtok(NULL, " ");
				tok = strtok(NULL, " ");
				strcpy(nameLabel, "label");
				shift(tok);
				strcat(nameLabel, tok);
				it = labels->find(nameLabel);
				if(it == labels->end())
				{
					char * labelAux = new char[20];
					strcpy(labelAux, nameLabel);
					labels->insert(pair<char*, int>(labelAux, 1));
				} 
				else
				{
					it->second++;
				}

			}
			else if(strcmp(tok, "label") == 0) //br label %2
			{
				tok = strtok(NULL, " ");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				shift(tok);
				strcat(nameLabel, tok);
				map<char*, int>::iterator it = labels->find(nameLabel);
				if(it == labels->end())
				{
					char * labelAux = new char[20];
					strcpy(labelAux, nameLabel);
					labels->insert(pair<char*, int>(labelAux, 1));
				}
				else
				{
					it->second++;
				} 

			}
		}
		else if (strcmp(aux, ";") == 0)		//; <label>:10 
		{
			char * tok2 = strtok(NULL, " ");
			if(tok2[0] == '<' && tok2[1] == 'l')		//identifica a declaração de um label
			{
				tok2 = strtok(tok2, ":");
				tok2 = strtok(NULL, ":");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				strcat(nameLabel, tok2);
				map<char*, int>::iterator it = labels->find(nameLabel);
				if(it == labels->end())
				{
					char * labelAux = new char[20];
					strcpy(labelAux, nameLabel);
					labels->insert(pair<char*, int>(labelAux, 0));
				} 				
			}
		}

	}while(strcmp(linha, "!end") != 0);

	//remover da map os labels que tem referências
	list<char*> removeList;
	map<char*, int>::iterator it;
	for(it = labels->begin(); it != labels->end(); it++)
	{
		if(it->second > 0)
		{
			removeList.push_back(it->first);
		}
	}

	list<char*>::iterator m;
	for (m = removeList.begin(); m != removeList.end(); m++)
	{
		labels->erase(*m);
		char * lab = *m;
		cout << "removendo => " << lab << endl;
		delete[] * m;
	}
	removeList.clear();

	cout << "Segunda etapa parte 1 finalizada \n";
	//Segunda etapa parte 2: remove do código os items da map (os labels que não são referênciados)
	io->openRead(nameProgram);
	code.str("");
	do{
		io->readLn(linha);
		strcpy(linhaCopia, linha);
		aux = strtok(linha, " ");

		if (strcmp(aux, ";") == 0)		//; <label>:10 
		{
			char * tok2 = strtok(NULL, " ");
			if(tok2[0] == '<' && tok2[1] == 'l')		//identifica a declaração de um label
			{
				tok2 = strtok(tok2, ":");
				tok2 = strtok(NULL, ":");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				strcat(nameLabel, tok2);
				map<char*, int>::iterator it = labels->find(nameLabel);
				if(it == labels->end())
				{
					code << linhaCopia << "\n";
				} 				
			}
			else
			{
				code << linhaCopia << "\n";
			}
		}
		else
		{
			code << linhaCopia << "\n";
		}

	}while(strcmp(linha, "!end") != 0);
	io->openWrite(nameProgram);
	io->writeCode(code.str());

	map<char*, int>::iterator ite;
	for(ite = labels->begin(); ite != labels->end(); ite++)
		delete ite->first;
	delete labels;

	cout << "Segunda etapa parte 2 finalizada\n";

	//--Terceira etapa

	//Terceira etapa - parte 1: pegar as posições dos labels
	cmp = compare;
	map<char*, int, bool(*)(char*, char*)> mapLabelPos(cmp);		//map que contém o label e sua posição no código

	io->openRead(nameProgram);
	int posLinha = 0;
	do{
		io->readLn(linha);
		posLinha++;
		aux = strtok(linha, " ");

		if (strcmp(aux, ";") == 0)		//; <label>:10 
		{
			char * tok2 = strtok(NULL, " ");
			if(tok2[0] == '<' && tok2[1] == 'l')		//identifica a declaração de um label
			{
				tok2 = strtok(tok2, ":");
				tok2 = strtok(NULL, ":");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				strcat(nameLabel, tok2);
				map<char*, int>::iterator it = mapLabelPos.find(nameLabel);
				if(it == mapLabelPos.end())
				{
					char * nn = new char[15];
					strcpy(nn, nameLabel);
					mapLabelPos.insert(pair<char*, int>(nn, posLinha));
				}				
			}
		}

	}while(strcmp(linha, "!end") != 0);

	cout << "\nTerceira etapa parte 1 finalizada\n";
	//Terceiraa etapa - parte 2: criar uma lista de bloco onde tem a indicação da linha onde começa o bloco e a linha onde termina
	map<int, int> blocos;		//a chave indica a posição inicial do bloco e o valor indica a posição final do bloco

	io->openRead(nameProgram);
	posLinha = 0;
	do{
		io->readLn(linha);
		posLinha++;
		aux = strtok(linha, " ");

		if(strcmp(aux, "br") == 0)
		{
			tok = strtok(NULL, " ");
			if(strcmp(tok, "label") == 0) //br label %2
			{
				tok = strtok(NULL, " ");
				char nameLabel[15];
				strcpy(nameLabel, "label");
				shift(tok);
				strcat(nameLabel, tok);
				map<char*, int>::iterator it = mapLabelPos.find(nameLabel);
				if(it == mapLabelPos.end())
				{
					cout <<	"FALHA, LABEL " << nameLabel << "NÃO ENCONTRADO NA MAP!\n\n";
					exit(EXIT_FAILURE);
				}
				else
				{
					if(it->second < posLinha)
						blocos.insert(pair<int, int>(it->second, posLinha));
				}
			}
		}

	}while(strcmp(linha, "!end") != 0);
	cout << "Terceira etapa parte 2 finalizada\n";

	//Terceiraa etapa - parte 3: criar os pacotes do loop
	code.str("");					//código principal do programa
	stringstream packagesLoops;		//parte do código apenas com os pacotes já definidos para os laços
	int laco = 1;					//identificador dos laços para os pacotes
	int subLinhas = 0;					//númerode linhas que deve ser subtraido do indicado de fim do laço externo
	if(blocos.size() > 0)
	{

		map<int, int>::reverse_iterator it;
		for(it = blocos.rbegin(); it != blocos.rend(); it++)
		{
			code.str("");
			it->second -= subLinhas;
			cout << "it->first = " << it->first << " it->second = " << it->second << endl;
			subLinhas = 0;
			io->openRead(nameProgram);
			int posLinha = 0;

			packagesLoops << "PACKAGE loop" << laco << endl;
			packagesLoops << "ADDRESS MAU_0\n";

			//adianta linhas até chegar no inicio do bloco
			do{
				io->readLn(linha);
				code << linha << endl;
				posLinha++;
			} while ((posLinha + 1) < it->first);
			io->readLn(linha);
			posLinha++;
			code << "\tEXEC MAU_0; ";
			code << "Program1, loop" << laco << ";"  << endl;

			io->readLn(linha);
			posLinha++;
			while(strcmp(linha, "!end") != 0 && posLinha < it->second)
			{
				packagesLoops << "\t" << linha << "\n";
				io->readLn(linha);
				posLinha++;
			}
			io->readLn(linha);
			posLinha++;

			packagesLoops << "\tEXEC MAU_0; ";
			packagesLoops << "Program1, loop" << laco << ";" << endl;
			packagesLoops << "\tJUMP end_loop;\n";
			packagesLoops << "\t" << linha << endl;
			packagesLoops << "\tEXEC MAU_0; ";
			packagesLoops << "Program1, end_loop" << laco << ";"<< endl;
			packagesLoops << "\tend_loop:\n";
			packagesLoops << "END\n";

			packagesLoops << "PACKAGE end_loop" << laco << endl;
			packagesLoops << "ADDRESS MAU_0\n";

			io->readLn(linha);
			posLinha++;
			strcpy(linhaCopia, linha);
			tok = strtok(linhaCopia, " ");
			aux = strtok(NULL, " ");
			while(strcmp(linha, "!end") != 0 && strcmp(tok, "ret") != 0)
			{				
				if(strcmp(tok, "br") == 0 && strcmp(aux, "label") == 0)
				{
					tok = strtok(NULL, " ");
					shift(tok);
					char nameLabel[25];
					strcpy(nameLabel, "label");
					strcat(nameLabel, tok);
					map<char *, int>::iterator i = mapLabelPos.find(nameLabel);
					if(i == mapLabelPos.end())
					{
						cout <<	"FALHA, LABEL NÃO ENCONTRADO NA MAP!\n\n";
						exit(EXIT_FAILURE);
					}
					else if(posLinha > i->second)		//encontrado um jump de retorno para o laço mais externo
					{
						packagesLoops << "\tEXEC MAU_0; ";
						packagesLoops << "Program1, loop" << laco + 1 << ";" << endl;
						code << linha << endl;
						break;
					}
					else								//desvio para frente no código
					{
						packagesLoops << "\t" << linha << "\n";
					}
				}
				else
				{
					packagesLoops << "\t" << linha << "\n";
				}
				io->readLn(linha);
				posLinha++;
				subLinhas++;

				strcpy(linhaCopia, linha);
				tok = strtok(linhaCopia, " ");
				aux = strtok(NULL, " ");
			}
			if (strcmp(tok, "ret") == 0)
				code << linha << endl;
			subLinhas += (it->second - it->first + 1);
			packagesLoops << "\tNOP NOP NOP\n";
			packagesLoops << "END\n";


			io->readLn(linha);
			while(strcmp(linha, "!end") != 0)
			{
				code << linha << endl;
				io->readLn(linha);
			}
			io->openWrite(nameProgram);
			io->writeCode(code.str());
			laco++;
		}

		io->openWrite(nameProgram);		
		io->writeCode(code.str());
		
		io->openRead(nameProgram);
		code.str("");
		io->readLn(linha);
		strcpy(linhaCopia, linha);
		tok = strtok(linhaCopia, " ");
		while(strcmp(tok, "attributes") != 0)
		{
			code << linha << endl;			
			io->readLn(linha);
			strcpy(linhaCopia, linha);
			tok = strtok(linhaCopia, " ");
		}

		code << packagesLoops.str();
		code << "attributes #0" << endl;
		io->openWrite(nameProgram);		
		io->writeCode(code.str());


		cout << "##############################################################\n";
		cout << "########################## MAIN ##############################\n";
		cout << code.str();
		cout << "\n##############################################################\n\n";
		/*cout << "######################### PACKAGE ############################\n";
		cout << packagesLoops.str() << endl;
		cout << "##############################################################\n";*/
	}

	cout << "Terceira etapa parte 3 finalizada\n";
	map<char*, int>::iterator ity;
	for(ity = mapLabelPos.begin(); ity != mapLabelPos.end(); ity++)
		delete ity->first;

	delete linha;
	delete linhaCopia;
}