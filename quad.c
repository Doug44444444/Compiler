#include "symtab.h"
#include <stdio.h>
#include "quad.h"
struct Quadstruct{
    char op; int a1,a2,a3;
}; typedef struct Quadstruct Quad;
static Quad quad[MAXQUADS];
static int quadCt = 0;

static gen(char op, int a1, int a2, int a3){
    if (quadCt >= MAXQUADS) yyerror("quad overflow");
    quad[quadCt].op = op; quad[quadCt].a1 = a1;
    quad[quadCt].a2 = a2; quad[quadCt].a3 = a3;
    quadCt++;
}

void qbackPatch(int pos, int trg){
         quad[pos].a3 = trg;
}

void loop(char op, int op1){
         int qud;
         qud = op1;
         gen('j',-1,-1,op1);
}

void getQd(int ix, char * op, int * a1, int * a2, int * a3){
    *op = quad[ix].op; 
    *a1 = quad[ix].a1; *a2 = quad[ix].a2; *a3 = quad[ix].a3;
}

int  getA1(int ix){return quad[ix].a1;}


int  qOp( char op, int op1, int op2){ /* ret := op1 op op2                    */
    Type tp1,tp2;  int trg; int n;
    /*printf("qOp %c\n",op);*/
    switch(op){
     case '+': case '-': case '*': case '/': case '>': case '<': case 'E':
	if(tp(op1)==ITP && tp(op2)==ITP) { trg = gettemp( ITP ); gen(op,op1,op2,trg); return trg;}
	if(tp(op1)==RTP && tp(op2)==RTP) { trg = gettemp( RTP ); gen(op,op1,op2,trg); return trg;}
	if(tp(op1)==ITP && tp(op2)==RTP) { op1 = qUo('R', op1); trg = gettemp( RTP ); gen(op,op1,op2,trg); return trg;}
	if(tp(op1)==RTP && tp(op2)==ITP) { op2 = qUo('R', op2); trg = gettemp( RTP ); gen(op,op1,op2,trg); return trg;}
	   yyerror("Operands are not the same type!");
     case 'A': case 'O': 
        trg = gettemp( ITP );        
        gen(op,op1,op2,trg); return trg;
     case '%': 
 	if(tp(op1)==ITP && tp(op2)==ITP) { trg = gettemp( ITP ); gen(op,op1,op2,trg); return trg;}
	    yyerror("Operand are not integer!");
     case 'V':
	    if(op2 == -1){
            trg = gettemp(ITP);
            gen(op,op1,-1,trg); return trg;
		}
	    else{
            trg = gettemp(RTP);
            gen(op,op1,op2,trg); return trg;
		}
     case '@':  
	    if(op2 == -1){ 
	        n = tp(op1);
	        switch(n){
                case ITP: trg = gettemp( ITP );
                case RTP: trg = gettemp( RTP );
	        }	
        }
        else{
		    n = tp(op2);
	        switch(n){
                case ITP: trg = gettemp( ITP );
                case RTP: trg = gettemp( RTP );
	        }			
		}		
        gen(op,op1,op2,trg); return trg;
     default:  yyerror("bad qOp"); 
    }
}

void qPo(char op, int op1, int src, int trg){
    if(tp(trg)==ITP && tp(src)==RTP) {  src = qUo('I', src); }
    if(tp(trg)==RTP && tp(src)==ITP) {  src = qUo('R', src); }
	gen(op,op1,src,trg); 
}

void qSf(int trg){
    gen('r',-1,-1,trg);
}

int qCa(int i, int n){
    int trg; int base, plus, addr;
    base = qUo('&', i);
    if(tp(i)==ITP) { plus = qOp('V', 4, -1); }else{ plus = qOp('V', 8, -1);}
    addr = qOp('*', n, plus);
    addr = qOp('+', base, addr);
    if(tp(i)==RTP){typeTran(i, addr);}
    return addr;
}

void qSa(int i, int n, int src){
    int trg; int base, plus, addr;
    base = qUo('&', i);
    if(tp(i)==ITP) { plus = qOp('V', 4, -1); }else{ plus = qOp('V', 8, -1);}
    addr = qOp('*', n, plus);
    addr = qOp('+', base, addr);
    if(tp(i)==RTP){typeTran(i, addr);}
    qPo( '@', -1, src, addr);
}

