{
open Parser

let string_buffer = Buffer.create 80
exception Lex_error of string

let debugprint = ref false;;

let print str =  if !debugprint then print_endline str;;

}

let digit = ['0'-'9']
let literal = ['a'-'z'] ['A'-'Z' 'a'-'z' '_' '0'-'9' ':']*
let decorator = "//@" literal
let comment   = "//" [^ '@' '\n'] [^'\n']*  '\n'
let str = [^'@' '\n']
let include_sth = "#include"
let destructor = '~' literal
(* [' ' '\t']+ "<" [^' ' '\n' '\t'] ">" *)

rule token = parse
  | "class"     { print "class>>"; CLASS }
  | "struct"    { print "struct>>>"; STRUCT }
  | "typedef"   { print"typedef>>>"; TYPEDEF }
  | '('         { LRBRACE }
  | ')'         { RRBRACE }
  | ','         { COMMA }
  | ';'         { print "; -> SEMICOLON"; SEMICOLON }
  | '<'         { LBRACE }
  | '>'         { RBRACE }
  | '\"'        { QUOTE }
  | '&'         { REFERENCE }
  | destructor as d { DESTRUCTOR(d) }
  | "const"     { print "const"; CONST }
  | "public:"   { print "public"; PUBLIC } (* ignored by parser *)
  | "private:"  { PRIVATE } (* ignored by parser *)
  | include_sth as i { print i; INCLUDE(i) }
  | decorator [' ']* '\n' as d {
    print ("decorator-> "^d);
    DECORATOR( String.sub d 0 ((String.length d) - 1) )
  }
  | comment      { print"comment"; token lexbuf }
  | literal as s { print ("literal->"^s); LITERAL(s) }

  | '}'         { print"}<="; RBRACE2 }
  | '{'         { print"=>{"; LBRACE2 }
  | _           { token lexbuf }
  | eof		{ print "eof."; EOF }
      
and comment = parse
  | _ * '\n' { token lexbuf }
