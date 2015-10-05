PROGRAM if

DATA

	z1 = 0
	a = 0
	b = 0

PACKAGE main
ADDRESS MAU_0
	COPY c2 c1;
		0;
	COPY c4 c3;
		0;
	COPY c6 c5;
		1;
	COPY %2;
		c3;
	COPY %3;
		c5;
	BL label7;
		%2
		%3;
	COPY c8 c7;
		c6;
	COPY %6;
		c8;
	COPY c10 c9;
		%6;
label7: 
	STORE MAU_0;
		c9
		a;
	STORE MAU_0;
		c7
		b;
	STORE MAU_0;
		c1
		z1;
END

END_PROGRAM
