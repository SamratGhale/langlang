#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>

#include "lexer.c"
#include "ast.c"
#include "gen.c"

//Let use unity build for now and maybe forever
static void usage(char const* prog)
{
	fprintf(stderr, "%s Usuage: infile \n", prog);
	exit(1);
}


static void scanfile()
{
	token T;
	while(scan(&T))
	{
		printf("Token %s", tokstr[T.token]);
		if(T.token == T_INTLIT)
		{
			printf(", value = %d", T.intvalue);
		}
		printf("\n");
	}
}


int main(int argc, char const *argv[])
{
	if(argc == 1)
	{
		usage(argv[0]);
	}

	init();

	if((InFile = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "Unable to open %s\n", argv[1]);
		exit(1);
	}

	//Create the output file
	if((Outfile = fopen("out.s", "w")) == NULL)
	{
		fprintf(stderr, "Unable to create output file %s\n", strerror(errno));
		exit(1);

	}
	//scanfile();
	scan(&Token);
	ASTnode* node =  binexpr(0);

	printf("%d \n", interpretAST(node));
	generatecode(node);
	fclose(Outfile);

	return 0;
}












