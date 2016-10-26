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
       _depth = init_V_Uchar ();
       _value = init_V_Uint () }
  | Avl_node_basePolicy ->
     V_Avl_node_basePolicy
  | Uchar -> init_V_Uchar ()
  | Uint -> init_V_Uint ()
  | Loc _ -> init_V_Loc ()
  | Int -> init_V_Int ()
  | Bool -> init_V_Bool ()
  | _ -> assert false

let size_of_stype sty =
  match sty with
  | A -> assert false
  | Avl_node_base -> 40
  | Avl_node_basePolicy -> 1
  | Void -> assert false
  | Loc _ -> 8
  | Bool -> 1
  | Uchar -> 1
  | Uint -> 8
  | Ulong -> 8
  | Ulonglong -> 8
  | Char_s -> assert false
  | Int -> 8
  | Long -> 8
  | Longlong -> 8
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
            | 32 ->
               begin
                 match v with
                 | V_Uint _ -> nd._value <- v
                 | _ -> assert false
               end
            | _ -> assert false
          end
       | V_Uchar _ ->
          begin
            match v with
            | V_Uchar _ -> st.space.(lc) <- Val v
            | _ -> assert false
          end
       | V_Uint _ ->
          begin
            match v with
            | V_Uint _ -> st.space.(lc) <- Val v
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
            | 32 ->
               begin
                 match sty with
                 | Uint _ -> nd._value
                 | _ -> assert false
               end
            | o -> print_int o; assert false
          end
       | V_Uchar _ ->
          begin
            match sty with
            | Uchar _ -> v
            | _ -> assert false
          end
       | V_Uint _ ->
          begin
            match sty with
            | Uint _ -> v
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
(** Stmt **)
(**********)

let do_if sty cond ifthen ifelse st =
  match cond st with
  | Coq_step(b, st) ->
     begin
       match b with
       | V_Bool(Some _b) -> if _b then ifthen st else ifelse st
       | _ -> assert false
     end
  | rest -> rest

let do_for sty_inc sty_body sty_t cond inc body st =
  let rec do_for_rec st =
    match cond st with
    | Coq_step(V_Bool(Some _b), st) ->
       if _b then
         match body st with
         | Coq_step(_, st) ->
            begin
              match inc st with
              | Coq_step(_, st) -> do_for_rec st
              | _ -> assert false
            end
         | Coq_ret _ as r -> r
         | Coq_continue st ->
            begin
              match inc st with
              | Coq_step(_, st) -> do_for_rec st
              | _ -> assert false
            end
         | Coq_break st -> Coq_step(V_Void, st)
         | Coq_error _ -> assert false
       else Coq_step(V_Void, st)
    | _ -> assert false in
  do_for_rec st

let do_while sty_body sty_t cond body st =
  let rec do_while_rec st =
    match cond st with
    | Coq_step(b, st) ->
       begin
         match b with
         | V_Bool(Some _b) ->
            if _b then
              match body st with
              | Coq_ret _ as r -> r
              | Coq_step(_, st) -> do_while_rec st
              | Coq_continue st -> do_while_rec st
              | Coq_break st -> Coq_step(V_Void, st)
              | Coq_error _ -> assert false
            else Coq_step(V_Void, st)
         | _ -> assert false
       end
    | _ -> assert false in
  do_while_rec st

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

(* Dependent *)
(* BitCast *)
(* LValueBitCast *)
(* LValueToRValue *)
(* NoOp *)
(* BaseToDerived *)
(* DerivedToBase *)
(* UncheckedDerivedToBase *)
(* Dynamic *)
(* ToUnion *)
(* ArrayToPointerDecay *)
let array_to_pointer_decay sty v = v

(* FunctionToPointerDecay *)
(* NullToPointer *)
let null_to_pointer sty_s sty_t v =
  match v with
  | V_Int(Some 0) -> V_Loc(Some 0)
  | _ -> assert false

(* NullToMemberPointer *)
(* BaseToDerivedMemberPointer *)
(* DerivedToBaseMemberPointer *)
(* MemberPointerToBoolean *)
(* ReinterpretMemberPointer *)
(* UserDefinedConversion *)
(* ConstructorConversion *)
(* IntegralToPointer *)
(* PointerToIntegral *)
(* PointerToBoolean *)
let pointer_to_boolean sty v =
  match v with
  | V_Loc (Some _v) ->
     if _v != 0 then mk_V_Bool true else mk_V_Bool false
  | V_Nullptr -> mk_V_Bool false
  | _ -> assert false

