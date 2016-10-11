type coq_int = int option
type loc = int option
type uchar = int option
type ulong = int option
type ulonglong = int option
type char_s = int option
type long = int option
type longlong = int option

type value =
| V_Void
| V_Loc of loc
| V_Bool of bool
| V_Uchar of uchar
| V_Ulong of ulong
| V_Ulonglong of ulonglong
| V_Char_s of char_s
| V_Int of coq_int
| V_Long of long
| V_Longlong of longlong
| V_Nullptr

type mitem = Val of value | Ptr of int | Emp

type state = { mutable index : int; space : mitem array}
