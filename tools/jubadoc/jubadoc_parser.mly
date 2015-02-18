/*
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
*/

%{
open Printf
open Stree

let _ = Parsing.set_trace false;;
let debugprint = ref false;;

let print str = if !debugprint then print_endline str;;

let parse_error s = print ("parse_error->"^s);;

let doc_lines = [];;

%}

%token TYPEDEF
%token MESSAGE
%token EXCEPTION
%token SERVICE

%token RRBRACE LRBRACE COMMA COLON
%token RBRACE LBRACE RBRACE2 LBRACE2
%token QUESTION
%token ENUM
%token EOF DEFINE

%token <int>    INT
/* TODO: %token <double> DOUBLE <float> FLOAT <bool> true, false, <string> "..." */
%token <string> LITERAL
%token <string> INCLUDE
%token <string> DECORATOR
%token <string> DOC_LINE

%start input
%type <Stree.stree list> input

%%

input: 
     | input exp0 { print "input exp0"; $2::$1 }
     | exp0           { print ">>newline"; [$1] }
;
   
exp0:
     | doclines exp0 {
       match $2 with
	 | Typedef(a,b,_) -> Typedef(a,b,$1);
	 | Enum(a,b,_) -> Enum(a,b,$1);
	 | Message(a,b,_) -> Message(a,b,$1);
	 | Exception(a,b,c,_) -> Exception(a,b,c,$1)
	 | Service(a,b,_) -> Service(a,b,$1)
     }
     | typedef { $1 }
     | enum    { $1 }
     | msg     { $1 }
     | ex      { $1 }
     | service { $1 }
;

doclines:
        | DOC_LINE { [$1] }
	| DOC_LINE doclines { $1::$2 }
;

typedef:
	| TYPEDEF LITERAL DEFINE a_type{
	  let _ = Stree.add_known_types $2 in Typedef($2, $4, [])
	}
;

a_type:
	| a_type QUESTION {
	  Nullable($1)
	}
	| LITERAL {
	  print (">anytype: (LITERAL) " ^ $1);
	  match $1 with
	    | "void" -> Void;
	    | "object" -> Object;
	    | "bool"   -> Bool;
	    | "byte"   -> Byte;
	    | "short"  -> Short;
	    | "int" -> Int;
	    | "long" -> Long;
	    | "ubyte" -> Ubyte;
	    | "ushort" -> Ushort;
	    | "uint"   -> Uint;
	    | "ulong"  -> Ulong;
	    | "float"  -> Float;
 	    | "double" -> Double
	    | "raw"    -> Raw;
	    | "string" -> String;
	    | s when Stree.check_type s ->
	      Struct(s);
	    | s ->
	      print ("unknown type: " ^ s);
	      raise (Stree.Unknown_type s)
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
	      raise (Stree.Unknown_type s)
	}
;

types:
	| a_type COMMA types { $1::$3 }
	| a_type { [$1] }

enum:
	| ENUM LITERAL LBRACE2 numbers RBRACE2 {
	  Stree.add_known_types $2; Enum($2, $4, [])
	}
numbers:
	| INT COLON LITERAL {
	  [($1, $3)]
	}
	| INT COLON LITERAL numbers {
	  ($1, $3)::$4
	}

/* msg and ex: we shoudl implement type-param... (TODO) */
msg:
	| MESSAGE LITERAL LBRACE2 fields RBRACE2 {
	  Stree.add_known_types $2;
	  Message($2, $4, [])
	}
ex:
	| EXCEPTION LITERAL LBRACE2 fields RBRACE2 {
	  Exception($2, $4, "", [])
	}
	| EXCEPTION LITERAL LBRACE LITERAL LBRACE2 fields RBRACE2 {
	  Exception($2, $6, $4, [])
	}

fields:
	| field fields { $1::$2 }
	| field { [$1] }
;
field:
	| doclines field {
	  let Field(a,b,c,_) = $2 in Field(a,b,c,$1)
	}
	| INT COLON a_type LITERAL {
	  Field($1, $3, $4, [])
	} /* TODO: implement default value */
;

service: /* TODO: implement version and new RPC-spec */
	| SERVICE LITERAL LBRACE2 api_defs RBRACE2 {
	  print (" service > "^$2);
	  Service($2, $4, [])
	}
api_defs:
	| api_def { [$1] }
	| api_def api_defs { $1::$2 }
;
api_def:
	| doclines api_def {
	  let Method(a,b,c,d,_) = $2 in Method(a,b,c,d,$1)
	}
	| decorators a_type LITERAL LRBRACE RRBRACE
	    { print $3; Method($2, $3, [], $1, []) }
	| decorators a_type LITERAL LRBRACE cfields RRBRACE
	    { print $3; Method($2, $3, $5, $1, []) }
	| a_type LITERAL LRBRACE RRBRACE
	    { print $2; Method($1, $2, [], [], []) }
	| a_type LITERAL LRBRACE cfields RRBRACE
	    { print $2; Method($1, $2, $4, [], []) }

decorators:
	| DECORATOR
	    { print $1; [ (Stree.make_decorator $1) ] }
	| DECORATOR decorators
	    { print $1; (Stree.make_decorator $1)::$2 }
	| DECORATOR LRBRACE INT RRBRACE
	    { print $1; [ (Stree.make_decorator_with_int $1 $3) ] }
	| DECORATOR LRBRACE INT RRBRACE decorators
	    { print $1; (Stree.make_decorator_with_int $1 $3)::$5 }

/* comma separated fields */
cfields:
	| field COMMA cfields { $1::$3 }
	| field { [$1] }

/* TODO: include "hoge.rdl"
	| INCLUDE LBRACE LITERAL RBRACE   {
	  print ("ignoring inclusion " ^ $3); Nothing
	}
*/
;

%%
