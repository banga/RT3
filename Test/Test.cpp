// Test.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

int yyparse(void);
int _tmain(int argc, _TCHAR* argv[])
{
    yyparse();
	return 0;
}