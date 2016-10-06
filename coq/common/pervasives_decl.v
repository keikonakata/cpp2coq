Require Import mathcomp.ssreflect.ssreflect.
Require Import common.values.
Require Import common.compt.

Parameter ttt : value Void.
Extract Constant ttt => "Pervasives_impl.ttt".

Parameter tt : value Bool.
Extract Constant tt => "Pervasives_impl.tt".

Parameter ff : value Bool.
Extract Constant ff => "Pervasives_impl.ff".

Parameter call : forall L T, fanswer L -> m L T.
Extract Constant call => "Pervasives_impl.call".

Parameter add_Int_Int : forall T, value Int -> value Int -> m Int T.
Extract Constant add_Int_Int => "Pervasives_impl.add_Int_Int".