(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License version 2.1 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*)

{
open Jubadoc_parser

exception Lex_error of string

let debugprint = ref false;;

let print str =  if !debugprint then print_endline str;;

}

let digit = ['0'-'9']*
let literal = ['a'-'z'] ['A'-'Z' 'a'-'z' '_' '0'-'9']*
let decorator = "#@" literal
let doc_line  = "#-" [^'\n']* '\n'
let comment   = "#" [^'@' '-'] [^'\n']* '\n'
(* let include_sth = "#include" *)
let newline = "\n"

rule token = parse
  | "exception" { print "exception>"; EXCEPTION }
  | "message" { print "message>"; MESSAGE }
  | "type" { print "typedef>"; TYPEDEF }
  | "enum" { print "enum>"   ; ENUM }
  | "service" { print "service>"; SERVICE }
  | literal as s { print ("literal->"^s); LITERAL(s) }
  | ','       { COMMA }
  | '<'       { LBRACE }
  | '>'       { RBRACE }
  | '}'       { print"}<="; RBRACE2 }
  | '{'       { print"=>{"; LBRACE2 }
  | '('       { LRBRACE }
  | ')'       { RRBRACE }
  | '?'       { QUESTION }
  | '='       { DEFINE }
  | ':'       { COLON }
  | decorator as d { DECORATOR(d) }
  | digit as s { print s; INT( int_of_string s ) }

(*  | include_sth as i { print i; INCLUDE(i) } *)
  | doc_line as s { Lexing.new_line lexbuf; print "docline"; DOC_LINE(s) }
  | comment     { Lexing.new_line lexbuf; print "comment"; token lexbuf }
  | '\n'        { Lexing.new_line lexbuf; token lexbuf }
  | eof		{ print "eof."; EOF }
  | _   { token lexbuf }
