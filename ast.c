#include <stdio.h>

enum {
	A_ADD, A_SUB,  A_MUL, A_DIV, A_INTLIT
};
char* ASTop[] = { "+" , "-", "*", "/"};

typedef struct ASTnode{
	int op;// operation to be performed
	struct ASTnode *left, *right; //child trees
	int intvalue; //For A_INTLIT, the integer value
}ASTnode;

//Building a ast
// just make the ast

ASTnode* mkastnode(int op, ASTnode* left, ASTnode* right, int intvalue)
{
	ASTnode* n = (ASTnode*)malloc(sizeof(ASTnode));
	if(n == NULL)
	{
		fprintf(stderr, "Unable to malloc in mkastnode()\n");
		exit(1);
	}
	n->op       = op;
	n->left     = left;
	n->right    = right;
	n->intvalue = intvalue;
	return (n);
}

ASTnode *mkastleaf(int op, int intvalue)
{
	return mkastnode(op, NULL, NULL, intvalue);
}

ASTnode *mkastunary(int op,ASTnode* left, int intvalue)
{
	return mkastnode(op, left, NULL, intvalue);
}


tok_to_ast(int tok)
{
	switch(tok){
	case T_PLUS:
		return (A_ADD);
	case T_MINUS:
		return (A_SUB);
	case T_STAR:
		return (A_MUL);
	case T_SLASH:
		return (A_DIV);
	default:
		fprintf(stderr, "Unknown token in tok_to_ast %d\n", Line);
		exit(1);
		return (0);
	}
}

static ASTnode* primary(void)
{
	ASTnode* n;

	switch(Token.token){
	case T_INTLIT:
		n = mkastleaf(A_INTLIT, Token.intvalue);
		scan(&Token);
		return (n);
	default:
		fprintf(stderr, "syntax error on line %d\n", Line);
		exit(1);
		return (0);
	}
}

static int OpPrec[] = {0, 10, 10, 20, 20, 0};


static int op_precedence(int tokentype)
{
	int prec = OpPrec[tokentype];
	if(prec == 0)
	{
		fprintf(stderr, "Syntax error on line %d, token %d\n", Line, tokentype);
		exit(1);
	}
	return (prec);
}


//return a AST tree whose root is a binary expression
//previous tokentype
static ASTnode* binexpr(int ptp)
{
	ASTnode *left, *right;

	left = primary();

	int tokentype = Token.token;
	if(Token.token == T_EOF)
	{
		return (left);
	}

	while(op_precedence(tokentype) > ptp)
	{
		scan(&Token);

		right = binexpr(OpPrec[tokentype]);

		left  = mkastnode(tok_to_ast(tokentype), left, right, 0);

		tokentype = Token.token;
		if(tokentype == T_EOF)
		{
			return (left);
		}
	}

	return (left);
}


static int interpretAST(ASTnode* n)
{
	int leftval, rightval;

	if(n->left)
	{
		leftval = interpretAST(n->left);
	}
	if(n->right)
	{
		rightval = interpretAST(n->right);
	}

	if (n->op == A_INTLIT)
	{
		printf("int %d\n", n->intvalue);
	}
	else
	{
		printf("%d %s %d\n", leftval, ASTop[n->op], rightval);
	}

	switch (n->op)
	{
	case A_ADD:
		return (leftval + rightval);
	case A_SUB:
		return (leftval - rightval);
	case A_MUL:
		return (leftval * rightval);
	case A_DIV:
		return (leftval / rightval);
	case A_INTLIT:
		return (n->intvalue);
	default:
		fprintf(stderr, "Unknown AST operator %d\n", n->op);
		exit(1);
	}
}







