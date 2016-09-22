Inductive stype : Set :=
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
