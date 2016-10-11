open Pervasives_base

let print_value v =
  match v with
  | V_Int (Some i) -> print_int i
  | _ -> assert false
