(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and
 Telephone Corporation.

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
open Jdl_parser

exception Lex_error of string
}

let digit = ['0'-'9']*
let literal = ['a'-'z'] ['A'-'Z' 'a'-'z' '_' '0'-'9']*
let decorator = "#@" literal
let comment   = "#" [^'@'] [^'\n']* '\n'
(* let include_sth = "#include" *)
let newline = "\n"

rule token = parse
  | "exception" { EXCEPTION }
  | "message" { MESSAGE }
  | "type" { TYPEDEF }
  | "enum" { ENUM }
  | "service" { SERVICE }
  | literal as s { LITERAL(s) }
  | ','       { COMMA }
  | '<'       { LBRACE }
  | '>'       { RBRACE }
  | '}'       { RBRACE2 }
  | '{'       { LBRACE2 }
  | '('       { LRBRACE }
  | ')'       { RRBRACE }
  | '?'       { QUESTION }
  | '='       { DEFINE }
  | ':'       { COLON }
  | decorator as d { DECORATOR(d) }
  | digit as s { INT( int_of_string s ) }

(*  | include_sth as i { INCLUDE(i) } *)

  | comment   { token lexbuf }
  | '\n'      { token lexbuf }
  | eof	      { EOF }
  | _         { token lexbuf }
