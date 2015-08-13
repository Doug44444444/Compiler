       
/*  module to store and access the symbol table  */
#include  "symtab.h"
#include <stdio.h>
#include <string.h>
/*  
   Restricted access is given to the table by access functions declared
   in symtab.h
 */       

/*================= private globals ==================================*/
static SymRec sym_tbl[MXSYMS];  /* 0 .. last                          */
static int last = -17;          /* last entry in current table        */

/*================= initialization ===================================*/
void tbInit(){                  /* must be called first               */

    strcpy(sym_tbl[0].sym,"   ");  sym_tbl[0].tp = UTP;
    sym_tbl[0].kind=' ';
    sym_tbl[0].d=sym_tbl[0].ptr=sym_tbl[0].i1=sym_tbl[0].i2=0; 

    strcpy(sym_tbl[1].sym,"main"); sym_tbl[1].tp = UTP;
    sym_tbl[1].kind = 's';
    sym_tbl[1].d=sym_tbl[1].ptr=sym_tbl[1].i1=sym_tbl[1].i2=0; 

    last = 1; 
}

/*================ access routines ===================================*/    
SymRec tbl(int i){
   static SymRec dummySR; strcpy(dummySR.sym,"dummy"); dummySR.tp=UTP;
   if(i<0 || i>last) return dummySR;
   return sym_tbl[i];
}
int address(int i){ return sym_tbl[i].add; }
int ptr    (int i){ return sym_tbl[i].ptr; }
int depth  (int i){return sym_tbl[i].d;    }
int i1     (int i){ return sym_tbl[i].i1;  }
int i2     (int i){ return sym_tbl[i].i2;  }
char kind  (int i){return sym_tbl[i].kind; }
Type tp    (int i){return sym_tbl[i].tp;   }

/*=================== insertion routines =============================*/
int install( char *ident){
    int i = 0; int idx = -1;
    if (last==-17) yyerror("symtab not initialized");
    while(idx==-1){
        i = i+1;
        if( i>last ) break;
        if(!strcmp(ident,sym_tbl[i].sym))  idx = i;
    }
    if( idx != -1 ) return idx; /* ident was found in the table */
      
    last=last+1;     /* new entry */
    if(last>=MXSYMS) yyerror("sym table overflow");

	strncpy( sym_tbl[last].sym, ident, MXSLEN-1);
    sym_tbl[last].kind = 'v';         /* variable, by default         */
    sym_tbl[last].tp   = ITP;         /* integer type                 */
    sym_tbl[last].ptr  = 0;
    sym_tbl[last].d    = 0;           /* depth 0, until subprograms   */
	sym_tbl[last].i1   = 0;
	sym_tbl[last].i2   = 0;
    return last;
}
void setType(int i, Type t){
    switch(t){
       case UTP: sym_tbl[i].tp = UTP; break;
       case ITP: sym_tbl[i].tp = ITP; break;
	   case RTP: sym_tbl[i].tp = RTP; break;
	  default:  yyerror("bad SetType"); 
    }
}

void setArray(int i, int n){
    sym_tbl[i].kind = 'a';
    sym_tbl[i].i1 = n;
}

int isPointer(int v){
    if (ptr(v) != 0) { return 3; }
}

void typeTran(int p, int c) {
    if(c > 0){
	    if (tp(p) == ITP) { sym_tbl[c].tp = ITP; }
	    if (tp(p) == RTP) { sym_tbl[c].tp = RTP; }
    }
}

void SetPointer(){
    sym_tbl[last].ptr = sym_tbl[last].ptr + 1;
}

void isTypeDefine(char *ident,int n) {
    int i = 0; int idx = -1;
    if (last==-17) yyerror("symtab not initialized");
    while(idx==-1){
        i = i+1;
        if( i>last ) break;
        if(!strcmp(ident,sym_tbl[i].sym))  idx = i; 
    }
	switch(n){
	 case 1: 
            if( idx != -1 ) { yyerror("Variable has defined!"); }
            break;		
         case 2:
            if( idx == -1 ) { yyerror("Variable has not defined!"); }
	    break;
    }		
}

int gettemp( Type tp ){
    char str[7];
    if(last==-17) yyerror("symtab Not initialized");
    sprintf(str,"#%d", last+1 );

    last=last+1;
    if(last>=MXSYMS) yyerror("sym table overflow");

    strcpy( sym_tbl[last].sym, str);
    sym_tbl[last].kind = 'v';
    sym_tbl[last].tp   = tp;
	sym_tbl[last].ptr  = 0;
    sym_tbl[last].d    = 0;
	sym_tbl[last].i1   = 0;
	sym_tbl[last].i2   = 0;
    return last;
}

/*================ debug routines ====================================*/    
void list_table(){
    int i;
    printf("\n ==== Symbol Table ====\n");
	for(i=0; i<=last; i++){
        printf( "%3d:%4d %c %6s %5s%d %2d  %d,%d\n", 
               i, sym_tbl[i].add, sym_tbl[i].kind,sym_tbl[i].sym,txtTp[sym_tbl[i].tp], 
			   sym_tbl[i].ptr, sym_tbl[i].d, sym_tbl[i].i1,sym_tbl[i].i2);
    }
    printf("\n");
}

/*============== set addresses for whole structure ===================*/
void addAdd(){                /* begins back-end of compiler   */
    int i, j, k, m;  int nextA = 12;    /* room for flags, rtrnadd, rtrnvalueoffset  */
    for(i=0;i<=last;i++){
        if( tp(i)==UTP ){
       	    sym_tbl[i].add = -1;
        }
	else{ 
            if(i1(i)!=0){ 
		k = i1(i);				
    	        sym_tbl[i].add = nextA;
	        sym_tbl[i].i1 = nextA;
	        for(j = 0; j < k; j++){
	            if( tp(i)==RTP && ptr(i)==0 ){ 
                        nextA = nextA + RWID;
                    }
	            else {
                        nextA = nextA + IWID;
	            }
    	        }
		sym_tbl[i].i2 = nextA;
	    }
	    else{
                sym_tbl[i].add = nextA;			
	        if( tp(i)==RTP && ptr(i)==0 ){ 
                    nextA = nextA + RWID;
                }
	        else {
	            nextA = nextA + IWID;
	        }
	    }
	}
    }
    sym_tbl[1].i2 = nextA;         /* total size of main activation rec */
}

