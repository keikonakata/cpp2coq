open Values
open Compt
open Pervasives_base

let ttt = V_Void
let tt = V_Bool true
let ff = V_Bool false

let size_of_stype sty =
  match sty with
  | A
  | Void
  | Loc(_)
  | Bool
  | Uchar
  | Ulong
  | Ulonglong
  | Char_s
  | Int
  | Long
  | Longlong
  | Nullptr -> assert false

let salloc sty_l sty_t st =
  let size = size_of_stype sty_l in
  let current_loc = st.index in
  assert false

let halloc sty_l sty_t st = assert false

let put sty_l sty_t lc v st = assert false
let get sty_l sty_t lc st = assert false

let salloc_with_init sty_l sty_t v st = assert false
let salloc_with_init_ sty_l v st = salloc_with_init sty_l Void v st

let call sty_l sty_t f st =
  match f st with
  | Coq_ret(v, st') ->  Coq_step(v, st')
  | Coq_step(v, st') -> Coq_step(V_Void, st')
  | _ -> assert false

let call_ f st = call Void Void f st

let add_Int_Int sty l r st =
  match l with
  | V_Int i ->
     begin
       match r with
       | V_Int j -> Coq_step(V_Int (i+j), st)
       | _ -> assert false
     end
  | _ -> assert false

let int_0 = V_Int 0
let int_1 = V_Int 1
