
FILE* Outfile;
static int freereg[4];
static char* reglist[] = {"%r8", "%r9", "%r10", "%r11"};

void freeall_registers(void)
{
  freereg[0]= freereg[1]= freereg[2]= freereg[3]= 1;
}

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int alloc_register(void)
{
  for (int i=0; i<4; i++) {
    if (freereg[i]) {
      freereg[i]= 0;
      return(i);
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
static void free_register(int reg)
{
  if (freereg[reg] != 0) {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg]= 1;
}


void cgpreamble()
{
  freeall_registers();
  fputs(
	"\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n"
	"\tnop\n"
	"\tleave\n"
	"\tret\n"
	"\n"
	"\t.globl\tmain\n"
	"\t.type\tmain, @function\n"
	"main:\n"
	"\tpushq\t%rbp\n"
	"\tmovq	%rsp, %rbp\n",
  Outfile);
}

void cgpostamble()
{
  fputs(
	"\tmovl	$0, %eax\n"
	"\tpopq	%rbp\n"
	"\tret\n",
  Outfile);
}


int cgload(int value)
{
	//get a new register
	int r = alloc_register();

	fprintf(Outfile, "\t movq\t$%d, %s\n", value, reglist[r]);
	return(r);
}

int cgadd(int r1, int r2)
{
	fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return (r2);
}

int cgmul(int r1, int r2)
{
	fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return (r2);
}

int cgsub(int r1, int r2)
{
	fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
	free_register(r2);
	return (r1);
}

int cgdiv(int r1, int r2)
{
	fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[r1]); //load to rax
	fprintf(Outfile, "\tcqo\n"); //extend to 8 bytes
	fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]); 
	fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]); 
	free_register(r1);
	return (r2);
}

void cgprintint(int r)
{
	fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(Outfile, "\tcall\tprintint\n");
	free_register(r);
}


//Generate assembly code
static int genAST(ASTnode* n)
{
	int leftreg, rightreg;

	//get the left and right subtree value
	if (n->left)
	{
		leftreg = genAST(n->left);
	}
	if (n->right)
	{
		rightreg = genAST(n->right);
	}

	switch(n->op)
	{
	case A_ADD:      return (cgadd(leftreg, rightreg));
	case A_SUB:      return (cgsub(leftreg, rightreg));
	case A_MUL:      return (cgmul(leftreg, rightreg));
	case A_DIV:      return (cgdiv(leftreg, rightreg));
	case A_INTLIT:   return (cgload(n->intvalue));
	default:
		fprintf(stderr, "unknown AST operator %d\n", n->op);
		exit(1);
	}

}

static void generatecode(ASTnode* n)
{
	int reg;

	cgpreamble();
	reg = genAST(n);
	cgprintint(reg); //print the register with the result as an int
	cgpostamble();
}

