;Function prolog,
;load argument x
push %rbp
mov %rsp,%rbp
mov %rdi,-0x8(%rbp)
;Node A in tree:
;Compare x < 5
cmpq %rax, $5
;Goto Node B
jb $21
;Compare x >= 8
cmpq %rax, $8
;Goto Node C
jae $70
;Return Bitvector 2
mov $2, %eax
pop %rbp
retq
;Node B in tree:
;Compare x < 1
cmpq %rax, $1
;Goto Node D 
jb $21
;Compare x >= 3
cmpq %rax, $3
;Jump over return statement
jae $7
;Return Bitvector 0
mov $0, %eax
pop %rbp
retq
;Node D in tree:
;Compare x < 3
cmpq %rax, $3
;Jump over return statement
jb $7
;Return Bitvector 1
mov $1, %eax
pop %rbp
retq
;Return Bitvector 1
mov $1, %eax
pop %rbp
retq
;Node C in tree:
;Compare x < 11
cmpq %rax, $11
;Goto Node E
jb $21
;Node F in tree:
;Compare x < 13
cmpq %rax, $13
;Goto return statement
jae $35
;Return Bitvector 4
mov $4, %eax
pop %rbp
retq
;Node E in tree:
;Compate x < 8
cmpq %rax, $8
;Jump over return statement
jb $7
;Return Bitvector 3
mov $3, %eax
pop %rbp
retq
;Return Bitvector 3
mov $3, %eax
pop %rbp
retq
;Node F in tree:
;Compare x < 13
cmpq %rax, $13
;Jump over return statement
jb $7
;Return Bitvector 5
mov $5, %eax
pop %rbp
retq 
;Return Bitvector 5
mov $5, %eax
pop %rbp
retq
