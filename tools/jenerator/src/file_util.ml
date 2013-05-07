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

let safe_open_out filename f =
  let out = open_out filename in
  try
    f out
  with e ->
    close_out out;
    raise e
;;

let output_line out s =
  output_string out s;
  output_char out '\n'
;;

let take_base path =
  let file = Filename.basename path in
  if String.contains file '.' then
    Filename.chop_extension file
  else
    file
;;

let file_exist path =
  try
    Unix.access path [Unix.F_OK];
    true
  with Unix.Unix_error(_, _, _) ->
    false
;;

let rec split_path path =
  if path = "." or path ="/" then
    [path]
  else
    let dir = Filename.dirname path in
    let base = Filename.basename path in
    split_path dir @ [base]
;;

let mkdir_all path =
  let paths = split_path path in
  let current = ref "" in
  List.iter (fun p ->
    current := Filename.concat !current p;
    if not (file_exist !current) then
      Unix.mkdir !current  0o755
  ) paths
;;
