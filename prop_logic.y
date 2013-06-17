%{
#include "global.h"
#include "PLResolution.h"
%}

%token END
%token LEFT_PAREN RIGHT_PAREN
%token BICOND
%token FORWARD_IMPLIC BACKWARD_IMPLIC
%token OR
%token AND
%token NOT
%token identifier

%left BICOND
%left BACKWARD_IMPLIC
%left FORWARD_IMPLIC
%left OR
%left AND
%left NOT

%start Input
%%
Input:
/* Empty */
| Input Line
;
Line:
END
| Sentence END
;
Sentence:
AtomicSentence
| ComplexSentence
;
AtomicSentence:
identifier {$$=MakeAtomicSentence($1);}
;
ComplexSentence:
LEFT_PAREN Sentence RIGHT_PAREN
| NOT Sentence {$$=MakeNotSentence($1);}
| Sentence AND Sentence {$$=MakeAndSentence($1, $2);}
| Sentence OR Sentence {$$=MakeOrSentence($1, $2);}
| Sentence FORWARD_IMPLIC Sentence {$$=MakeForwardIMPLIC($1, $2);}
| Sentence BACKWARD_IMPLIC Sentence {$$=MakeBackwardIMPLIC($1, $2);}
| Sentence BICOND Sentence {$$=MakeBICOND($1, $2);}
;
%%
int yyerror(char *s) {
    printf("%s\n",s);
}
int main(void) {
    yyparse()
}