int qUo( char op, int op1 ){  /* ret := op op1, jump to op1, parm op1 */
    Type tp1;  int trg; int qud;
    switch (op) {
     case 'M':
	 	if(tp(op1)==ITP) { trg = gettemp( ITP ); }
		if(tp(op1)==RTP) { trg = gettemp( RTP ); }
		gen(op,op1,-1,trg); return trg;
     case 'N':
	     trg = gettemp(ITP);
         gen(op,op1,-1,trg); return trg;
	 case 'z':
	     qud = nextQ();
	     gen(op,op1,-1,-1); return qud;
	 case 'j':
	     qud = nextQ();
		 gen(op,-1,-1,-1); return qud;
     case 'I':
         trg = gettemp(ITP);
         gen(op,op1,-1,trg); return trg;
     case 'R':
         trg = gettemp(RTP);
         gen(op,op1,-1,trg); return trg;
     case '&':
	     trg = gettemp(ITP);
         gen(op,op1,-1,trg); return trg;
     default:  yyerror("bad qUo");
    }
}

int qPr(char op, int op1, Type t){
	if(tp(op1) != t){
        yyerror("print type is not correct!");
	}
    gen(op,-1,-1,op1); return -1;
}

void qAn( char op, int trg, int src){ /* src := trg */
    switch (op) {
     case '=':
	    if(ptr(trg)==0){
            if(tp(trg)==ITP && tp(src)==RTP) {  src = qUo('I', src); }
            if(tp(trg)==RTP && tp(src)==ITP) {  src = qUo('R', src); }
		}
            gen(op,src,-1,trg);
              break;
         default:  yyerror("bad qAn");

	}
}
          
void qHt( void){ gen('H',-1,-1,-1); }          
          
/*=======================================================================*/

int nextQ(){ return quadCt; }


void list_quads( void ){
    int i;  SymRec sR1,sR2,sR3;
    printf("\nIntrmediate code: %d\n\n",quadCt);
    for( i=0; i<quadCt; i++ ){
        printf( "%3d: (%c,%6d,%6d,%6d)   ", i, quad[i].op,quad[i].a1,
                                                    quad[i].a2,quad[i].a3);
        sR1 = tbl(quad[i].a1); sR2 = tbl(quad[i].a2); sR3 = tbl(quad[i].a3);
        switch( quad[i].op ){
           case '=':
               printf("%6s = %s\n",sR3.sym, sR1.sym); break;
           case '+': case '*': case '-': case '/': case '%': case '>': case '<': 
               printf("%6s = %s %c %s\n", sR3.sym, sR1.sym, quad[i].op, sR2.sym); break;
           case 'w': printf("    print(%s)\n",sR3.sym); break;
           case 'V': if(quad[i].a2 == -1){ printf("%6s = %d\n",sR3.sym,quad[i].a1); break; }
		             else { printf("%6s = %d*10**(%d-6)\n", sR3.sym, quad[i].a1, quad[i].a2);  break; }
	       case 'H': printf("   return\n"); break;
	       case 'M': printf("%6s = -%s\n",sR3.sym, sR1.sym); break;
		   case 'I': printf("%6s = %s\n",sR3.sym, sR1.sym); break;
		   case 'R': printf("%6s = %s\n",sR3.sym, sR1.sym); break;
		   case 'N': printf("%6s = not %s\n",sR3.sym, sR1.sym); break;
		   case 'E': printf("%6s = %s %s %s\n", sR3.sym, sR1.sym, "equal", sR2.sym); break;
		   case 'A': printf("%6s = %s %s %s\n", sR3.sym, sR1.sym, "and", sR2.sym); break;
		   case 'O': printf("%6s = %s %s %s\n", sR3.sym, sR1.sym, "or", sR2.sym); break;
		   case 'z': printf("IF %s false, goto quad %d\n", sR1.sym, quad[i].a3); break;
		   case 'j': printf("  E true, jump to quad %d\n", quad[i].a3); break;
		   case '@': if(quad[i].a1 == -1){ printf("    *%s = %s\n", sR3.sym, sR2.sym); break; } 
		             else { printf("%6s = *%s\n", sR3.sym, sR1.sym); break;}
		   case '&': printf("%6s = &%s\n",sR3.sym, sR1.sym); break;
		   case 'r': printf("   Read %s\n",sR3.sym); break;
           default:  printf("     ?  ?  ?\n");
        }
    }
}

