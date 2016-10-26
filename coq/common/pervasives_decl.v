Require Import mathcomp.ssreflect.ssreflect.
Require Import common.values.
Require Import common.compt.

Parameter ttt : value Void.
Extract Constant ttt => "Pervasives_impl.ttt".

Parameter tt : value Bool.
Extract Constant tt => "Pervasives_impl.tt".

Parameter ff : value Bool.
Extract Constant ff => "Pervasives_impl.ff".

Parameter salloc : forall L T, m (Loc L) T.
Extract Constant salloc => "Pervasives_impl.salloc".

Parameter halloc : forall L T, m (Loc L) T.
Extract Constant halloc => "Pervasives_impl.halloc".

Parameter get : forall L T, value (Loc L) -> m L T.
Extract Constant get => "Pervasives_impl.get".

Parameter put : forall L T, value (Loc L) -> value L -> m (Loc L) T.
Extract Constant put => "Pervasives_impl.put".

Parameter salloc_with_init : forall L T, value L -> m (Loc L) T.
Extract Constant salloc_with_init => "Pervasives_impl.salloc_with_init".

Parameter call : forall L T, fanswer L -> m L T.
Extract Constant call => "Pervasives_impl.call".

(**********)
(** Stmt **)
(**********)

Parameter do_if : forall T, m Bool T -> m Void T -> m Void T -> m Void T.
Extract Constant do_if => "Pervasives_impl.do_if".

Parameter do_for : forall Linc Lbody T, m Bool T -> m Linc T -> m Lbody T -> m Void T.
Extract Constant do_for => "Pervasives_impl.do_for".

Parameter do_while : forall Lbody T, m Bool T -> m Lbody T -> m Void T.
Extract Constant do_while => "Pervasives_impl.do_while".


(**********)
(** Expr **)
(**********)

Parameter array_subscript: forall I L T,  value (Loc L) -> value I -> m (Loc L) T.
Extract Constant array_subscript => "Pervasives_impl.array_subscript".

Parameter ife : forall L T, m Bool T -> m L T -> m L T -> m L T.
Extract Constant ife => "Pervasives_impl.ife".

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
Parameter array_to_pointer_decay : forall L, value (Loc (Array L)) -> value (Loc L).
Extract Constant array_to_pointer_decay => "Pervasives_impl.array_to_pointer_decay".

(* FunctionToPointerDecay *)
(* NullToPointer *)
Parameter null_to_pointer : forall L T, value L -> value T.
Extract Constant null_to_pointer => "Pervasives_impl.null_to_pointer".

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
Parameter pointer_to_boolean : forall E, value (Loc E) -> value Bool.
Extract Constant pointer_to_boolean => "Pervasives_impl.pointer_to_boolean".

(* ToVoid *)
(* VectorSplat *)
(* IntegralCast *)
Parameter integral_cast : forall Src Trg T, value Src -> m Trg T.
Extract Constant integral_cast => "Pervasives_impl.integral_cast".

(* IntegralToBoolean *)
Parameter integral_to_boolean : forall L, value L -> value Bool.
Extract Constant integral_to_boolean => "Pervasives_impl.integral_to_boolean".

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

(* PtrMemD *)
(* PtrMemI *)
(* Mul *)
(* Div *)
(* Rem *)
(* Add *)
Parameter add_Int_Int : forall T, value Int -> value Int -> m Int T.
Extract Constant add_Int_Int => "Pervasives_impl.add_Int_Int".

(* Sub *)
Parameter sub_Int_Int : forall T, value Int -> value Int -> m Int T.
Extract Constant sub_Int_Int => "Pervasives_impl.sub_Int_Int".

(* Shl *)
(* Shr *)
(* LT *)
Parameter lt_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant lt_Int_Int => "Pervasives_impl.lt_Int_Int".

Parameter lt_Uint_Uint : forall T, value Uint -> value Uint -> m Bool T.
Extract Constant lt_Uint_Uint => "Pervasives_impl.lt_Uint_Uint".

(* GT *)
Parameter gt_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant gt_Int_Int => "Pervasives_impl.gt_Int_Int".

(* LE *)
Parameter le_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant le_Int_Int => "Pervasives_impl.le_Int_Int".

Parameter le_Uint_Uint : forall T, value Uint -> value Uint -> m Bool T.
Extract Constant le_Uint_Uint => "Pervasives_impl.le_Uint_Uint".

