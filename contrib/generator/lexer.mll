{
open Parser

let string_buffer = Buffer.create 80
exception Lex_error of string

let debugprint = ref false;;

let print str =  if !debugprint then print_endline str;;

let depth = ref 0;;

}

let digit = ['0'-'9']
let literal = ['a'-'z'] ['A'-'Z' 'a'-'z' '_' '0'-'9' ':']*
let decorator = "//@" literal
let comment   = "//" [^ '@' '\n'] [^'\n']*  '\n'
let str = [^'@' '\n']
let include_sth = "#include"
(* [' ' '\t']+ "<" [^' ' '\n' '\t'] ">" *)

rule token = parse
  | "class"     { CLASS }
  | '('         { LRBRACE }
  | ')'         { RRBRACE }
  | ','         { COMMA }
  | ';'         { print "; -> SEMICOLON"; SEMICOLON }
  | '<'         { LBRACE }
  | '>'         { RBRACE }
  | '\"'         { QUOTE }
  | "public:"   { PUBLIC } (* ignored by parser *)
  | "private:"  { PRIVATE } (* ignored by parser *)
  | include_sth as i { print i; INCLUDE(i) }
  | decorator [' ']* '\n' as d {
    print ("d-> "^d);
    DECORATOR( String.sub d 0 ((String.length d) - 1) )
  }
  | comment     { token lexbuf }
  | literal as s { print ("s->"^s); LITERAL(s) }

  | '}'         {
(*    print_int !depth; *)
    if !depth = 1 then begin
      print "}<= ";
      RBRACE2
    end else begin
      RBRACE2
    end
  }

  | '{'         {
    print_int !depth;
    incr depth;
    if !depth = 1 then begin
      print "=>{";
      LBRACE2
    end else begin
      Buffer.reset string_buffer;
      Buffer.add_char string_buffer '{';
      code 0 lexbuf;
      let s = Buffer.contents string_buffer in
      Buffer.reset string_buffer;
      print ("->"^s^"<-");
      CODE s
    end
  }

  | _           { token lexbuf }
  | eof		{ print "eof."; EOF }

and code d = parse
  | '{' { Buffer.add_char string_buffer '{';  code (d+1) lexbuf }
  | '}' {
    decr depth;
    Buffer.add_char string_buffer '}'; 
    if d = 0 then begin
      (); (* print_endline ("code: " ^ (Buffer.contents string_buffer)); (); *)
    end else
      code (d-1) lexbuf
  }
  | eof { raise  (Lex_error "unterminated code") }
  | _   { Buffer.add_char string_buffer (Lexing.lexeme_char lexbuf 0);
	  code d lexbuf }
      
and comment = parse
  | _ * '\n' { token lexbuf }

