jmp magicNumber

p:     .fill 80 - ASCII for 'P'
six:   .fill 54 - ASCII for '6'
space: .fill 32 - ASCII for ' '

magicNumber: lw $1 $0 p
             out $1         - Output 'P' stdout
             lw $1 $0 six
             out $1         - Output '6' to stdout
             lw $1 $0 space
             out $1         - Output ' ' to stdout
             halt
