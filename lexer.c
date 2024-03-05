#include <stdio.h>
#include <stdbool.h>


//Let use unity build for now and maybe forever
int   Line;
int   Putback;
FILE* InFile;

static putback(int c)
{
	Putback = c;
}


typedef struct token 
{
	int token;
	int intvalue; // this is true while T_INTLIT is true
}token;

token Token;

enum {T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT};
char* tokstr[] = { "+" , "-", "*", "/", "INTLIT" };


//(note) Putback is a int we should use it if it has value instead of next int

static int next(void)
{
	int c;
	if(Putback)
	{ 
		c = Putback;
		Putback = 0; // if we used the putback set it to zero
	}else
	{
		c = fgetc(InFile); // get the next token from file
		if('\n' == c) Line++; // just for storing the line number
	}
	return c; //Having return at the end always?
}

//skip the whitespace because we don't care about it

static int skip(void)
{
	int c;
	c = next();
	while( ' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c ) {
		c = next();
	}
	return c;
}

static int chrpos(char* s, int c)
{
	char* p;
	p = strchr(s, c);
	return (p ? p-s : -1); //this gives distance but distance is the required value
}


static int scanint(int c)
{
	int k, val = 0;

	while((k = chrpos("0123456789", c)) >= 0)
	{
		val = val * 10 +k;
		c   = next();
	}
	//we hit a non integer value so quitely put it back
	putback(c);
	return val;
}


//Scanner
//Return is just true or false
static bool scan(token* t)
{
	int c;
	c = skip();

	switch(c)
	{
	case EOF:
		t->token = T_EOF;
		return(0);
	case '+':
		t->token = T_PLUS;
		break;
	case '-':
		t->token = T_MINUS;
		break;
	case '*':
		t->token = T_STAR;
		break;
	case '/':
		t->token = T_SLASH;
		break;
	default:
		//More here soon
		if(isdigit(c)){
			t->token    = T_INTLIT;
			t->intvalue = scanint(c);
			break;
		}
		printf("Unrecognized character %c on line %d\n", c, Line);
		exit(1);
	}
	return (true);
}


static void init()
{
	Line = 1;
	Putback = '\n';
}











