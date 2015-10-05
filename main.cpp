/*
	Descri��o: Programa que faz an�lise l�xica, sint�tica, sem�ntica,
		gera��o de c�digo intermedi�rio e tradu��o para o PDL da 
		arquitetura IPNoSys.

	data:				??/09/2014
	modificado:			21/09/2015
	vers�o:				1.2.1
	vers�o do Clang:	3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)
	autor:				Ulysses Gomes
*/

#include "Translator.h"
#include <cstdio>

int main()
{
	Translator t("ops2", "/home/ulysses/Desktop/Programas_em_C");
	//Translator t("com_funcao", "/home/ulysses/Desktop/Programas_em_C");
	t.createIR();
	t.translation();
	t.saveToPDL();

	cout << "Fim do programa" << endl << endl;
	
	return 0;
}