(* ToVoid *)
(* VectorSplat *)
(* IntegralCast *)
let integral_cast sty_src sty_trg sty_t v st =
  match v with
  | V_Bool (Some _b) ->
     begin
       match sty_trg with
       | Int ->
          if _b then Coq_step(mk_V_Int 1, st) else Coq_step(mk_V_Int 0, st)
       | Uint -> assert false
       | _ -> assert false
     end
  | V_Int i ->
     begin
       match sty_trg with
       | Uchar -> Coq_step(V_Uchar i, st)
       | Uint -> Coq_step(V_Uint i, st)
       | Int -> Coq_step(V_Int i, st)
       | _ -> assert false
     end
  | V_Uchar i ->
     begin
       match sty_trg with
       | Int -> Coq_step(V_Int i, st)
       | _ -> assert false
     end
  | V_Uint i -> assert false
  | V_Loc lc -> assert false
  | _ -> assert false

(* IntegralToBoolean *)
let integral_to_boolean sty v =
  match v with
  | V_Int (Some i) ->
     if i = 0 then mk_V_Bool false else mk_V_Bool true
  | _ -> assert false

(* IntegralToFloating *)
(* FloatingToIntegral *)
(* FloatingToBoolean *)
(* BooleanToSignedIntegral *)
(* FloatingCast *)
(* CPointerToObjCPointerCast *)
(* BlockPointerToObjCPointerCast *)
(* AnyPointerToBlockPointerCast *)
(* ObjCObjectLValueCast *)
(* FloatingRealToComplex *)
(* FloatingComplexToReal *)
(* FloatingComplexToBoolean *)
(* FloatingComplexCast *)
(* FloatingComplexToIntegralComplex *)
(* IntegralRealToComplex *)
(* IntegralComplexToReal *)
(* IntegralComplexToBoolean *)
(* IntegralComplexCast *)
(* IntegralComplexToFloatingComplex *)
(* ARCProduceObject *)
(* ARCConsumeObject *)
(* ARCReclaimReturnedObject *)
(* ARCExtendBlockObject *)
(* AtomicToNonAtomic *)
(* NonAtomicToAtomic *)
(* CopyAndAutoreleaseBlockObject *)
(* BuiltinFnToFnPtr *)
(* ZeroToOCLEvent *)
(* AddressSpaceConversion *)
(* IntToOCLSampler *)

(***********************)
(** Binary Operations **)
(***********************)

(* ptrmemd *)
(* ptrmemi *)
(* mul *)
(* div *)
(* rem *)

(* Add *)
let add_Int_Int sty l r st =
  match l with
  | V_Int(Some i) ->
     begin
       match r with
       | V_Int(Some j) -> Coq_step(mk_V_Int (i+j), st)
       | _ -> assert false
     end
  | _ -> assert false

(* Sub *)
let sub_Int_Int sty_t l r st =
  match l, r with
  | V_Int(Some _l), V_Int(Some _r) ->
     Coq_step(mk_V_Int (_l - _r), st)
  | _ -> assert false

(* Shl *)
(* Shr *)

(* LT *)
let lt_Int_Int sty_t l r st =
  match l, r with
  | V_Int(Some _l), V_Int(Some _r) ->
     Coq_step(mk_V_Bool (_l < _r), st)
  | _ -> assert false

let lt_Uint_Uint sty_t l r st =
  match l, r with
  | V_Uint(Some _l), V_Uint(Some _r) ->
     Coq_step(mk_V_Bool (_l < _r), st)
  | _ -> assert false

(* GT *)
let gt_Int_Int sty l r st =
  match l, r with
  | V_Int (Some _l), V_Int (Some _r) ->
     Coq_step(mk_V_Bool (_l > _r), st)
  | _ -> assert false

(* LE *)
let le_Int_Int sty_t i1 i2 st =
  match i1, i2 with
  | V_Int(Some _i1), V_Int(Some _i2) ->
     Coq_step(mk_V_Bool (_i1 <= _i2), st)
  | _ -> assert false

let le_Uint_Uint sty_t i1 i2 st =
  match i1, i2 with
  | V_Uint(Some _i1), V_Uint(Some _i2) ->
     Coq_step(mk_V_Bool (_i1 <= _i2), st)
  | _ -> assert false


