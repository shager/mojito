push %rbp
mov %rsp,%rbp
mov %rdi,-0x8(%rbp)
cmpq %rax, $5
jb $21
cmpq %rax, $8
jae $70
mov $2, %eax
pop %rbp
retq
cmpq %rax, $1
jb $21
cmpq %rax, $3
jae $7
mov $0, %eax
pop %rbp
retq
cmpq %rax, $3
jb &7
mov $1, %eax
pop %rbp
retq
mov $1, %eax
pop %rbp
retq
cmpq %rax, $11
jb $21
cmpq %rax, $13
jae $35
mov $4, %eax
pop %rbp
retq
cmpq %rax, $8
jb $7
mov $3, %eax
pop %rbp
retq
mov $3, %eax
pop %rbp
retq
cmpq %rax, $13
jb $7
mov $5, %eax
pop %rbp
retq 
mov $5, %eax
pop %rbp
retq
