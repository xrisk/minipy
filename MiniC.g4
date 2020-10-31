grammar MiniC;

prog : function_decl+ EOF ;

datatype_base : INT | FLOAT | VOID ;
datatype : datatype_base  ('[' expr ']')? ( '[' expr ']' )? ;


unary: NOT ;
binop_exp: EXP ;
binop1: MULT | DIV | MODULO ;
binop2: ADD | SUB ;
binop3: LT | GT | LEQ | NEQ | GEQ | EQ ;
binop4: AND | OR; 
array_literal_1d: '{' expr (',' expr)* '}';
array_literal_2d: '{' array_literal_1d (',' array_literal_1d)* '}';
array_literal: array_literal_1d | array_literal_2d;

bool_literal: 'True' | 'False' ;
char_literal: CHAR;

literal: INT_LITERAL | bool_literal | array_literal | char_literal ;

fn_call: IDENT '(' (expr (',' expr)*)? ')' ;

expr : 
    unary expr 
    |<assoc=right> expr binop_exp expr
    | expr binop1 expr 
    | expr binop2 expr
    | expr binop3 expr
    | expr binop4 expr
    | fn_call
    | literal
    | IDENT
    | IDENT '[' expr ']'
    | IDENT '[' expr ']' '[' expr ']'
    | expr 'if' expr 'else' expr
    | '(' expr ')'
     ;

block: '{' stmt+ '}' ;

if_stmt: 'if' expr  block ('elif' expr  block)* ('else' block )? ;
for_stmt: 'for' '(' simple_stmt ';' expr ';' simple_stmt ')' block ;
while_stmt: 'while' expr block ;


return_type : datatype;

function_decl: return_type IDENT '(' (datatype IDENT (',' datatype IDENT)*)? ')' block ;

return_stmt: 'return' expr ;

decl : datatype IDENT (ASSIGN expr)?  ;

assignment : (IDENT |
             IDENT '[' expr ']' | 
             IDENT '[' expr ']'  '[' expr ']' ) ASSIGN  expr;

simple_stmt: decl | assignment | fn_call | return_stmt;

stmt : simple_stmt EOL
    | if_stmt 
    | for_stmt
    | while_stmt ;


/*Tokens*/

INT: 'int32';
FLOAT: 'float';
NOT: 'not';
AND: 'and';
OR: 'or';
EXP: '**';
ADD: '+';
SUB: '-';
MULT: '*';
DIV: '/';
VOID: 'void';
LT: '<';
GT: '>';
LEQ: '<=';
GEQ: '>=';
NEQ: '!=';
EQ: '==';
MODULO: '%';
EOL: ';';
ASSIGN: '=' ;
CHAR: '\''[A-Za-z0-9 ]'\'';
IDENT: [a-zA-Z][a-zA-Z0-9_]*;
INT_LITERAL: [0-9A-Fa-f]+;
WS: [ \t\r\n]+ -> skip;


fragment SPACES
 : [ \t]+
 ;

