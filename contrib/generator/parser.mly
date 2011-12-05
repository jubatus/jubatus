
%{
open Printf

let _ = Parsing.set_trace false;;
let debugprint = ref false;;
let classname  = ref "";;

let set_classname str = classname := str;;
let print str = if !debugprint then print_endline str;;

let includee = Hashtbl.create 20;;
(* let _ = Hashtbl.replace includee "" "";; *)

let parse_error s = print ("parse_error->"^s);;

%}

%token VOID INT CHAR
%token CONST
%token NAMESPACE_SEPARATOR
%token RRBRACE LRBRACE COMMA SEMICOLON
%token RBRACE LBRACE
%token RBRACE2 LBRACE2
%token EOF COMMENT
%token QUOTE
%token CLASS STRUCT CONST
%token PUBLIC PRIVATE
%token REFERENCE
%token TYPEDEF
%token <string> LITERAL
%token <string> DECORATOR
%token <string> CODE
%token <string> INCLUDE
%token <string> DESTRUCTOR

%start input
%type <(Stree.type_def list * Stree.struct_def list * Stree.class_def list)> input

%%

input: 
          exp0 {
	    match $1 with
	      | `Typedef(t) -> ([t], [], []);
	      | `Class(c) -> ([], [], [c]);
	      | `Struct(s) -> ([], [s], []);
	      | `Nothing -> ([], [], []);
	  }
        | input exp0		{
	  let (typedefs, structs, classes) = $1 in
	  match $2 with
	    | `Typedef(t) -> ((t::typedefs), structs, classes);
	    | `Class(c) -> (typedefs, structs, (c :: classes));
	    | `Struct(s) -> (typedefs, (s::structs), classes);
	    | `Nothing -> $1
	}
;

exp0:
	| typedef    { print ">typedef"; `Typedef($1) } /* FIXME: currently ignored */
	| one_class  { print ">one_class"; `Class($1) }
	| one_struct { print "> struct";   `Struct($1) }
	| INCLUDE LBRACE LITERAL RBRACE   { print ("ignoring inclusion " ^ $3); `Nothing }
;

typedef: TYPEDEF anytype LITERAL SEMICOLON { Stree.TypeDef($2, $3) }

one_struct:
        STRUCT LITERAL LBRACE2 exp SEMICOLON {
	  print ">STRUCT LITERAL LBRACE2 exp SEMICOLON"; 
	  set_classname $2;
	  let (_,members) = $4 in
	  Stree.StructDef($2, members)
	}
;

one_class:
	CLASS LITERAL LBRACE2 exp SEMICOLON {
	  print ">CLASS LITERAL LBRACE2 exp SEMICOLON"; 
	  set_classname $2;
	  let (funcs,members) = $4 in
	  Stree.ClassDef($2, funcs, members)
	}
;
exp:
	  PUBLIC exp { $2 }
        | PRIVATE exp { $2 }
        | declaration     { print ">declaration"; ([$1], []) }
        | declaration exp { print ">declaration exp"; let (funs,mems) = $2 in ( $1 :: funs, mems ) }
        | member          { ([], [$1]) }
        | member exp      { let (funs,mems) = $2 in ( funs , $1 :: mems) }
	| RBRACE2         { ([], []) }
;

member:   anytype LITERAL SEMICOLON   { ($1, $2) }
;

declaration:
	| DECORATOR declaration
	    { print ">dec1"; let (t,n,arg,decs,b) = $2 in (t,n,arg, $1::decs,b) }

	  /* methods */
	| anytype LITERAL LRBRACE RRBRACE CONST SEMICOLON
	    { print ">meth1"; ($1, $2, [], [],true) }
	| anytype LITERAL LRBRACE anytypes RRBRACE CONST SEMICOLON
	    { print ">meth2"; ($1, $2, (List.rev $4), [],true) }
	| anytype LITERAL LRBRACE RRBRACE SEMICOLON
	    { print ">meth3"; ($1, $2, [], [],false) }
	| anytype LITERAL LRBRACE anytypes RRBRACE  SEMICOLON
	    { print ">meth4"; ($1, $2, (List.rev $4), [],false) }
;

anytypes:anytype                  { print ">anytypes"; [$1] }
	|anytype COMMA anytypes   { $1 :: $3 }
	|anytype LITERAL { [$1] }
	|anytype LITERAL COMMA anytypes { $1 :: $4 }
;

anytype: CONST anytype                  { print ">const "; $2 }
	|LITERAL                        { print ">anytype: LITERAL"; Stree.make_anytype $1 false}
	|LITERAL REFERENCE              { print ">anytype: LITERAL"; Stree.make_anytype $1 true}
	|LITERAL LBRACE anytypes RBRACE { Stree.make_template $1 $3 false}
	|LITERAL LBRACE anytypes RBRACE REFERENCE { Stree.make_template $1 $3 true}
;

%%
