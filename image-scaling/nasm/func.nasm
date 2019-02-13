section .data
align 16
v_f_2ones:              dd      1.0, 1.0, 0, 0
mask_for_rgba:          dd      0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF
zero:                   dd      0

section .text
global scale_image

scale_image:
        ; prolog
        push            ebp
        mov             ebp, esp
        sub             esp, 8 ; 8 bytes for local variables
        push            ebx ; registers that need to be saved
        push            edi
        push            esi

        ; dst_width*4   -8
        ; src_width*4   -4
        ; EBP           +0
        ; ret           +4
        ; src_pixels    +8
        ; src_width     +12
        ; src_height    +16
        ; dst_pixels    +20
        ; dst_width     +24
        ; dst_height    +28

        ; store number of bytes in row in src
        mov             eax, [ebp+12]
        shl             eax, 2
        mov             [ebp-4], eax

        ; store number of bytes in row in src
        mov             eax, [ebp+24]
        shl             eax, 2
        mov             [ebp-8], eax

        ; load sizes and convert to float
        ; xmm0  [0 0 src_width src_height]
        movq            xmm0, QWORD [ebp+12]
        pshufd          xmm0, xmm0, 00000001b

        cvtdq2ps        xmm0, xmm0

        ; xmm1 [0 0 dst_width dst_height]
        movq            xmm1, QWORD [ebp+24]
        ; prevent division by 0
        pshufd          xmm1, xmm1, 00000001b
        cvtdq2ps        xmm1, xmm1

        ; xmm0 = [0 0 ratioX ratioY]
        divps           xmm0, xmm1

        sub             DWORD [ebp+12], 1
        sub             DWORD [ebp+16], 1

        ; ecx: y counter = dst_height
        mov             ecx, [ebp+28]

        ; edi - address of end of dst_pixels
        mov             edi, [ebp+20]
        mov             eax, [ebp-8]
        imul            eax, ecx
        add             edi, eax

y_iter:
        sub             ecx, 1
        ; edx: x counter = dst_width
        mov             edx, [ebp+24]
x_iter:
        sub             edx, 1
        movd            xmm1, ecx
        movd            xmm2, edx
        pshufd          xmm2, xmm2, 11110011b
        por             xmm1, xmm2
        push            ecx
        push            edx

        ; xmm1 = [0 0 x_dst y_dst]
        cvtdq2ps        xmm1, xmm1
        mulps           xmm1, xmm0
        ; xmm1 = [0 0 x_src y_src]
        roundps         xmm2, xmm1, 01b
        ; xmm2 = [0 0 _x_src_ _y_src]
        subps           xmm1, xmm2
        ; xmm1 = [0 0 a b]
        movaps          xmm3, [v_f_2ones]
        subps           xmm3, xmm1
        ; xmm3 = [0 0 (1-a) (1-b)]
        pshufd          xmm3, xmm3, 01001111b
        por             xmm1, xmm3
        ; xmm1 = [(1-a) (1-b) a b]

        ; prepare source address
        cvtps2dq        xmm2, xmm2
        ; y src
        movd            eax, xmm2
        pshufd          xmm2, xmm2, 00000001b
        ; x src
        movd            ebx, xmm2 
        ; calc esi
        mov             esi, [ebp + 8]
        ; edx - number of bytes in row in src
        mov             edx, [ebp-4]
        ; number of bytes in row * _y_src_
        mov             ecx, eax
        imul            ecx, edx
        add             esi, ecx
        lea             esi, [esi+ebx*4]

        ; edi - offset to next row
        ; if last row
        cmp             eax, [ebp+16]
        cmove           edx, [zero]
        ; eax no longer used
        ; eax - offset to next column
        mov             eax, 4
        cmp             ebx, [ebp+12]
        cmove           eax, [zero]

        ; pixel 00
        ; split to [R G B A]
        movd            xmm2, DWORD [esi]
        punpcklbw       xmm2, xmm2
        punpcklwd       xmm2, xmm2
        andps           xmm2, [mask_for_rgba]
        cvtdq2ps        xmm2, xmm2

        ; pixel 01
        movd            xmm3, DWORD [esi+eax]
        ; split to [R G B A]
        punpcklbw       xmm3, xmm3
        punpcklwd       xmm3, xmm3
        andps           xmm3, [mask_for_rgba]
        cvtdq2ps        xmm3, xmm3

        ; move to next row
        add             esi, edx

        ; pixel 10
        ; split to [R G B A]
        movd            xmm4, DWORD [esi]
        punpcklbw       xmm4, xmm4
        punpcklwd       xmm4, xmm4
        andps           xmm4, [mask_for_rgba]
        cvtdq2ps        xmm4, xmm4

        ; pixel 11
        ; split to [R G B A]
        movd            xmm5, DWORD [esi+eax]
        punpcklbw       xmm5, xmm5
        punpcklwd       xmm5, xmm5
        andps           xmm5, [mask_for_rgba]
        cvtdq2ps        xmm5, xmm5

        ; xmm2 pixel 00
        ; xmm3 pixel 01
        ; xmm4 pixel 10
        ; xmm5 pixel 11

        ; (1-a)*00
        pshufd          xmm6, xmm1, 11111111b
        mulps           xmm2, xmm6
        ; (1-a)*10
        mulps           xmm4, xmm6
        
        ; a * 01
        pshufd          xmm6, xmm1, 01010101b
        mulps           xmm3, xmm6
        ; a * 11
        mulps           xmm5, xmm6

        ; (1-a)*00 + a * 01
        addps           xmm2, xmm3
        ; (1-a)*10 + a * 11
        addps           xmm4, xmm5

        ; (1-b) * ((1-a)*00 + a * 01)
        pshufd          xmm6, xmm1, 10101010b
        mulps           xmm2, xmm6

        ; b * ((1-a)*10 + a * 11)
        pshufd          xmm6, xmm1, 00000000b
        mulps           xmm4, xmm6

        addps           xmm2, xmm4

        cvtps2dq        xmm2, xmm2
        ; reverse split
        packusdw        xmm2, xmm2
        packuswb        xmm2, xmm2
        movd            ebx, xmm2

        ; prepare destination address
        sub             edi, 4
        mov             DWORD [edi], ebx

        pop             edx
        pop             ecx

        ; end of x_iter
        test            edx, edx
        jnz             x_iter
        ; end of y_iter
        test            ecx, ecx
        jnz             y_iter

        ; epilog
        pop             esi
        pop             edi
        pop             ebx
        add             esp, 8
        mov             esp, ebp
        pop             ebp
        ret