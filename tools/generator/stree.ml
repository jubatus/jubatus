(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.

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

type decl_type = [ `Void | `Object | `Bool
		 | `Byte | `Short  | `Int | `Long
		 | `Ubyte | `Ushort | `Uint | `Ulong
		 | `Float | `Double 
		 | `Raw   | `String 
		 | `Struct of string
		 | `List of decl_type
		 | `Map of decl_type * decl_type
		 | `Tuple of decl_type list
		 | `Nullable of decl_type
(*		 | `Null *)
		 ]
    
type field_type = [`Field of int * decl_type * string ]
type method_type = [`Method of decl_type * string * (field_type list) * (string list)]

type stree = [ `Typedef of string * decl_type
	     | `Enum of string * (int * string) list
	     | `Message of string * field_type list
	     | `Exception of string * field_type list * string
	     | `Service of string * method_type list
	     ]

let known_types = Hashtbl.create 256;;

let _ =
  List.iter (fun s -> Hashtbl.add known_types s 0) [
    "int"; (*"void"; "char"; "double" *)
  ];;

exception Unknown_type of string

let add_known_types name =
  Hashtbl.add known_types name 0;;

let check_type name =
  Hashtbl.mem known_types name;;
