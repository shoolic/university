# Image scaling with bilinear interpolation algorithm
# Maciej Szulik 
# Warsaw University of Technology
# Faculty of Electronics and Information Technology
# Computer Architecture 
# 2018/2019

.eqv INP_BUFFER_SIZE            4096
.eqv OUT_BUFFER_SIZE            4092 # ! divisible by 12
.eqv FRAC_N                     12
.eqv FRAC_MASK                  0x00000fff
.eqv INT_MASK                   0xfffff000

.data

# user interface messages
introMsg:                       .asciiz "Image scaling with bilinear interpolation algorithm\n"
inputPathRequestMsg:            .asciiz "Enter a path to a .bmp input file:\n"
outputPathRequestMsg:           .asciiz "Enter a path to a .bmp output file:\n"
outputResolutionMsg:            .asciiz "Enter an output resolution in pixels\n"
outputWidthRequestMsg:          .asciiz "Witdh:\n"
outputHeightRequestMsg:         .asciiz "Height:\n"
fileErrorMsg:                   .asciiz "A file error occured\n"
tooWidePictureErrMsg:           .asciiz "Picture is too wide and cannot be processed\n"
successMsg:                     .asciiz "Image was successfully scaled\n"



inputPath:                      .space  32
outputPath:                     .space  32
maxNumberOfLinesInInputBuffer:  .word   0
inputFullLineLength:            .word   0
firstLineIndex:                 .word   0
inputBuffer:                    .space  INP_BUFFER_SIZE
outputBuffer:                   .space  OUT_BUFFER_SIZE

