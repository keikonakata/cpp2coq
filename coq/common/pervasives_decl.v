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

Parameter salloc_with_init : forall L T, value L -> m (Loc L) T.
Extract Constant salloc_with_init => "Pervasives_impl.salloc_with_init".

Parameter call : forall L T, fanswer L -> m L T.
Extract Constant call => "Pervasives_impl.call".

Parameter add_Int_Int : forall T, value Int -> value Int -> m Int T.
Extract Constant add_Int_Int => "Pervasives_impl.add_Int_Int".

Parameter int_0 : value Int.
Parameter int_1 : value Int.