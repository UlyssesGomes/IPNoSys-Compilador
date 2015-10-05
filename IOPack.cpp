#include "IOPack.h"

/*IOPack::IOPack(char n[])
{
	LINE = 200;
	flagFilesRead = flagFilesWrite = false;

	fin = new ifstream();
	fout = new ofstream();

	char n2[50];
	strcpy(n2, n);
	strcat(n2, ".pdl");
	openRead(n2);
	if(!fin->is_open())
	{
		cout << "Falha ao abrir arquivo " << n << "." << endl;
	}

	char * line = new char[LINE];
	
	for(int u = 0; u < 4; u++)
		fin->getline(line, LINE);
	delete line;
}*/

IOPack::IOPack()
{
	LINE = 200;
	flagFilesRead = flagFilesWrite = false;

	fin = new ifstream();
	fout = new ofstream();
}

void IOPack::openRead(char n[])
{
	if(flagFilesRead)
		closeReadFile();

	fin->open(n, ios_base::in);
	flagFilesRead = true;
}

void IOPack::openWrite(char n[])
{
	if(flagFilesWrite)
		closeWriteFile();

	fout->open(n, ios_base::out | ios_base::trunc);	
	flagFilesWrite = true;
}

void IOPack::closeReadFile()
{
	fin->close();
	flagFilesRead = false;
}

void IOPack::closeWriteFile()
{
	fout->close();
	flagFilesWrite = false;
}

IOPack::~IOPack()
{
	if(flagFilesWrite)
		closeReadFile();
	if(flagFilesRead)
		closeWriteFile();

	delete fin;
	delete fout;
}

void IOPack::readLn(char * line)
{
	if(!fin->good())
	{
		strcpy(line,"!end");
		closeReadFile();
		return;
	}
	fin->getline(line, LINE);
	//cout << line << endl;
}

void IOPack::writeCode(string c)
{
	*(fout) << c << endl;
}

//apenas reseta o arquivo para a forma original antes
//de todas as modificações feitas
void IOPack::fazerCopia(char * fonte, char * copia)
{
	openRead(fonte);
	openWrite(copia);

	char * line = new char[LINE];
	string j;
	readLn(line);
	while (strcmp(line, "!end") != 0)
	{
		j += line;
		j += "\n";
		readLn(line);
	}

	writeCode(j);
}