%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE int
extern  char * yytext; /* this is defined in lex and holds most 
recent token */
extern FILE * yyin;    /* this is defined in lex; lex reads from this file   */
%}

%token  ID INT MAIN WHILE IF ELSE PRINTF PRIA PRIB QUOTE COMMA DOUBLE SCANF SCAA SCAB
%token  GE LE EQ NE AND OR
%token  NUM REAL NUMA STRING

%right '='
%left OR 
%left AND 
%left GE LE EQ NE '>' '<' 
%left '+' '-'
%left '*' '/' '%'
%left '(' ')'

%right UMINUS UPLUS '!' 

%%

Progr     : INT MAIN '(' ')' CpdS                                  { setType($$, UTP);  qHt();           }
CpdS      : '{' Dclar SList '}'                                    {}
Dclar     : Type Varlist ';' Dclar                                 {}
          |                                                        {}
	        ;
Type      : INT                                                    {  setType($$,ITP);  }
          | DOUBLE                                                 {  setType($$,RTP);  }
          ;
Varlist   : Var                                                    { typeTran($0, $1);  }
          | Varlist COMMA Var                                      { typeTran($0, $3);  }
	        ;
Var       : '*' Var                                                { typeTran($0, $2); SetPointer(); }
          | IDs Arraylist                                          { typeTran($0, $1);   }
       	  | IDs                                                    { typeTran($0, $1);   }
          ;
IDs       : ID                                                     { isTypeDefine(yytext,1);  $$ = install(yytext); typeTran($0, $1); }
          ;
Arraylist : '[' Numa ']'                                           { setArray($0, $2);}
          | Arraylist '[' Numa ']'                                 {}
          ;
SList     : Stmt                                                   {}
          | Stmt  SList                                            {}
          ;
Stmt      : Exprlist ';'                                           { $$ = $1;                                } 
          | IF '(' Expr ')' M Stmt                                 { qbackPatch($5, nextQ());                }
          | IF '(' Expr ')' M Stmt ELSE M1 Stmt                    { qbackPatch($8, nextQ());                }
          | WHILE M2 '(' Expr ')' M Stmt                           { loop('j', $2); qbackPatch($6, nextQ()); }
          | PRINTF '(' QUOTE PRIA QUOTE COMMA Expr ')' ';'         { qPr('w', $7, ITP);                      }
      	  | PRINTF '(' QUOTE PRIB QUOTE COMMA Expr ')' ';'         { qPr('w', $7, RTP);                      }
          | ';'                                                    {}
       	  | CpdS                                                   {}
          | Assn ';'                                               {}
          | SCANF '(' STRING COMMA '&' Id ')' ';'                  {  qSf($6);                  }  
          ;
Exprlist  : Expr                                                   {}
          | '*' Exprlist                                           { $$ = qOp('@', $2, -1);     }
      	  | '&' Exprlist                                           { $$ = qUo('&', $2);         }
       	  | Id '[' Expr ']'                                        { $$ = qOp('@', qCa($1, $3), -1);          }
      	  | '(' Id '+' Expr ')'                                        { $$ = qCa($2, $4); }
      	  | '(' Id '-' Expr ')'                                        { $$ = qCa($2, $4); }  
      	  ;
Assn      : Id '=' Exprlist                                        { $$ = $1; qAn('=', $1, $3); }
	        | Id '[' Expr ']''=' Expr                                { qSa($1, $3, $6);           }
          | '*' Exprlist '=' Exprlist                              { qPo('@', -1, $4, $2);      }
	        ;
Expr      : Expr '+' Expr             { $$ = qOp('+', $1, $3);      }
          | Expr '-' Expr             { $$ = qOp('-', $1, $3);      }
          | Expr '*' Expr             { $$ = qOp('*', $1, $3);      }
          | Expr '/' Expr             { $$ = qOp('/', $1, $3);      }
          | Expr '%' Expr             { $$ = qOp('%', $1, $3);      }
          | Expr '>' Expr             { $$ = qOp('>', $1, $3);      }
          | Expr '<' Expr             { $$ = qOp('<', $1, $3);      }
	        | Expr EQ  Expr             { $$ = qOp('E', $1, $3);      }
          | Expr GE  Expr             { $$ = qOp('O', qOp('>', $1, $3), qOp('E', $1, $3)); }
          | Expr LE  Expr             { $$ = qOp('O', qOp('<', $1, $3), qOp('E', $1, $3)); }
          | Expr NE  Expr             { $$ = qUo('N', qOp('E', $1, $3));                   }
      	  | Expr AND M Expr           { qbackPatch($3, nextQ()); $$ = qOp('A', $1, $4);    }
	        | Expr OR M3 Expr           { qbackPatch($3, nextQ()); $$ = qOp('O', $1, $4);    }
          | '!' Expr                  { $$ = qUo('N', $2);          }
          | '(' Expr ')'              { $$ = $2;                    }
          | '-' Expr %prec UMINUS     { $$ = qUo('M', $2);          }
          | '+' Expr %prec UPLUS      { $$ = $2;                    }
          | Num                       { $$ = $1;          }
          | Id                        { $$ = $1;          }
          ;
Num       : NUM                       {  $$ = qOp('V', val(yytext), -1);                       }
          | REAL                      {  $$ = qOp('V', valReal(yytext,1), valReal(yytext,2));  }
          ;
Numa      : NUM                       { $$ = val(yytext);                                      }
          ;
Id        : ID                        { isTypeDefine(yytext,2); $$ = install(yytext);          }
          ;
M         : ;                         { $$ = qUo('z', $-1);                           }
M1        : ;                         { $$ = qUo('j', -1);  qbackPatch($-2, nextQ()); }
M2        : ;                         { $$ = nextQ();                                 }
M3        : ;                         { $$ = qUo('z', qUo('N', $-1));                 }


%%

#include "symtab.h"
#include "quad.h"
#include "obj.h"
int     val(char *s); /* return  val from dig s */
int valReal(char * s, int n);

main(int argc, char* *argv){
    char ch;
    if(argc != 2) {printf("useage:  pas filename \n"); exit(1);}
    if( !(yyin = fopen(argv[1],"r")) ){ 
           printf("cannot open file\n");exit(1);
     }
    printf("pre parse\n");
    tbInit();
    yyparse();         /* the function created by yacc */
    printf("done parse\n");
    addAdd(0);         /* calculate mac addresses & put in symtable */
    printf("done addadd\n");
    list_quads();      /* debugging output */
    list_table(0);     /* debugging output */
    print_object();    /* output yacc.obj, the machine code */
}

void yyerror(char *mesg){
    printf("!!%s\n", mesg);
    list_quads();
    list_table(0);
    printf("!!%s\n", mesg);
    exit(1); /* stop at first error: lex.out will show where in src*/
}

int val(char * s){
    int ans;
    sscanf(s,"%d",&ans); 
    return ans;
}

int valReal(char * s, int n){
    double val; int mant; int expo = 0;
	sscanf(s, "%lf", &val);
	if(val == 0.0){ mant = 0, expo = 0; return; }
	while(val < 0.1) {expo--; val = val*10.0;}
	while(val >= 1.0) {expo++; val = val/10.0;}
	mant = 1000000 * val;
        switch(n){
        case 1: return mant;
	    case 2: return expo;
        }
}

