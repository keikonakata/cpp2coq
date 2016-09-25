Require Import mathcomp.ssreflect.ssreflect.
Require Import values.

Set Implicit Arguments.

Parameter state : Set.

Inductive compt (L T : stype) : Set :=
| step : value L -> state ->  compt L T
| ret : value T -> state -> compt L T
| continue : state -> compt L T
| break : state -> compt L T
| error : compt L T.

Definition m (L T : stype) := state -> compt L T.
Definition result (T : stype) := m void T.

Definition bind (L0 L1 T : stype) (a : m L0 T) (f : value L0 -> m L1 T) : m L1 T:= fun st =>
match a st with
| step v st' => f v st'
| ret v st' => ret _ v st'
| continue st' => continue _ _ st'
| break st' => break _ _ st'
| error => error _ _
end.

Notation "X <- A ; B" := (bind A (fun X => B)) (at level 80, right associativity).
