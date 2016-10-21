Inductive stype : Set :=
| A : stype
| Avl_node_base : stype
| Avl_node_basePolicy : stype
| Void : stype
| Loc : stype -> stype
| Array : stype -> stype
(* builtin types *)
| Bool
| Uchar
| Uint
| Ulong
| Ulonglong
| Char_s
| Int
| Long
| Longlong
| Nullptr.

Parameter value : stype -> Set.
Extract Constant value => "Pervasives_base.value".
