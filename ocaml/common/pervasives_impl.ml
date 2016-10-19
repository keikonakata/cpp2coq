open Values
open Compt
open Pervasives_base

let ttt = V_Void
let tt = V_Bool(Some true)
let ff = V_Bool(Some false)

let init sty =
  match sty with
  | Avl_node_base ->
     V_Avl_node_base
       { _child = V_Array (Array.make 2 (init_V_Loc ()));
       _parent = init_V_Loc ();
       _depth = init_V_Uchar () }
  | Loc _ -> init_V_Loc ()
  | Int -> init_V_Int ()
  | Bool -> init_V_Bool ()
  | _ -> assert false

let size_of_stype sty =
  match sty with
  | A
  | Avl_node_base -> 25
  | Void -> assert false
  | Loc _ -> 8
  | Bool -> 1
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
    st.space.(index_old + i) <- Ptr index_old
  done;

  Coq_step(V_Loc(Some index_old), st)

let salloc' sty st = salloc sty Void st

let halloc sty_l sty_t st = salloc sty_l sty_t st

let _put lc off v st =
  match st.space.(lc) with
  | Val v_old ->
     begin
       match v_old with
       | V_Avl_node_base nd ->
          begin
            match off with
            | 0 ->
               begin
                 match v with
                 | V_Loc _ ->
                    begin
                      match nd._child with
                      | V_Array arr -> arr.(0) <- v
                      | _ -> assert false
                    end
                 | _ -> assert false
               end
            | 8 ->
               begin
                 match v with
                 | V_Loc _ ->
                    begin
                      match nd._child with
                      | V_Array arr -> arr.(1) <- v
                      | _ -> assert false
                    end
                 | _ -> assert false
               end
            | 16 ->
               begin
                 match v with
                 | V_Loc _ -> nd._parent <- v
                 | _ -> assert false
               end
            | 24 ->
               begin
                 match v with
                 | V_Uchar _ -> nd._depth <- v
                 | _ -> assert false
               end
            | _ -> assert false
          end
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
       | V_Bool _ ->
          begin
            match v with
            | V_Bool _ -> st.space.(lc) <- Val v
            | _ -> assert false
          end
       | _ -> assert false
     end
  | _ -> assert false

let _get sty lc off st =
  match st.space.(lc) with
  | Val v ->
     begin
       match v with
       | V_Avl_node_base nd ->
          begin
            match off with
            | 0 ->
               begin
                 match sty with
                 | Loc _ ->
                    begin
                      match nd._child with
                      | V_Array arr -> arr.(0)
                      | _ -> assert false
                    end
                 | _ -> assert false
               end
            | 8 ->
               begin
                 match sty with
                 | Loc _ ->
                    begin
                      match nd._child with
                      | V_Array arr -> arr.(1)
                      | _ -> assert false
                    end
                 | _ -> assert false
               end
            | 16 ->
               begin
                 match sty with
                 | Loc _ -> nd._parent
                 | _ -> assert false
               end
            | 24 ->
               begin
                 match sty with
                 | Uchar _ -> nd._depth
                 | _ -> assert false
               end
            | _ -> assert false
          end
       | V_Int _ ->
          begin
            match sty with
            | Int _ -> v
            | _ -> assert false
          end
       | V_Loc _ ->
          begin
            match sty with
            | Loc _ -> v
            | _ -> assert false
          end
       | V_Bool _ ->
          begin
            match sty with
            | Bool -> v
            | _ -> assert false
          end
       | _ -> assert false
     end
  | _ -> assert false

let norm lc st =
  match lc with
  | V_Loc (Some _lc) ->
     begin
       match st.space.(_lc) with
       | Val _ -> (_lc, 0)
       | Ptr _lc' -> (_lc', _lc - _lc')
       | _ -> assert false
     end
  | _ -> assert false

let put sty_l sty_t lc v st =
  let (_lc, off) = norm lc st in
  _put _lc off v st;
  Coq_step(v, st)

let get sty_l sty_t lc st =
  let (_lc, off) = norm lc st in
  Coq_step(_get sty_l _lc off st, st)

let salloc_with_init sty_l sty_t v st =
  let Coq_step(lc, st) = salloc sty_l sty_t st in
  put sty_l sty_t lc v st;
  Coq_step(lc, st)

let salloc_with_init_ sty_l v st = salloc_with_init sty_l Void v st

let call sty_l sty_t f st =
  match f st with
  | Coq_ret(v, st') ->  Coq_step(v, st')
  | Coq_step(v, st') -> Coq_step(V_Void, st')
  | _ -> assert false

let call' f st = call Void Void f st

(**********)
(** Expr **)
(**********)

let array_subscript sty_i sty_l sty_t arr idx st =
  let size = size_of_stype sty_l in
  match arr with
  | V_Loc(Some _arr) ->
     begin
       match idx with
       | V_Int(Some _idx) -> Coq_step(mk_V_Loc (_arr + size * _idx), st)
       | V_Bool(Some b) ->
          if b then Coq_step(mk_V_Loc (_arr + size), st)
          else Coq_step(mk_V_Loc _arr, st)
       | _ -> assert false
     end
  | _ -> assert false

let ife sty_l sty_t c t f st =
  match c st with
  | Coq_ret _ as r -> r
  | Coq_step(v, st') ->
     begin
       match v with
       | V_Bool (Some b) ->
          if b then t st else f st
       | _ -> assert false
     end
  | _ -> assert false

(*********************)
(** Cast Operations **)
(*********************)

let pointer_to_boolean sty v =
  match v with
  | V_Loc (Some _v) ->
     if _v != 0 then mk_V_Bool true else mk_V_Bool false
  | V_Nullptr -> mk_V_Bool false
  | _ -> assert false

let integral_cast sty_src sty_trg sty_t v st =
  match v with
  | V_Int i ->
     begin
       match sty_trg with
       | Uchar -> Coq_step(V_Uchar i, st)
       | _ -> assert false
     end
  | _ -> assert false

let integral_to_boolean sty v =
  match v with
  | V_Int (Some i) ->
     if i = 0 then mk_V_Bool false else mk_V_Bool true
  | _ -> assert false

let array_to_pointer_decay sty v = v

let null_to_pointer sty_s sty_t v =
  match v with
  | V_Int(Some 0) -> V_Loc(Some 0)
  | _ -> assert false

(***********************)
(** Binary Operations **)
(***********************)

let gt_Int_Int sty l r st =
  match l with
  | V_Int (Some i) ->
     begin
       match r with
       | V_Int (Some j) -> Coq_step(mk_V_Bool (i < j), st)
       | _ -> assert false
     end
  | _ -> assert false

let add_Int_Int sty l r st =
  match l with
  | V_Int(Some i) ->
     begin
       match r with
       | V_Int(Some j) -> Coq_step(mk_V_Int (i+j), st)
       | _ -> assert false
     end
  | _ -> assert false

let int_0 = mk_V_Int 0
let int_1 = mk_V_Int 1

(**************************)
(** Temporal definitions **)
(**************************)

let assign_LocAvl_node_base_LocAvl_node_base sty_t lc v st =
  put Void Void lc v st;
  Coq_step(lc, st)
