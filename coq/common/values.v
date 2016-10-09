Inductive stype : Set :=
| A : stype
| Void : stype
| Loc : stype -> stype
(* builtin types *)
| Bool
| Uchar
| Ulong
| Ulonglong
| Char_s
| Int
| Long
| Longlong
| Nullptr.

Parameter value : stype -> Set.
Extract Constant value => "Pervasives_base.value".
