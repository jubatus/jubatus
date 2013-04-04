(*
 Jubatus: Online machine learning framework for distributed environment
 Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and
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

type t = {
  language: string;
  outdir: string;
  internal: bool;
  namespace: string;
  default_template: bool;
  include_guard: string;
}

let usage =
  "usage: jenerator -l <lang> [-o <out>] [-i] [-n <namespace>] [-t] [-g <includeguard>] idl ...\n"
;;

let languages = [ "server"; "cpp"; "python" ; (*"java"; *)"ruby"; ]
;;

let exit_with_message specs msg =
  Arg.usage specs (msg ^ "\n" ^ usage);
  exit 1

let parse_args () =
  let language = ref None in
  let outdir = ref "." in
  let internal = ref false in
  let namespace = ref "" in
  let default_template = ref false in
  let include_guard = ref "" in

  let specs = [
    ("-l", Arg.Symbol(languages, (fun s -> language := Some s)),
     "Language");
    ("-o", Arg.Set_string outdir,
     "Output directory (default: \".\")");
    ("-i", Arg.Set internal,
     "Internal include (generate #include with relative path)");
    ("-n", Arg.Set_string namespace,
     "Namespace");
    ("-t", Arg.Set default_template,
     "Output default template xxx_serv files");
    ("-g", Arg.Set_string include_guard,
     "Prefix of include guard (used in C++)");
  ] in
  let rest = ref [] in
  Arg.parse specs (fun arg -> rest := arg::!rest) usage;

  match !language with
  | None ->
    exit_with_message specs "Please specify language with -l option"
  | Some lang ->
    if !rest = [] then
      exit_with_message specs "No input files"
    else
      let conf = {
        language = lang;
        outdir = !outdir;
        namespace = !namespace;
        internal = !internal;
        default_template = !default_template;
        include_guard = !include_guard;
      } in
      (conf, !rest)
;;
