(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

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
