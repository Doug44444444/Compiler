%{
#include <stdio.h>
#include "y.tab.h"

/* this file produces a function int lex(void) when the lex program is run
   It also produces two external variables which are shared:
     1)  the return value is the type of token. It is either
       a) a value less than 256, which is to be interpreted as an ascii code
       b) a named constant, (e.g. ASSIGN) defined in Yacc and y.tab.h
     2) string yytext is shared, and holds the text of the token matched by lex
     3) yyin is the file read by lex, and is opened in yacc.
 */  
   
%}

main      main
int       int
double    double
printf    printf
pria      "==>%d\\n"
prib      "==>%f\\n"
scanf     scanf
scaa      "%d"
while     while
if        if
else      else
ge        ">="
le        "<="
eq        "=="
ne        "!="
and       "&&"
or        "||"
quote     \"
comma     ","
letter    [A-Za-z]
digit     [0-9]
num       {digit}({digit})*
real      {num}?("."{num})+
ident     ({letter}|\_)({letter}|{digit}|\_)*
string	  [A-Za-z]?\"(\\.|[^\\"\n]|\\\n)*\%d\"
op        "="|"+"|"-"|"*"|"/"|"%"|"("|")"|";"|"{"|"}"|">"|"<"|"!"|"@"|"&"|"["|"]"
ws        [ \t\n\r]
other     .

%%

{int}     {  return qq(INT);          } 
{main}    {  return qq(MAIN);         }
{double}  {  return qq(DOUBLE);       }
{printf}  {  return qq(PRINTF);       }
{pria}    {  return qq(PRIA);         }
{prib}    {  return qq(PRIB);         }
{scanf}   {  return qq(SCANF);        }
{scaa}    {  return qq(SCAA);         } 
{comma}   {  return qq(COMMA);        } 
{quote}   {  return qq(QUOTE);        } 
{while}   {  return qq(WHILE);        }
{if}      {  return qq(IF);           }
{else}    {  return qq(ELSE);         }
{ge}      {  return qq(GE);           }
{le}      {  return qq(LE);           }
{eq}      {  return qq(EQ);           }
{ne}      {  return qq(NE);           }
{and}     {  return qq(AND);          }
{or}      {  return qq(OR);           }
{num}     {  return qq(NUM);          }
{real}    {  return qq(REAL);         }
{ws}      {  /* note, no return */ ;  }
{ident}   {  return qq(ID);           }
{string}  {  return qq(STRING);       } 
{op}      {  return qq( yytext[0] );  }
{other}   {  return qq( '?' );        }

%%
int qq(int tokn){
    static FILE * f = 0;
    if(f == 0)   f = fopen("lex.out","w");
    if( tokn == ID )  fprintf(f,"~");
    if( tokn < 256 )  fprintf(f,"%c ",tokn);
    else              fprintf(f,"%s ",yytext);
    return tokn;
} 
