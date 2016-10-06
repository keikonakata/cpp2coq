type void = unit
type loc = int
type uchar = int
type ulong = int
type ulonglong = int
type char_s = int
type long = int
type longlong = int
type nullptr = int

type value =
| V_Void of void
| V_Loc of loc
| V_Bool of bool
| V_Uchar of uchar
| V_Ulong of ulong
| V_Ulonglong of ulonglong
| V_Char_s of char_s
| V_Int of int
| V_Long of long
| V_Longlong of longlong
| V_Nullptr of nullptr

type mitem = Val of value | Ptr of int | Emp

type state = { index : int; space : mitem array}
