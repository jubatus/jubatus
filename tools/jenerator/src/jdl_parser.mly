/*
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
*/

%{

open Printf
open Syntax

let _ = Parsing.set_trace false;;
let debugprint = ref false;;

let print str = if !debugprint then print_endline str;;

let parse_error s = print ("parse_error->"^s);;

%}

%token TYPEDEF
%token MESSAGE
%token EXCEPTION
%token SERVICE

%token RPAREN LPAREN COMMA COLON
%token RBRACE LBRACE RBRACE2 LBRACE2
%token QUESTION
%token ENUM
%token EOF DEFINE

%token <int>    INT
/* TODO: %token <double> DOUBLE <float> FLOAT <bool> true, false, <string> "..." */
%token <string> LITERAL
%token <string> INCLUDE
%token <string> DECORATOR

%start input
%type <Syntax.idl> input

%%

input:
 | exp0 input { $1::$2 }
 | exp0  { [$1] }
;

exp0:
 | typedef { $1 }
 | enum    { $1 }
 | msg     { $1 }
 | ex      { $1 }
 | service { $1 }

typedef:
 | TYPEDEF LITERAL DEFINE a_type {
   Typedef($2, $4) }
;

a_type:
 | a_type QUESTION {
   Nullable($1)
 }
 | LITERAL {
   match $1 with
   | "object" -> Object
   | "bool"   -> Bool
   | "byte"   -> Int(true, 1)
   | "short"  -> Int(true, 2)
   | "int"    -> Int(true, 4)
   | "long"   -> Int(true, 8)
   | "ubyte"  -> Int(false, 1)
   | "ushort" -> Int(false, 2)
   | "uint"   -> Int(false, 4)
   | "ulong"  -> Int(false, 8)
   | "float"  -> Float false
   | "double" -> Float true
   | "raw"    -> Raw
   | "string" -> String
   | s -> Struct(s)
 }
 | LITERAL LBRACE types RBRACE {
   match $1 with
   | "list" -> List( (List.hd $3) );
   | "map"  ->
     let left = List.hd $3 in
     let right = List.hd (List.tl $3) in
     Map(left, right);
   | "tuple" -> Tuple($3);
   (* user defined types?   hoge<hage, int> *)
   | s ->
     print ("unknown container: " ^ s);
     raise (Syntax.Unknown_type s)
 }
;

ret_type:
 | a_type { match $1 with
	      | Struct("void") -> None
	      | _ -> Some ($1) }   /* a bit ad-hoc : tsushima */


types:
 | a_type COMMA types { $1::$3 }
 | a_type { [$1] }
;

enum:
 | ENUM LITERAL LBRACE2 numbers RBRACE2 {
   Enum { enum_name = $2;
          enum_values = $4; }
 }
;

numbers:
 | INT COLON LITERAL {
   [($1, $3)]
 }
 | INT COLON LITERAL numbers {
   ($1, $3)::$4
 }
;

msg:
 | MESSAGE LITERAL LBRACE2 fields RBRACE2 {
   Message { message_name = $2;
             message_fields = $4; }
 }
;

ex:
 | EXCEPTION LITERAL LBRACE2 fields RBRACE2 {
   Exception { exception_name = $2;
               exception_fields = $4;
               exception_super = None; }
 }
 | EXCEPTION LITERAL LBRACE LITERAL LBRACE2 fields RBRACE2 {
   Exception { exception_name = $2;
               exception_fields = $6;
               exception_super = Some $4; }
 }
;

fields:
 | field fields { $1::$2 }
 | field { [$1] }
;

field:
 | INT COLON a_type LITERAL {
   { field_number = $1; field_type = $3; field_name = $4; }
 }
;
// default value is not yet implemented | INT COLON a_type LITERAL DEFINE INT {
//  ($1, $3, $4, )
//}
;

// TODO: implement version and new RPC-spec
service:
 | SERVICE LITERAL LBRACE2 api_defs RBRACE2 {
   Service { service_name = $2;
             service_methods = $4; }
 }
;

api_defs:
 | api_def { [$1] }
 | api_def api_defs { $1::$2 }
;
api_def:
 | decorators ret_type LITERAL cfields
     { { method_return_type = $2;
         method_name = $3;
         method_arguments = $4;
         method_decorators = $1; }
     }
 | ret_type LITERAL cfields
     { { method_return_type = $1;
         method_name = $2;
         method_arguments = $3;
         method_decorators = []; }
     }
;

decorators:
 | DECORATOR
     { [ (Syntax.make_decorator $1) ] }
 | DECORATOR decorators
     { (Syntax.make_decorator $1)::$2 }
 | DECORATOR LPAREN INT RPAREN
     { [ (Syntax.make_decorator_with_int $1 $3) ] }
 | DECORATOR LPAREN INT RPAREN decorators
     { (Syntax.make_decorator_with_int $1 $3)::$5 }

// comma separated fields
cfields:
 | LPAREN RPAREN { [] }
 | LPAREN field cfields_rest { $2::$3 }
;
cfields_rest:
 | COMMA field cfields_rest { $2::$3 }
 | RPAREN { [] }

/* TODO: include "hoge.rdl"
 | INCLUDE LBRACE LITERAL RBRACE   {
   print ("ignoring inclusion " ^ $3); Nothing
 }
*/
;

%%
