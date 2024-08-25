# 1 "src/IVT.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/IVT.S"
.align 4
.global IVT
.type IVT, @function
IVT:
j supervisorTrap
j supervisorTrap1
j supervisorTrap
j supervisorTrap
j supervisorTrap
j supervisorTrap
j supervisorTrap
j supervisorTrap
j supervisorTrap
j supervisorTrap2
j supervisorTrap
j supervisorTrap
