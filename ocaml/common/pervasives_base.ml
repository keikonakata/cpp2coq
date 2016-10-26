type coq_int = int option
type coq_bool = bool option
type loc = int option
type uchar = int option
type uint = int option
type ulong = int option
type ulonglong = int option
type char_s = int option
type long = int option
type longlong = int option

type value =
| V_Avl_node_base of avl_node_base
| V_Avl_node_basePolicy
| V_Void
| V_Loc of loc
| V_Array of value array
| V_Bool of coq_bool
| V_Uchar of uchar
| V_Uint of uint
| V_Ulong of ulong
| V_Ulonglong of ulonglong
| V_Char_s of char_s
| V_Int of coq_int
| V_Long of long
| V_Longlong of longlong
| V_Nullptr

and avl_node_base = { mutable _child : value; mutable _parent : value; mutable _depth : value; mutable _value : value }


let init_V_Loc () = V_Loc None
let init_V_Int () = V_Int None
let init_V_Uchar () = V_Uchar None
let init_V_Uint () = V_Uint None
let init_V_Bool () = V_Bool None
let mk_V_Int i = V_Int (Some i)
let mk_V_Loc i = V_Loc (Some i)
let mk_V_Bool b = V_Bool (Some b)

type mitem = Val of value | Ptr of int | Emp

type state = { mutable index : int; space : mitem array}