.text
main:

    # lw  $s3, outputWidth # TEMPORARY
    # lw  $s4, outputHeight # TEMPORARY

    ### UI

    # print introMsg
    la      $a0, introMsg
    li      $v0, 4
    syscall

    # INPUT FILE

    # print inputPathRequestMsg
    la      $a0, inputPathRequestMsg
    li      $v0, 4
    syscall

    # load input path
    la      $a0, inputPath
    li      $a1, 31
    li      $v0, 8
    syscall

    # remove '\n' character
    # $t0 - address of inputPath
    # $t1 - char read 
    la      $t0, inputPath
    strRemoveEndlineAtInputPath:
        lbu     $t1, 0($t0)
        addiu   $t0, $t0, 1
        bne     $t1, '\n', strRemoveEndlineAtInputPath
        sb      $zero, -1($t0)

    # OUTPUT FILE

    # print outputPathRequestMsg
    la      $a0, outputPathRequestMsg
    li      $v0, 4
    syscall

    # load output path
    la      $a0, outputPath
    li      $a1, 31
    li      $v0, 8
    syscall

    # remove '\n' character
    # $t0 - address of inputPath
    # $t1 - char read 
    la      $t0, outputPath
    strRemoveEndlineAtOutputPath:
        lbu     $t1, 0($t0)
        addiu   $t0, $t0, 1
        bne     $t1, '\n', strRemoveEndlineAtOutputPath
        sb      $zero, -1($t0)


    # print outputResolutionMsg
    la      $a0, outputResolutionMsg
    li      $v0, 4
    syscall

    # print outputWidthRequestMsg
    la      $a0, outputWidthRequestMsg
    li      $v0, 4
    syscall

    # load output width
    li      $v0, 5
    syscall

    # save output width
    move    $s3, $v0


    # print outputHeightRequestMsg
    la      $a0, outputHeightRequestMsg
    li      $v0, 4
    syscall

    # load output height
    li      $v0, 5
    syscall

    # save output width
    move    $s4, $v0

    ### IMAGE PROCESSING

    # REGISTERS:
    # $s0 - input file descriptor
    # $s1 - input width - 1
    # $s2 - input height - 1 
    # $s3 - output width
    # $s4 - output height (decrements during the loop) 
    # $s5 - ratioY
    # $s6 - output padding
    # $s7 - ratioX
    # $a3 - output file
    # $t0 - inputLineFullLength

    # OPEN INPUT FILE
    li      $v0, 13
    la      $a0, inputPath
    li      $a1, 0
    li      $a2, 0
    syscall

    # check if opening file was successful
    bltz    $v0, fileError

    # save file descriptor
    move    $s0, $v0

    # READING BITMAPFILEHEADER AND BITMAPINFOHEADER

    li      $v0, 14
    move    $a0, $s0
    la      $a1, outputBuffer
    li      $a2, 54 
    syscall

    # NEW FUNCTION REGISTER
    # $t0 - bmpHeaders address
    # $t1 - offset to pixel array
    # $t2 - input padding
    # get offset to pixel array

    la      $t0, outputBuffer

    ulw     $t1, 10($t0)

    # get input width
    ulw     $s1, 18($t0)

    # get input height
    ulw     $s2, 22($t0) 

    # set input padding
    andi    $t2, $s1, 3

    # calc inputFullLinelength
    mulu    $t0, $s1, 3
    addu    $t0, $t0, $t2
    sw      $t0, inputFullLineLength

    # calc maxNumberOfLinesInInputBuffer
    li      $t4, INP_BUFFER_SIZE
    divu    $t4, $t4, $t0
    sw      $t4, maxNumberOfLinesInInputBuffer

    # check if at least two lines can be stored in the buffer
    blt     $t4, 2, tooWidePictureErr

    # calc firstLineIndex
    # $t4 = input width($s2) + maxNumberOfLinesInInputBuffer($t4) -1
    addu    $t4,  $s2, $t4
    subiu   $t4, $t4, 1
    sw      $t4, firstLineIndex

    
    # calc ratioY
    sll     $s2, $s2, FRAC_N
    divu    $s5, $s2, $s4
    srl     $s2, $s2, FRAC_N

    # calc ratioX
    sll     $s1, $s1, FRAC_N
    divu    $s7, $s1, $s3
    srl     $s1, $s1, FRAC_N

    # OUTPUT FILE OPENING
    li      $v0, 13
    la      $a0, outputPath
    li      $a1, 1
    li      $a2, 0
    syscall

    # check if opening file was successful
    bltz    $v0, fileError

    # save file descriptor
    move    $a3, $v0

    # set output padding
    andi    $s6, $s3, 3
    
    # calculate size of output file
    # file size = offset + size of pixel array
    # size of pixel array = height * width
    # NEW FUNCTION REGISTERS
    # $t2 - size of file
    # $t3 - result of width << 3
    # $t4 - address of output buffer

    mulu    $t2, $s3, $s4
    # file size = offset + size of pixel array
    addu    $t2, $t2, $t1

    la      $t4, outputBuffer

    # set file size
    usw     $t2, 2($t4)

    # set output width
    usw     $s3, 18($t4)

    # set output height
    usw     $s4, 22($t4) 

    # store headers
    li      $v0, 15
    move    $a0, $a3
    la      $a1, outputBuffer
    move    $a2, $t1
    syscall

    # read trash 
    li      $v0, 14
    move    $a0, $s0
    la      $a1, inputBuffer
    subiu   $a2, $t1, 54
    syscall


    # LOOP INIT

    la      $t8, outputBuffer
    la      $t9, inputBuffer

    # decrement input width & height
    subiu   $s1, $s1, 1
    subiu   $s2, $s2, 1

    heightIteration:
        beqz    $s4, end
        # LOAD INITIAL VALUES 
        # REGISTERS:
        # $s0 - input file descriptor
        # $s1 - input width -1 
        # $s2 - input height -1 
        # $s3 - output width
        # $s4 - output height (decrements during the loop) 
        # $s5 - ratioY
        # $s6 - output padding
        # $s7 - ratioX
        # $a3 - output file

        # NEW FUNCTIONS REGISTERS
        # $t0 - inputLineFullLength
        # $t1 - firstLineIndex
        # $t2 - maxNumberOfLinesInInputBuffer
        # $t9 - (moved) address of inputBuffer
        lw      $t0, inputFullLineLength
        lw      $t1, firstLineIndex
        lw      $t2, maxNumberOfLinesInInputBuffer

        # decrement height counter
        subiu   $s4, $s4, 1

        # NEW FUNCTIONS REGISTERS
        # $t3 - input y = output y(nbc) * ratioY(fix)
        # $t4 - fractional part of input y, (fix)

        mulu    $t3, $s4, $s5
        andi    $t4, $t3,  FRAC_MASK

        # NEW FUNCTIONS REGISTERS
        # $t3 - floor of input y (nbc)
        andi    $t3, $t3, INT_MASK
        srl     $t3, $t3, FRAC_N

        # NEW FUNCTIONS REGISTERS
        # $t9 - points address of current line

        # FREE REGISTER $t5,6,7

        checkBuffer:
        # CHECKING IF REQUIRED DATA STORED IN BUFFER
        # NEW FUNCTIONS REGISTERS
        # $t5 - index of last read line
        # $t5 = firstLineIndex ($t1) - maxNumberOfLinesInInputBuffer ($t2) + 1 (last read line)
        # if index of current line ($t3) is greater than $t5,
        # there is no need to read extra bytes
        subu    $t5, $t1, $t2
        addiu   $t5, $t5, 1
        bgt     $t3, $t5, heightIterationContinuation

                # if index of current line is equal to $t5 
                # there is still need to store it, but also to read extra bytes (indexEquality)
                beq     $t3, $t5, indexEquality

                        # if index of current line is greater than index of last read line,
                        # there is  need to read max number of lines 
                        # that can be stored in inputBuffer

                        # NEW FUNCTIONS REGISTERS
                        # $a2 - number of bytes to read
                        # $a2 = inputLineFullLength($t0) * maxNumberOfLinesInInputBuffer($t2)

                        # read bytes
                        li      $v0, 14
                        move    $a0, $s0
                        la      $a1, inputBuffer
                        mulu    $a2, $t0, $t2
                        syscall

                        # set new value of firstLineIndex
                        subu    $t1, $t1, $t2
                        sw      $t1, firstLineIndex
            
                        # FREE REGISTERS $t5,6,7
                        j       checkBuffer

                indexEquality:
                    # if index of current line is equal to $t5,
                    # there is still need to store current line in memory
                    # but also to read extra lines

                    # calc byte offset in inputBuffer from firstLineIndex to floor of input y
                    la      $t9, inputBuffer
                    subu    $t5, $t1, $t3
                    mulu    $t5, $t5, $t0
                    addu    $t9, $t9, $t5

                    # NEW FUNCTIONS REGISTERS
                    # $t5 - tmp for word
                    # $t6 - beginning of input buffer
                    # $t7 - bytes left counter
                    la      $t6, inputBuffer
                    move    $t7, $t0

                    # move last line to the beginning of inputBuffer
                    copyBytes:
                        lb      $t5, ($t9)
                        sb      $t5, ($t6)
                        subiu   $t7, $t7, 1
                        addiu   $t9, $t9, 1
                        addiu   $t6, $t6, 1
                        bnez    $t7, copyBytes

                    # FREE REGISTERS $t5,7

                    # read maxNumberOfLinesInInputBuffer - 1 lines
                    # $t6 - beginning of second line in input buffer

                    # decrement maxNumberOfLinesInInputBuffer ($t2)
                    subiu   $t2, $t2, 1

                    # $a2 - number of bytes to read
                    # $a2 = (maxNumberOfLinesInInputBuffer - 1)($t2) * inputLineFullLength ($t0)
        
                    li      $v0, 14
                    move    $a0, $s0
                    la      $a1, ($t6)
                    mulu    $a2, $t2, $t0
                    syscall

                    # set new value of firstLineIndex
                    subu    $t1, $t1, $t2
                    sw      $t1, firstLineIndex

                    # FREE REGISTERS $t2,5,6,7

        heightIterationContinuation:
            # calc offset in inputBuffer from firstLineIndex to floor of input y
            la      $t9, inputBuffer
            subu    $t5, $t1, $t3
            mulu    $t5, $t5, $t0
            addu    $t9, $t9, $t5

            # NEW FUNCTIONS REGISTERS
            # $t1 - output x counter
            move    $t1, $zero

            # $t5 - 1 - fractional part of y
            li      $t5, 1
            sll     $t5, $t5, FRAC_N
            subu    $t5, $t5, $t4

            # if floor of input y($t3)  equals to input height - 1 ($s2),
            # it means it's last line
            # $t0 - 0 or value of inputFullLineLength depending on wether we operate on last line
            bnez    $t3, widthIteration
                    move    $t0, $zero

            # FREE REGISTERS $t2,3,5,6,7 / $a0,1,2

            widthIteration:

                # NEW FUNCTIONS REGISTERS
                # $t2 - input x = output x(nbc) * ratioX(fix)
                # $t3 - fractional part of input x, (fix)
                
                # get fractional part of input x
                mulu    $t2, $t1, $s7 # fix 
                andi    $t3, $t2,  FRAC_MASK

                # $t2 - floor of input x (nbc)
                andi    $t2, $t2, INT_MASK
                srl     $t2, $t2, FRAC_N

                # NEW FUNCTIONS REGISTERS
                # $t6 = floor of input x(nbc) * 3
                # $a2 - saved value of $t9 to restore it at the ned of the loop
                mulu    $t6, $t2, 3
                move    $a2, $t9
                # move address by 3 * floor x bytes
                addu    $t9, $t9, $t6

                # NEW FUNCTIONS REGISTERS
                # $t6 - 1 - fractional part of input x, (fix)
                li      $t6, 1
                sll     $t6, $t6, FRAC_N
                subu    $t6, $t6, $t3

                # $t2 - bits shift to next pixel (0 if last column or 3  if not)
                # if is not last column, set $t4 to 1, if not 0
                sne     $t2, $t2, $s1
                mulu    $t2, $t2, 3

                # REGISTERS
                # $t0 - line shift (0 or inputLineFullLength) (nbc)
                # $t1 - output x counter (nbc)
                # $t2 - bits shift to next pixel (0 or 3) (nbc)
                # $t3 - fractional part of input x, (fix); fx
                # $t4 - fractional part of input y, (fix); fy
                # $t5 - 1 - fractional part of input y, (fix); 1 - fy
                # $t6 - 1 - fractional part of input x, (fix); 1 - fx

                # FREE REGISTERS $t7 / $a0,1

                # CALC NEW SUBPIXEL VALUE
                # F(x,y) = (1 - fy) * F(x,0) + fy * F(x,1)
                # F(x,0) = (1 - fx) * F(0,0) + fx * F(1,0)
                # F(x,1) = (1 - fx) * F(0,1) + fx * F(1,1)
                
                # FIRST
                # (1 - fx) * F(0,1)
                lbu     $t7, ($t9)
                mulu    $t7, $t7, $t6 # fix

                # move address to next column
                addu    $t9, $t9, $t2

                # fx * F(1,1)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # F(x,0)
                addu    $t7, $t7, $a0

                # fy * F(x,0)
                mulu    $t7, $t7, $t4
                srl     $t7, $t7, FRAC_N

                # move address to next row
                addu    $t9, $t9, $t0

                # fx * F(1,0)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # move address to prev column
                subu    $t9, $t9, $t2

                # (1 - fx) * F(0,0)
                lbu     $a1, ($t9)
                mulu    $a1, $a1, $t6

                # F(x,0)
                addu    $a0, $a0, $a1

                # (1- fy) * F(x,0)
                mulu    $a0, $a0, $t5
                srl     $a0, $a0, FRAC_N

                # F(x,y)
                addu    $t7, $t7, $a0
                srl     $t7, $t7, FRAC_N
                andi    $t7, $t7, FRAC_MASK

                # store byte
                sb      $t7, ($t8)
                addiu   $t8, $t8, 1

                # move address to prev row
                subu    $t9, $t9, $t0

                # move address to next subpixel
                addiu   $t9, $t9, 1

                # SECOND
                # (1 - fx) * F(0,1)
                lbu     $t7, ($t9)
                mulu    $t7, $t7, $t6 # fix

                # move address to next column
                addu    $t9, $t9, $t2

                # fx * F(1,1)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # F(x,0)
                addu    $t7, $t7, $a0

                # fy * F(x,0)
                mulu    $t7, $t7, $t4
                srl     $t7, $t7, FRAC_N

                # move address to next row
                addu   $t9, $t9, $t0

                # fx * F(1,0)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # move address to prev column
                subu    $t9, $t9, $t2

                # (1 - fx) * F(0,0)
                lbu     $a1, ($t9)
                mulu    $a1, $a1, $t6

                # F(x,0)
                addu    $a0, $a0, $a1

                # (1- fy) * F(x,0)
                mulu    $a0, $a0, $t5
                srl     $a0, $a0, FRAC_N

                # F(x,y)
                addu    $t7, $t7, $a0
                srl     $t7, $t7, FRAC_N
                andi    $t7, $t7, FRAC_MASK

                # store byte
                sb      $t7, ($t8)
                addiu   $t8, $t8, 1

                # move address to prev row
                subu    $t9, $t9, $t0

                # move address to next subpixel
                addiu   $t9, $t9, 1

                # LAST
                # (1 - fx) * F(0,1)
                lbu     $t7, ($t9)
                mulu    $t7, $t7, $t6 # fix

                # move address to next column
                addu    $t9, $t9, $t2

                # fx * F(1,1)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # F(x,0)
                addu    $t7, $t7, $a0

                # fy * F(x,0)
                mulu    $t7, $t7, $t4
                srl     $t7, $t7, FRAC_N

                # move address to next row
                addu   $t9, $t9, $t0

                # fx * F(1,0)
                lbu     $a0, ($t9)
                mulu    $a0, $a0, $t3 # fix

                # move address to prev column
                subu    $t9, $t9, $t2

                # (1 - fx) * F(0,0)
                lbu     $a1, ($t9)
                mulu    $a1, $a1, $t6

                # F(x,0)
                addu    $a0, $a0, $a1

                # (1- fy) * F(x,0)
                mulu    $a0, $a0, $t5
                srl     $a0, $a0, FRAC_N

                # F(x,y)
                addu    $t7, $t7, $a0
                srl     $t7, $t7, FRAC_N
                andi    $t7, $t7, FRAC_MASK

                # store byte
                sb      $t7, ($t8)
                addiu   $t8, $t8, 1

                # FREE REGISTERS $t2,3,6,7 / $a0,1
                # $t3 = current address in outputBuffer - beginning of outputBuffer
                la      $t2, outputBuffer
                subu    $t3, $t8, $t2


                # restore $t9 
                move $t9, $a2

                # check if there is free space
                bne     $t3, OUT_BUFFER_SIZE, widthIterationEnd
                        li      $v0, 15
                        move    $a0, $a3
                        la      $a1, outputBuffer
                        li      $a2, OUT_BUFFER_SIZE
                        syscall 
                        la      $t8, outputBuffer

                widthIterationEnd:
                addiu   $t1, $t1, 1
                bne     $t1, $s3, widthIteration

        # ADD PADDING BYTES
        addu	$t8, $t8, $s6
    
        j heightIteration

    fileError:
        # print fileErrorMsg
        li      $v0, 4
        la      $a0, fileErrorMsg
        syscall
        j final

    tooWidePictureErr:
        # print fileErrorMsg
        li      $v0, 4
        la      $a0, tooWidePictureErrMsg
        syscall
        j final


    end:
        # store rest of pixels
        la      $t2, outputBuffer
        subu    $t3, $t8, $t2
        li      $v0, 15
        move    $a0, $a3
        la      $a1, outputBuffer
        move    $a2, $t3
        syscall 

        # close files
        li $v0, 16
        move $a0, $s0
        syscall

        li $v0, 16
        move $a0, $a3
        syscall

        # print successMsg
        li      $v0, 4
        la      $a0, successMsg
        syscall

    final:
