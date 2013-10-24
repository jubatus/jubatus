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

exception Illegal_character of (Lexing.position * char)

let buffer = Buffer.create 256

let reset_buffer () =
  Buffer.clear buffer

let push_char c =
  Buffer.add_char buffer c

let get_buffer () =
  Buffer.contents buffer

let convert_backslash_char = function
  | '\\' -> '\\'
  | '\'' -> '\''
  | '\"' -> '\"'
  | 'n' -> '\n'
  | 'r' -> '\r'
  | 'b' -> '\b'
  | 't' -> '\t'
  | c -> c
}

let digit = ['0'-'9']*
let literal = (['a'-'z']['a'-'z' '0'-'9']*) ('_' ['a'-'z' '0'-'9']+)*
let decorator = "#@" literal
let comment   = "#" [^'@'] [^'\n']* '\n'
(* let include_sth = "#include" *)
let newline = "\n"
let space = [' ' '\t']

rule token = parse
  | "%include" { INCLUDE }
  | "exception" { EXCEPTION }
  | "message" { MESSAGE }
  | "enum" { ENUM }
  | "service" { SERVICE }
  | literal as s { LITERAL(s) }
  | ','       { COMMA }
  | '<'       { LBRACE }
  | '>'       { RBRACE }
  | '}'       { RBRACE2 }
  | '{'       { LBRACE2 }
  | '('       { LPAREN }
  | ')'       { RPAREN }
  | '?'       { QUESTION }
  | '='       { DEFINE }
  | "::"      { COLON_COLON }
  | ':'       { COLON }
  | decorator as d { DECORATOR(d) }
  | digit as s { INT( int_of_string s ) }
  | '"' {
    reset_buffer ();
    string lexbuf;
    STRING(get_buffer ()) }

(*  | include_sth as i { INCLUDE(i) } *)

  | comment   { Lexing.new_line lexbuf; token lexbuf }
  | newline   { Lexing.new_line lexbuf; token lexbuf }
  | eof       { EOF }
  | space+    { token lexbuf }
  | _         {
    let ch = Lexing.lexeme_char lexbuf 0 in
    raise (Illegal_character(Lexing.lexeme_start_p lexbuf, ch)) }

and string = parse
    | '"' 
        { () }
    | '\\' ( ['\\' '\'' '"' 'n' 't' 'b' 'r'] as c )
        { push_char (convert_backslash_char c);
          string lexbuf}
    | _ as c
        { push_char c;
          string lexbuf }
