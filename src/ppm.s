jmp magicNumber

p:     .fill 80 - ASCII for 'P'
six:   .fill 54 - ASCII for '6'
space: .fill 32 - ASCII for ' '
four:  .fill 52 - ASCII for '4'

magicNumber: lw $1 $0 p
             out $1
             lw $1 $0 six
             out $1
             lw $1 $0 space
             out $1           - Output "P6 " to stdout


             addi $3 $3 2     - $3 = 2
widthHeight: lw $1 $0 six
             out $1
             lw $1 $0 four
             out $1
             lw $1 $0 space
             out $1           - Output "64 " to stdout
             addi $2 $2 1     - $2++
             beq $2 $3 2      - Skip if outputted twice,
             jmp widthHeight  -   otherwise loop
             halt
