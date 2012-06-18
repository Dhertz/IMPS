jmp magicNumber

space: .fill 32 - ASCII for ' '
two:   .fill 50 - ASCII for '2'
four:  .fill 52 - ASCII for '4'
five:  .fill 53 - ASCII for '5'
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

maxValue:    lw $1 $0 two
             out $1
             lw $1 $0 five
             out $1
             out $1
             lw $1 $0 space
             out $1           - Output "255 " to stdout

             addi $1 $0 255   - $1 = 255
             addi $2 $0 0     - $2 = 0
             addi $3 $0 32    - $3 = 32
             addi $5 $0 4     - $5 = 4
             addi $6 $0 24    - $6 = 24

twoRows:     addi $4 $0 0     - $4 = 0

whiteBlack:  addi $7 $0 0     - $7 = 0
whiteLoop:   out $1           - Output 255 (3x this makes up a white pixel)
             addi $7 $7 1     - $7++
             beq $7 $6 2      - Skip if 8 pixels outputted,
             jmp whiteLoop    -   loop otherwise
             addi $7 $0 0     - $7 = 0

blackLoop:   out $0           - Output 0 (3x this makes black pixel)
             addi $7 $7 1     - $7++
             beq $7 $6 2      - Skip if 8 pixels outputted,
             jmp blackLoop    -   loop otherwise
             addi $4 $4 1     - $4++
             beq $4 $3 2      - Skip if row finished,
             jmp whiteBlack   -   otherwise loop

             addi $4 $0 0     - $4 = 0

blackWhite:  addi $7 $0 0     - $7 = 0
blackLoop2:  out $0           - Output 0 (3x this makes black pixel)
             addi $7 $7 1     - $7++
             beq $7 $6 2      - Skip if 8 pixels outputted,
             jmp blackLoop2   -   loop otherwise
             addi $7 $0 0     - $7 = 0

whiteLoop2:  out $1           - Output 255 (3x this makes up a white pixel)
             addi $7 $7 1     - $7++
             beq $7 $6 2      - Skip if 8 pixels outputted,
             jmp whiteLoop2   -   loop otherwise
             addi $4 $4 1     - $4++

             beq $4 $3 2      - Skip if row finished,
             jmp blackWhite   -   otherwise loop
             addi $2 $2 1     - $2++

             beq $2 $5 2      - Skip if image finished,
             jmp twoRows      -   otherwise loop
             halt
