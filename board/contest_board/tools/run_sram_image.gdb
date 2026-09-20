set pagination off
target extended-remote localhost:55000
load
set {unsigned int}0xE000ED08 = 0x34000400
set $sp = *(unsigned int *)0x34000400
set $pc = *(unsigned int *)0x34000404
continue
