open Values
open Compt
open Pervasives_base

let ttt = V_Void
let tt = V_Bool true
let ff = V_Bool false

let init sty =
  match sty with
  | Loc _ -> V_Loc None
  | Int -> V_Int None
  | _ -> assert false

let size_of_stype sty =
  match sty with
  | A
  | Void -> assert false
  | Loc _ -> 8
  | Bool
  | Uchar
  | Ulong
  | Ulonglong
  | Char_s -> assert false
  | Int -> 8
  | Long
  | Longlong
  | Nullptr -> assert false

let salloc sty_l sty_t st =
  let size = size_of_stype sty_l in
  let current_loc = st.index in

  let index_old = st.index in
  st.index <- index_old + size;

  st.space.(index_old) <- Val(init sty_l);
  for i = 1 to size - 1 do
    st.space.(index_old + 1) <- Ptr index_old
  done;

  Coq_step(V_Loc(Some index_old), st)

let halloc sty_l sty_t st = salloc sty_l sty_t st

let _put lc v st =
  match st.space.(lc) with
  | Val v_old ->
     begin
       match v_old with
       | V_Int _ ->
          begin
            match v with
            | V_Int _ -> st.space.(lc) <- Val v
            | _ -> assert false
          end
       | V_Loc _ ->
          begin
            match v with
            | V_Loc _ -> st.space.(lc) <- Val v
            | V_Nullptr -> st.space.(lc) <- Val v
            | _ -> assert false
          end
       | _ -> assert false
     end
  | _ -> assert false

let put sty_l sty_t lc v st =
  begin
    match lc with
    | V_Loc (Some lc) -> _put lc v st
    | _ -> assert false
  end;
  Coq_step(v, st)

let get sty_l sty_t lc st = assert false

let salloc_with_init sty_l sty_t v st =
  let Coq_step(lc, st) = salloc sty_l sty_t st in
  put sty_l sty_t lc v st

let salloc_with_init_ sty_l v st = salloc_with_init sty_l Void v st

let call sty_l sty_t f st =
  match f st with
  | Coq_ret(v, st') ->  Coq_step(v, st')
  | Coq_step(v, st') -> Coq_step(V_Void, st')
  | _ -> assert false

let call_ f st = call Void Void f st

let add_Int_Int sty l r st =
  match l with
  | V_Int(Some i) ->
     begin
       match r with
       | V_Int(Some j) -> Coq_step(V_Int(Some (i+j)), st)
       | _ -> assert false
     end
  | _ -> assert false

let int_0 = V_Int(Some 0)
let int_1 = V_Int(Some 1)