(* GE *)
Parameter ge_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant ge_Int_Int => "Pervasives_impl.ge_Int_Int".

Parameter ge_Uint_Uint : forall T, value Uint -> value Uint -> m Bool T.
Extract Constant ge_Uint_Uint => "Pervasives_impl.ge_Uint_Uint".

(* EQ *)
Parameter eq_LocAvl_node_base_LocAvl_node_base : forall T, value (Loc Avl_node_base) -> value (Loc Avl_node_base) -> m Bool T.
Extract Constant eq_LocAvl_node_base_LocAvl_node_base => "Pervasives_impl.eq_LocAvl_node_base_LocAvl_node_base".

Parameter eq_Uint_Uint : forall T, value Uint -> value Uint -> m Bool T.
Extract Constant eq_Uint_Uint => "Pervasives_impl.eq_Uint_Uint".

Parameter eq_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant eq_Int_Int => "Pervasives_impl.eq_Int_Int".

(* NE *)
Parameter ne_LocAvl_node_base_LocAvl_node_base : forall T, value (Loc Avl_node_base) -> value (Loc Avl_node_base) -> m Bool T.
Extract Constant ne_LocAvl_node_base_LocAvl_node_base => "Pervasives_impl.ne_LocAvl_node_base_LocAvl_node_base".

Parameter ne_Int_Int : forall T, value Int -> value Int -> m Bool T.
Extract Constant ne_Int_Int => "Pervasives_impl.ne_Int_Int".

(* And *)
(* Xor *)
(* Or *)
(* LAnd *)
Parameter land_Bool_Bool : forall T, m Bool T -> m Bool T -> m Bool T.
Extract Constant land_Bool_Bool => "Pervasives_impl.land_Bool_Bool".

(* LOr *)
(* Assign *)
Parameter assign_Bool_Bool : forall T, value (Loc Bool) -> value Bool -> m (Loc Bool) T.
Extract Constant assign_Bool_Bool => "Pervasives_impl.assign_Bool_Bool".

Parameter assign_Uchar_Uchar : forall T, value (Loc Uchar) -> value Uchar -> m (Loc Uchar) T.
Extract Constant assign_Uchar_Uchar => "Pervasives_impl.assign_Uchar_Uchar".

Parameter assign_LocAvl_node_base_LocAvl_node_base : forall T, value (Loc (Loc Avl_node_base)) -> value (Loc Avl_node_base) -> m (Loc (Loc Avl_node_base)) T.
Extract Constant assign_LocAvl_node_base_LocAvl_node_base => "Pervasives_impl.assign_LocAvl_node_base_LocAvl_node_base".

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
Parameter postinc_Int : forall T, value (Loc Int) -> m Int T.
Extract Constant postinc_Int => "Pervasives_impl.postinc_Int".

(* PostDec *)
(* PreInc *)
(* PreDec *)
(* AddrOf *)
Parameter addrof_Avl_node_base : forall T, value (Loc Avl_node_base) -> m (Loc Avl_node_base) T.
Extract Constant addrof_Avl_node_base => "Pervasives_impl.addrof_Avl_node_base".

(* Deref *)
(* Plus *)
(* Minus *)
Parameter minus_Int : forall T, value Int -> m Int T.
Extract Constant minus_Int => "Pervasives_impl.minus_Int".

(* Not *)
(* LNot *)
Parameter lnot_Bool : forall T, value Bool -> m Bool T.
Extract Constant lnot_Bool => "Pervasives_impl.lnot_Bool".

(* Real *)
(* Imag *)
(* Extension *)
(* Coawait *)

(***************)
(** Constants **)
(***************)

Parameter int_0 : value Int.
Extract Constant int_0 => "Pervasives_impl.int_0".

Parameter int_1 : value Int.
Extract Constant int_1 => "Pervasives_impl.int_1".

Parameter int_2 : value Int.
Extract Constant int_2 => "Pervasives_impl.int_2".

Parameter int_3 : value Int.
Extract Constant int_3 => "Pervasives_impl.int_3".

Parameter int_4 : value Int.
Extract Constant int_4 => "Pervasives_impl.int_4".

Parameter int_5 : value Int.
Extract Constant int_5 => "Pervasives_impl.int_5".

