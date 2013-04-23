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

open Syntax

let indent_line n (ind, line) =
  match line with
  | "" -> (ind, line)
  | _ -> (ind + n, line)
;;

let indent_lines n =
  List.map (indent_line n)
;;

let rfind line pos ch =
  if String.rcontains_from line pos ch then
    Some (String.rindex_from line pos ch)
  else
    None

let rindex_split_pos line pos =
  let paren = rfind line pos '(' in
  let comma = rfind line pos ',' in
  match paren, comma with
  | Some p, Some c -> Some (max (p + 1) (c + 1))
  | Some p, None -> Some (p + 1)
  | None, Some c -> Some (c + 1)
  | None, None -> None
;;

let rec print_indent p (indent, line) =
  if line == "" then
    (* don't append spaces to a blank line *)
    p ""
  else
    let space = String.make (indent * 2) ' ' in
    let max_len = 80 - indent * 2 in
    let len = String.length line in
    if len > max_len then
      match rindex_split_pos line (max_len - 1) with
      | Some pos ->
        p (space ^ String.sub line 0 pos);
        print_indent p (indent, "    " ^ String.sub line pos (len - pos))
      | None ->
        p (space ^ line)
    else
      p (space ^ line)
;;

let print_lines p lines =
  let blank = ref false in
  List.iter (fun (indent, line) ->
    if not (line = "" && !blank) then
      print_indent p (indent, line);
    blank := line = ""
  ) lines
;;

let rec concat_blocks blocks =
  let rec insert_blank = function
    | [] -> []
    | x::[] -> [x]
    | x::xs -> x::[(0, "")]::(insert_blank xs) in
  List.concat (insert_blank blocks)
;;

let make_header_message source comment_out_head for_template =
 (*  let file = Filename.basename source in *)
  let first = comment_out_head ^ " This file is auto-generated from " ^ source in
  if for_template then
    [first; ""]
  else [
    first;
    comment_out_head ^ " *** DO NOT EDIT ***";
    ""
  ]
;;

let make_source_impl for_template conf source filename content comment_out_head =
  let path = Filename.concat conf.Config.outdir filename in
  let dir = Filename.dirname path in
  File_util.mkdir_all dir;
  File_util.safe_open_out path (fun out ->
    let print = (fun s -> output_string out s; output_char out '\n') in
    let head = make_header_message source comment_out_head for_template in
    List.iter print head;
    print_lines print content
  )
;;

let make_source = make_source_impl false
;;

let make_template_source = make_source_impl true
;;

let get_services idl =
  let services =
    List.fold_left (fun lst x ->
      match x with
      | Service s ->
        s::lst
      | _ -> lst) [] idl in
  List.rev services
;;
