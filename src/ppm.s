jmp magicNumber

space: .fill 32 - ASCII for ' '
four:  .fill 52 - ASCII for '4'
six:   .fill 54 - ASCII for '6'
p:     .fill 80 - ASCII for 'P'

magicNumber: lw $1 $0 p
             out $1
             lw $1 $0 six
             out $1
             lw $1 $0 space
             out $1           - Output "P6 " to stdout

             addi $3 $0 2     - $3 = 2
widthHeight: lw $1 $0 six
             out $1
             lw $1 $0 four
             out $1
             lw $1 $0 space
             out $1           - Output "64 " to stdout
             addi $2 $2 1     - $2++
             beq $2 $3 2      - Skip if outputted twice,
             jmp widthHeight  -   otherwise loop

maxValue:    addi $1 $0 255
             out $1           - Output 255 to stdout
             lw $1 $0 space
             out $1           - Output " " to stdout


             addi $2 $0 0     - $2 = 0
             addi $3 $0 2048  - $3 = 2048
twoPixels:   addi $1 $0 255
             out $1
             out $1
             out $1
             addi $1 $0 0
             out $1
             out $1
             out $1
             addi $2 $2 1
             beq $2 $3 2
             jmp twoPixels
             halt
