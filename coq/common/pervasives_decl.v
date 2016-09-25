Require Import mathcomp.ssreflect.ssreflect.
Require Import values.
Require Import compt.

Parameter add_Int_Int : forall T, value Int -> value Int -> m Int T.