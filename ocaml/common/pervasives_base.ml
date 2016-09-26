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
| Void of void
| Loc of loc
| Bool of bool
| Uchar of uchar
| Ulong of ulong
| Ulonglong of ulonglong
| Char_s of char_s
| Int of int
| Long of long
| Longlong of longlong
| Nullptr of nullptr

type state

