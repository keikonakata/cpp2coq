open Pervasives_base

let print_Loc lc =
  match lc with
  | Some _lc -> print_int _lc
  | None -> print_endline "None"

let print_value v =
  match v with
  | V_Bool(Some b) ->
     if b then print_endline "true" else print_endline "false"
  | V_Int(Some i) -> print_int i
  | V_Loc lc -> print_Loc lc
  | V_Void -> print_endline "void"
  | _ -> assert false

let print_Avl_node_base lc st =
  match st.space.(lc) with
  | Val v ->
     begin
       match v with
       | V_Avl_node_base nd -> assert false
       | _ -> assert false
     end
  | _ -> assert false