(* GE *)
let ge_Int_Int sty_t i1 i2 st =
  match i1, i2 with
  | V_Int(Some _i1), V_Int(Some _i2) ->
     Coq_step(mk_V_Bool (_i1 >= _i2), st)
  | _ -> assert false

let ge_Uint_Uint sty_t i1 i2 st =
  match i1, i2 with
  | V_Uint(Some _i1), V_Uint(Some _i2) ->
     Coq_step(mk_V_Bool (_i1 >= _i2), st)
  | _ -> assert false

(* EQ *)

let cmp_LocAvl_node_base_LocAvl_node_base lc1 lc2 =
  match lc1, lc2 with
  | V_Loc(Some _lc1), V_Loc(Some _lc2) -> _lc1 = _lc2
  | V_Loc(Some _lc1), V_Nullptr -> _lc1 = 0
  | V_Nullptr, V_Loc(Some _lc2) -> 0 = _lc2
  | V_Nullptr, V_Nullptr -> true
  | _ -> assert false

let eq_LocAvl_node_base_LocAvl_node_base sty_t lc1 lc2 st =
  Coq_step(mk_V_Bool (cmp_LocAvl_node_base_LocAvl_node_base lc1 lc2), st)

let eq_Int_Int sty_t i1 i2 st =
  let b =
    match i1, i2 with
    | V_Int(Some _i1), V_Int(Some _i2) -> _i1 = _i2
    | _ -> assert false in
  Coq_step(mk_V_Bool b, st)

let eq_Uint_Uint sty_t i1 i2 st =
  let b =
    match i1, i2 with
    | V_Uint(Some _i1), V_Uint(Some _i2) -> _i1 = _i2
    | _ -> assert false in
  Coq_step(mk_V_Bool b, st)

(* ne *)
let ne_LocAvl_node_base_LocAvl_node_base sty_t lc1 lc2 st =
  Coq_step(mk_V_Bool (not(cmp_LocAvl_node_base_LocAvl_node_base lc1 lc2)), st)


let ne_Int_Int sty_t l r st =
  match l, r with
  | V_Int(Some _l), V_Int(Some _r) ->
     Coq_step(mk_V_Bool (_l != _r), st)
  | _ -> assert false

(* and *)
(* xor *)
(* or *)
(* land *)
let land_Bool_Bool sty_t l r st =
  match l st with
  | Coq_step(V_Bool(Some _l), st) ->
     if _l then r st else Coq_step(mk_V_Bool false, st)
  | _ -> assert false

(* lor *)

(* assign *)
let assign_Bool_Bool sty_t lc v st =
  put Bool sty_t lc v st;
  Coq_step(lc, st)

let assign_LocAvl_node_base_LocAvl_node_base sty_t lc v st =
  put (Loc Avl_node_base) sty_t lc v st;
  Coq_step(lc, st)

let assign_Uchar_Uchar sty_t lc v st =
  put Uchar sty_t lc v st;
  Coq_step(lc, st)
(* MulAssign *)
(* DivAssign *)
(* RemAssign *)
(* AddAssign *)
(* SubAssign *)

(* ShlAssign *)
(* ShrAssign *)
(* AndAssign *)
(* XorAssign *)
(* OrAssign *)
(* Comma *)

(***********************)
(** Unary Operations **)
(***********************)

(* PostInc *)
let postinc_Int sty_t lc st =
  match get Int sty_t lc st with
  | Coq_step(V_Int(Some _i), st) ->
     let new_value = mk_V_Int (_i + 1) in
     put Int sty_t lc new_value st;
     Coq_step(new_value, st)
  | _ -> assert false

(* PostDec *)
(* PreInc *)
(* PreDec *)
(* AddrOf *)
let addrof_Avl_node_base sty_t v st = Coq_step(v, st)

(* Deref *)
(* Plus *)
(* Minus *)
let minus_Int sty_t i st =
  match i with
  | V_Int(Some _i) -> Coq_step(mk_V_Int (- _i), st)
  | _ -> assert false

(* Not *)
(* LNot *)
let lnot_Bool sty_t b st =
  match b with
  | V_Bool(Some _b) -> Coq_step(mk_V_Bool (not _b), st)
  | _ -> assert false

(* Real *)
(* Imag *)
(* Extension *)
(* Coawait *)

(***************)
(** Constants **)
(***************)

let int_0 = mk_V_Int 0
let int_1 = mk_V_Int 1
let int_2 = mk_V_Int 2
let int_3 = mk_V_Int 3
let int_4 = mk_V_Int 4
let int_5 = mk_V_Int 5


