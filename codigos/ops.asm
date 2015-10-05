PROGRAM ops

DATA

	z1 = 0
	a = 0
	b = 0
	c = 0

PACKAGE main
ADDRESS MAU_0
	COPY c2 c1;
		0;
	LOAD MAU_0 %2;
		a;
	SUB %3;
		%2
		1;
	COPY c4 c3;
		%3;
	LOAD MAU_0 %4;
		b;
	MUL %5;
		%4
		2;
	COPY c6 c5;
		%5;
	LOAD MAU_0 %6;
		c;
	DIV %7;
		
	
END_PROGRAM
