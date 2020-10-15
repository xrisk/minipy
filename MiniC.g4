grammar MiniC;

tokens { INDENT, DEDENT }

@lexer::members {
  // A queue where extra tokens are pushed on (see the NEWLINE lexer rule).
  private java.util.LinkedList<Token> tokens = new java.util.LinkedList<>();
  // The stack that keeps track of the indentation level.
  private java.util.Stack<Integer> indents = new java.util.Stack<>();
  // The amount of opened braces, brackets and parenthesis.
  private int opened = 0;
  // The most recently produced token.
  private Token lastToken = null;
  @Override
  public void emit(Token t) {
    super.setToken(t);
    tokens.offer(t);
  }

  @Override
  public Token nextToken() {
    // Check if the end-of-file is ahead and there are still some DEDENTS expected.
    if (_input.LA(1) == EOF && !this.indents.isEmpty()) {
      // Remove any trailing EOF tokens from our buffer.
      for (int i = tokens.size() - 1; i >= 0; i--) {
        if (tokens.get(i).getType() == EOF) {
          tokens.remove(i);
        }
      }

      // First emit an extra line break that serves as the end of the statement.
      this.emit(commonToken(MiniCParser.NEWLINE, "\n"));

      // Now emit as much DEDENT tokens as needed.
      while (!indents.isEmpty()) {
        this.emit(createDedent());
        indents.pop();
      }

      // Put the EOF back on the token stream.
      this.emit(commonToken(MiniCParser.EOF, "<EOF>"));
    }

    Token next = super.nextToken();

    if (next.getChannel() == Token.DEFAULT_CHANNEL) {
      // Keep track of the last token on the default channel.
      this.lastToken = next;
    }

    return tokens.isEmpty() ? next : tokens.poll();
  }

  private Token createDedent() {
    CommonToken dedent = commonToken(MiniCParser.DEDENT, "");
    dedent.setLine(this.lastToken.getLine());
    return dedent;
  }

  private CommonToken commonToken(int type, String text) {
    int stop = this.getCharIndex() - 1;
    int start = text.isEmpty() ? stop : stop - text.length() + 1;
    return new CommonToken(this._tokenFactorySourcePair, type, DEFAULT_TOKEN_CHANNEL, start, stop);
  }

  // Calculates the indentation of the provided spaces, taking the
  // following rules into account:
  //
  // "Tabs are replaced (from left to right) by one to eight spaces
  //  such that the total number of characters up to and including
  //  the replacement is a multiple of eight [...]"
  //
  //  -- https://docs.python.org/3.1/reference/lexical_analysis.html#indentation
  static int getIndentationCount(String spaces) {
    int count = 0;
    for (char ch : spaces.toCharArray()) {
      switch (ch) {
        case '\t':
          count += 8 - (count % 8);
          break;
        default:
          // A normal space char.
          count++;
      }
    }

    return count;
  }

  boolean atStartOfInput() {
    return super.getCharPositionInLine() == 0 && super.getLine() == 1;
  }
}


prog : stmt+ EOF ;

datatype_base : 'uint32' | 'uint64' | 'int32' | 'int64' | 'int8' | 'uint8' | 'bool';
datatype : datatype_base
         | datatype_base '[' expr']'
         | datatype_base '[' expr ']' '[' expr ']'
         ;


unary: 'not';
binop_exp: '**' ;
binop1: '*' | '/' | '%' ;
binop2: '+' | '-';
binop3: '<' | '>' | '<=' | '!=' | '>=' | '==' ;
binop4: 'and' | 'or';
assignment_op: '=' ;

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

block: NEWLINE INDENT stmt+ DEDENT;

if_stmt: 'if' expr ':' block ('elif' expr ':' block)* ('else' ':' block )? ;
for_stmt: 'for' '(' simple_stmt ';' expr ';' simple_stmt ')' ':' block ;
while_stmt: 'while' expr ':' block ;

function_decl: 'def' IDENT '(' datatype IDENT (',' datatype IDENT)* ')' '->' (datatype | 'None') ':' block ;

return_stmt: 'return' expr ;

decl : datatype IDENT (assignment_op expr)? | 
       datatype '[' expr ']' IDENT (assignment_op expr)? |
       datatype '[' expr ']' '[' expr ']' IDENT (assignment_op expr)? ;

assignment : (IDENT |
             IDENT '[' expr ']' | 
             IDENT '[' expr ']'  '[' expr ']' ) assignment_op expr;

simple_stmt: decl | assignment | fn_call | return_stmt;

stmt : simple_stmt NEWLINE
    | if_stmt 
    | for_stmt
    | while_stmt 
    | function_decl
    ;


/*Tokens*/

CHAR: '\''[A-Za-z0-9 ]'\'';
IDENT: [a-zA-Z][a-zA-Z0-9_]*;
INT_LITERAL: [0-9A-Fa-f]+;
WS: ' '->skip;

fragment SPACES
 : [ \t]+
 ;

NEWLINE
 : ( {atStartOfInput()}?   SPACES
   | ( '\r'? '\n' | '\r' | '\f' ) SPACES?
   )
   {
     String newLine = getText().replaceAll("[^\r\n\f]+", "");
     String spaces = getText().replaceAll("[\r\n\f]+", "");

     // Strip newlines inside open clauses except if we are near EOF. We keep NEWLINEs near EOF to
     // satisfy the final newline needed by the single_put rule used by the REPL.
     int next = _input.LA(1);
     int nextnext = _input.LA(2);
     if (opened > 0 || (nextnext != -1 && (next == '\r' || next == '\n' || next == '\f' || next == '#'))) {
       // If we're inside a list or on a blank line, ignore all indents,
       // dedents and line breaks.
       skip();
     }
     else {
       emit(commonToken(NEWLINE, newLine));
       int indent = getIndentationCount(spaces);
       int previous = indents.isEmpty() ? 0 : indents.peek();
       if (indent == previous) {
         // skip indents of the same size as the present indent-size
         skip();
       }
       else if (indent > previous) {
         indents.push(indent);
         emit(commonToken(MiniCParser.INDENT, spaces));
       }
       else {
         // Possibly emit more than 1 DEDENT token.
         while(!indents.isEmpty() && indents.peek() > indent) {
           this.emit(createDedent());
           indents.pop();
         }
       }
     }
   }
 ;
