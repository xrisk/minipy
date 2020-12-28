grammar MiniC;

prog_top_level: external_decl | function_decl ;
prog : prog_top_level+ EOF ;

datatype_base : INT | FLOAT | VOID | BOOL;
datatype : datatype_base  ('[' expr ']')? ( '[' expr ']' )? ;


unary: NOT ;
binop_exp: EXP ;
binop1: DIV | MULT | MODULO ;
binop2: ADD | SUB ;
binop3: LT | GT | LEQ | NEQ | GEQ | EQ ;
binop4: AND | OR; 

array_literal: '{' expr (',' expr)* '}';

bool_literal: 'True' | 'False' ;
char_literal: CHAR;

literal: INT_LITERAL | bool_literal | array_literal | char_literal ;

fn_call: IDENT LPAREN (expr (',' expr)*)? RPAREN ;

assign_expr : loc ASSIGN expr ;

loc : IDENT
    | IDENT '[' expr ']' 
    | IDENT '[' expr ']' '[' expr ']' ;

expr : 
    unary expr 
    |<assoc=right> expr binop_exp expr
    | expr binop1 expr 
    | expr binop2 expr
    | expr binop3 expr
    | expr binop4 expr
    | <assoc=right> assign_expr
    | fn_call
    | literal
    | loc
    | expr 'if' expr 'else' expr
    | LPAREN expr RPAREN
     ;

block: '{' stmt* '}' ;

elif_block: ELIF expr block ;
else_block: ELSE block ;

if_stmt: IF expr block 
          elif_block*
          else_block? ;

for_init: simple_stmt ;
for_cond: expr ;
for_update: simple_stmt ;
for_stmt: 'for' LPAREN for_init? ';' for_cond? ';' for_update? RPAREN block ;
while_stmt: 'while' expr block ;


return_type : datatype;
arg: datatype IDENT;

function_decl: return_type IDENT LPAREN (arg (',' arg)*)? RPAREN  block ;
external_decl: EXTERN return_type IDENT LPAREN (arg (',' arg)*)? RPAREN EOL;

return_stmt: 'return' expr? ;

decl : datatype IDENT (ASSIGN expr)?  ;

assignment : (IDENT |
             IDENT '[' expr ']' | 
             IDENT '[' expr ']'  '[' expr ']' ) ASSIGN  expr;

simple_stmt: decl | expr | return_stmt;

stmt : simple_stmt EOL
    | if_stmt 
    | for_stmt
    | while_stmt ;


/*Tokens*/

EXTERN: 'extern';
INT: 'int32';
FLOAT: 'float';
BOOL: 'bool';
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

IF: 'if' ;
ELIF: 'elif';
ELSE: 'else';

CHAR: '\''[A-Za-z0-9 ]'\'';
IDENT: [a-zA-Z][a-zA-Z0-9_]*;
INT_LITERAL: [0-9A-Fa-f]+;
WS: [ \t\r\n]+ -> skip;
LPAREN: '(' ;
RPAREN: ')' ;

fragment SPACES
 : [ \t]+
 ;

