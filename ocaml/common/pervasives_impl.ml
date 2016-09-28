open Compt
open Pervasives_base

let add_Int_Int sty l r st =
  match l with
  | Int i ->
     begin
       match r with
       | Int j -> Coq_step(Int (i+j), st)
       | _ -> assert false
     end
  | _ -> assert false
