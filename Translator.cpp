#include "Translator.h"

Translator::Translator(const char name[], const char path[])
{
	strcpy(fontName, name);
	strcpy(pathName, path);
	strcat(pathName, "/");
	cmp = VariableCell::lessThan;
	copies = set<VariableCell*, bool(*)(VariableCell*, VariableCell *)>(cmp);
	cout << "Nome do arquivo fonte: " << fontName << endl;
	cout << "Nome do caminho: " << pathName << endl;
	funcSign = new FunctionSignature();

}

Translator::~Translator()
{
	clearCopies();
	delete io;
	delete funcSign;
}

void Translator::createIR()
{
	//clang nome_do_fonte.c -S -emit-llvm  <----- modelo de chamada do Clang
	char command[400] = "clang ";
	strcat(command, pathName);
	strcat(command, fontName);
	strcat(command, ".c");
	strcat(command, " -S -emit-llvm");
	//cout << command << endl;
	system(command);

	//FAZ AQUI O PRÉPROCESSAMENTO <------------------------------------------------------------
	char local[400];
	strcpy(local, fontName);
	strcat(local, ".ll");
	Preprocessor pre(local);
	pre.optimize();

	io = new IOPack();
	io->openRead(local);
}

void Translator::instructionTranslation(char * line)
{
	char * tok;								//token para pegar comandos
	char * aux;								//token de auxilio
	char * linha = line;					//linha com a instrução
	
	aux = strtok(linha, " \t");
	if (strcmp(aux, "store") == 0)	//exemplo de store: store i32 %2, i32* %a, align 4
	{
		tok = strtok(NULL, " ");	//tipo da variável
		tok = strtok(NULL, " ");	//id da variável
		int tam = strlen(tok);
		tok[tam - 1] = '\0';
		string idvar = tok;
		tok = strtok(NULL, " ");	//tipo da variável global
		tok = strtok(NULL, " ");	//id da variável global
		shift(tok);
		if (strchr(tok, ',') != NULL)
		{
			tam = strlen(tok);
			tok[tam - 1] = '\0';
		}
		string idgvar = "";
		//se a variável for apenas números (o que não é válido como token mas o
		//clang produz esse tipo de variável) então é adicionado um "z" antes do número.
		if (isNumber(tok))
		{
			idgvar = "z";
		}
		idgvar += tok;
		cout << "&KEY: (" << idgvar << ")" << " &TOK: (" << tok << ")\n";

		VariableCell vcell(idgvar);
		set<VariableCell*>::iterator it = copies.find(&vcell);
		if (it != copies.end())
		{
			VariableCell * variablecell = (*it);
			copies.erase(it);
			variablecell->copies->clear();
			delete variablecell;
		}

		addTab();
		mainCode << "COPY " << "c" << nCopies++ << " c" << nCopies++ << ";\n";
		addTab();
		mainCode << "\t" << idvar << ";\n";

		stringstream temp2;
		temp2.str("");
		list<string> * ltemp = new list < string > ;
		temp2 << "c" << (nCopies - 2);
		ltemp->push_back(temp2.str());
		temp2.str("");
		temp2 << "c" << (nCopies - 1);
		ltemp->push_back(temp2.str());
		copies.insert(new VariableCell(idgvar, ltemp));
	}
	else if (strcmp(aux, "br") == 0)
	{
		tok = strtok(NULL, " ");
		if (strcmp(tok, "label") == 0)
		{
			addTab();
			mainCode << "JUMP " << tok;
			tok = strtok(NULL, " ");
			shift(tok);
			mainCode << tok << ";\n";
		}
	}
	else if (strcmp(aux, ";") == 0)
	{
		tok = strtok(NULL, " ");
		if (tok[0] == '<' && tok[1] == 'l')		//identifica a declaração de um label
		{
			tok = strtok(tok, ":");
			shift(tok);
			int n = strlen(tok);
			tok[n-1] = '\0';
			mainCode << tok;
			tok = strtok(NULL, ":");
			mainCode << tok << ": \n";
		}
	}
	else if (strcmp(aux, "EXEC") == 0)
	{
		addTab();
		mainCode << aux;
		while (aux = strtok(NULL, " "))
			mainCode << " " << aux;
		mainCode << endl;
	}
	else if (strcmp(aux, "JUMP") == 0)	//JUMP end_loop;
	{
		addTab();
		mainCode << aux << " ";
		aux = strtok(NULL, " ");
		mainCode << aux << endl;
	}
	else if(strcmp(aux, "end_loop:") == 0)
	{
		addTab();
		mainCode << aux << " ";
	}
	else if (strcmp(aux, "ret") == 0)
	{
		//no main não é necessário retornos, mas no IR contém retornos, então serão descartados
	}
	else if (strcmp(aux, "NOP") == 0)
	{
		//apenas para pular esse comando gerado no próprio Translator::translation();
	}
	else
	{
		tok = strtok(NULL, " ");
		tok = strtok(NULL, " ");
		if (strcmp(tok, "alloca") == 0)
		{
			shift(aux);
			string idv = "";
			//se a variável for apenas números (o que não é válido como token mas o
			//clang produz esse tipo de variável) então é adicionado um "z" antes do número.
			if (isNumber(aux))
			{
				idv = "z";
			}
			idv += aux;
			varCode << "\t" << idv;
			varCode << " = 0\n";
		}
		else if (strcmp(tok, "load") == 0)		//exemplo de load: %1 = load i32* %e, align 4
		{
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			shift(tok);
			int t = strlen(tok);
			tok[t - 1] = '\0';
			string temp = tok;
			VariableCell vcell(tok);
			set<VariableCell*>::iterator ittemp = copies.find(&vcell);
			if (ittemp != copies.end())		//contém uma cópia no pacote
			{
				if ((*ittemp)->copies->size() > 1)
				{
					addTab();
					mainCode << "COPY " << aux << ";\n";
					addTab();
					mainCode << "\t" << (*ittemp)->getCopy() << ";\n";
				}
				else
				{
					//se só tem uma, então consome ela fazendo uma nova cópia
					addTab();
					mainCode << "COPY " << "c" << nCopies++ << " c" << nCopies++ << ";\n";
					addTab();
					mainCode << "\t" << (*ittemp)->getCopy() << ";\n";
					stringstream s;
					s.str("");
					s << "c" << (nCopies - 1);
					(*ittemp)->copies->push_back(s.str());
					s.str("");
					s << "c" << (nCopies - 2);
					(*ittemp)->copies->push_back(s.str());

					//após ter a cópia, consome-se mais uma variável
					addTab();
					mainCode << "COPY " << aux << ";\n";
					addTab();
					mainCode << "\t" << (*ittemp)->getCopy() << ";\n";
				}
			}
			else
			{
				addTab();
				mainCode << "LOAD MAU_0 " << aux << ";\n";
				addTab();
				mainCode << "\t" << tok << ";\n";
			}

		}
		else if (strcmp(tok, "add") == 0)
		{
			addTab();
			mainCode << "ADD " << aux << ";\n";
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			int tam = strlen(tok);
			tok[tam - 1] = '\0';
			addTab();
			mainCode << "\t" << tok << "\n";
			tok = strtok(NULL, " ");
			addTab();
			mainCode << "\t" << tok << ";\n";
		}
		else if (strcmp(tok, "sub") == 0)
		{
			addTab();
			mainCode << "SUB " << aux << ";\n";
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			int tam = strlen(tok);
			tok[tam - 1] = '\0';
			addTab();
			mainCode << "\t" << tok << "\n";
			tok = strtok(NULL, " ");
			addTab();
			mainCode << "\t" << tok << ";\n";
		}
		else if (strcmp(tok, "mul") == 0)
		{
			addTab();
			mainCode << "MUL " << aux << ";\n";
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			int tam = strlen(tok);
			tok[tam - 1] = '\0';
			addTab();
			mainCode << "\t" << tok << "\n";
			tok = strtok(NULL, " ");
			addTab();
			mainCode << "\t" << tok << ";\n";
		}
		else if (strcmp(tok, "sdiv") == 0)
		{
			addTab();
			mainCode << "DIV " << aux << ";\n";
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			int tam = strlen(tok);
			tok[tam - 1] = '\0';
			addTab();
			mainCode << "\t" << tok << "\n";
			tok = strtok(NULL, " ");
			mainCode << "\t" << tok << ";\n";
		}
		else if (strcmp(tok, "icmp") == 0)
		{
			//%3 = icmp slt i32 %1, %2
			//br i1 %3, label %4, label %6
			char var1[50], var2[50], branch[10], jump1[40], jump2[50];
			tok = strtok(NULL, " ");
			strcpy(branch, tok);
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			strcpy(var1, tok);
			int t = strlen(var1);
			var1[t - 1] = '\0';
			tok = strtok(NULL, " ");
			strcpy(var2, tok);

			//br i1 %4, label %5, label %7
			io->readLn(linha);
			tok = strtok(linha, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			shift(tok);

			if (strcmp(branch, "slt") == 0)	//menor que
			{
				addTab();
				mainCode << "BGE " << "label" << tok << ";\n";
				addTab();
				mainCode << "\t" << var1 << "\n";
				addTab();
				mainCode << "\t" << var2 << ";\n";
			}
			else if (strcmp(branch, "eq") == 0)	//igual
			{
				addTab();
				mainCode << "BNE " << "label" << tok << ";\n";
				addTab();
				mainCode << "\t" << var1 << "\n";
				addTab();
				mainCode << "\t" << var2 << ";\n";
			}
			else if (strcmp(branch, "sgt") == 0)	//maior que
			{
				addTab();
				mainCode << "BLE " << "label" << tok << ";\n";
				addTab();
				mainCode << "\t" << var1 << "\n";
				addTab();
				mainCode << "\t" << var2 << ";\n";
			}
			else if (strcmp(branch, "sle") == 0)	//menor igual
			{
				addTab();
				mainCode << "BG " << "label" << tok << ";\n";
				addTab();
				mainCode << "\t" << var1 << "\n";
				addTab();
				mainCode << "\t" << var2 << ";\n";
			}
			else if (strcmp(branch, "sge") == 0)	//maior igual
			{
				addTab();
				mainCode << "BL " << "label" << tok << ";\n";
				addTab();
				mainCode << "\t" << var1 << "\n";
				addTab();
				mainCode << "\t" << var2 << ";\n";
			}
			io->readLn(linha);	//descarta a linha, label desnecessário
		}
		else if(strcmp(tok, "call") == 0) // %2 = call i32 @soma(i32 10, i32 15)
		{
			addTab();
			mainCode << "CALL " << aux << ";\n";
			addTab();
			mainCode << "MAU_0" << endl;
			addTab();
			mainCode << "prog, ";
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			shift(tok);
			int tamanho = strlen(tok);
			tok[tamanho - 4] = '\0';
			signature * sign = funcSign->find(tok);
			mainCode << tok;
			tok = strtok(NULL, " ");
			int numero = strlen(tok);
			tok[numero - 1] = '\0';
			mainCode << "\n";
			for(list<string>::iterator it = sign->param->begin(); it != sign->param->end(); it++)
			{
				addTab();
				mainCode << (*it) << endl;
				addTab();
				mainCode << tok << endl;
				tok = strtok(NULL, " ");
				tok = strtok(NULL, " ");

				if(tok != NULL)
				{
					numero = strlen(tok);
					tok[numero - 1] = '\0';
				}
			}
			mainCode << ";\n";
		}
	}
}

void Translator::translation()
{
	char * linha = new char[io->LINE];		//armazena a linha lida do arquivo nessa variável
	char * linhaBkp = new char[io->LINE];	//linha de backup
	char * tok;								//token para pegar comandos
	char * aux;								//token de auxilio
	nCopies = 1;							//contador para identificar as cópias
	nTabs = 0;								//quantidade de tabs para indentar

	do{

		io->readLn(linha);
		if (strlen(linha) == 0)
			io->readLn(linha);
		tok = strtok(linha, " ");
		if (strcmp(tok, ";") == 0)			//encontrou uma função ou main
		{
			io->readLn(linha);
			tok = strtok(linha, " ");
			aux = strtok(NULL, " ");		//indica o retorno
			tok = strtok(NULL, " ");

			//verifica se é main
			if (strcmp(tok, "@main()") == 0)
			{
				clearCopies();
				mainCode << "PACKAGE main\n";
				mainCode << "ADDRESS MAU_0\n";
				io->readLn(linha);
				strcpy(linhaBkp, linha);
				aux = strtok(linha, " ");
				nTabs++;
				while (strcmp(aux, "}") != 0)
				{
					instructionTranslation(linhaBkp);	//faz a tradução da instrução e já copia para variável mainCode
					io->readLn(linha);
					strcpy(linhaBkp, linha);
					aux = strtok(linha, " ");

				}
				//faz o stores pendentes em pacote
				cout << "\n\n\ncopies.size: " << copies.size() << endl;
				for (set<VariableCell*>::iterator u = copies.begin(); u != copies.end(); u++)
				{
					addTab();
					mainCode << "STORE MAU_0;\n";
					addTab();
					mainCode << "\t" << (*u)->getCopy() << "\n";
					addTab();
					mainCode << "\t" << (*u)->key << ";\n";
				}
				clearCopies();

				nTabs--;
				mainCode << "END\n";
			}
			else if(tok[0] == '@')	//função normal, exemplo: define i32 @soma(i32 %a, i32 %b) #0 {
			{
				clearCopies();
				shift(tok);
				stringstream funcName;
				while(tok[0] != '(')		//separa o nome da função dos seus parâmetros
				{
					funcName << tok[0];
					shift(tok);
				}

				list<string> * l = new list<string>();
				string parametros;
				tok = strtok(NULL, " ");
				int fim;
				while(strcmp(tok, "{") != 0)		//extrai os parâmetros da função
				{
					cout << "ENTRANDO NO LOOP\n";
					shift(tok);
					fim = strlen(tok);
					fim -= 1;
					tok[fim] = '\0';
					parametros = tok;
					l->push_back(parametros);
					tok = strtok(NULL, " ");
					tok = strtok(NULL, " ");
				}
				
				signature * s = new signature;
				s->param = l;
				s->name = new char[200];
				funcName.str().copy(s->name, funcName.str().size());
				funcSign->add(s);

				mainCode << "FUNCTION " << funcName.str() << endl;
				mainCode << "ADDRESS MAU_0\n";


				io->readLn(linha);
				strcpy(linhaBkp, linha);
				aux = strtok(linha, " ");
				nTabs++;
				while (strcmp(aux, "}") != 0)
				{
					if(strcmp(aux, "ret") == 0) //exemplo: ret i32 %6
					{
						aux = strtok(NULL, " ");
						aux = strtok(NULL, " ");
						mainCode << "\tRETURN;\n";
						mainCode << "\t\tc" << aux << ";\n";
						strcpy(linhaBkp, "NOP NOP NOP");
					}

					instructionTranslation(linhaBkp);	//faz a tradução da instrução e já copia para variável mainCode
					io->readLn(linha);
					strcpy(linhaBkp, linha);
					aux = strtok(linha, " ");

				}

				mainCode << "END\n";				
			}	// final da análise da função
		}
		else if (strcmp(tok, "PACKAGE") == 0)
		{
			mainCode << "PACKAGE ";
			tok = strtok(NULL, " ");
			mainCode << tok << endl;
			io->readLn(linha);
			tok = strtok(linha, " ");
			mainCode << linha << endl;

			io->readLn(linha);
			strcpy(linhaBkp, linha);
			aux = strtok(linha, " ");
			nTabs++;
			while (strcmp(aux, "END") != 0)
			{
				instructionTranslation(linhaBkp);		//faz a tradução da instrução e já copia para variável mainCode
				io->readLn(linha);
				strcpy(linhaBkp, linha);
				aux = strtok(linha, " ");
			}

			mainCode << "END" << endl;

		}
		else if (strcmp(tok, "attributes") == 0)
		{
			cout << "ENCONTROU O FIM DO PROGRAMA EM ARQUIVO\n";
			break;
		}
	} while (strcmp(linha, "!end") != 0);

	delete linha;
	delete linhaBkp;

	cout << "#Variaveis: \n" << varCode.str() << endl;
	cout << "#Codigo principal: \n" << mainCode.str() << endl;
}

