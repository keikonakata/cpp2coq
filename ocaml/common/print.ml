open Pervasives_base

let print_value v =
  match v with
  | Int i -> print_int i
  | _ -> assert false
