%include        'functions.asm'

SECTION .bss
    is_prime    resb    100000              ; 1 bajt per number

SECTION .text
global  _start
 
_start:
    mov     ecx, 0
    mov     byte [is_prime + ecx], 0
    inc     ecx
    mov     byte [is_prime + ecx], 0        ; 1 and 0 are not prime

fill_with_ones:
    inc     ecx
    mov     byte [is_prime + ecx], 1        ; initialize: mark as prime 
    cmp     ecx, 100000
    jne     fill_with_ones

    mov     ecx, 1                          ; resetuj ecx po zakonczeniu fill_with_ones

sieve:
    inc     ecx                             ; zaczynamy od 2
    cmp     ecx, 100000
    je      sieve_done
    cmp     byte [is_prime + ecx], 1        ; sprawdzamy czy liczba jest pierwsza
    jne     sieve                           ; jesli nie; skip loop
    mov     ebx, ecx                        ; zaczynamy od liczby pierwszej
    add     ebx, ecx                        ; pierwsza wielokrotnosc do wykluczenia
mark_multiples:
    cmp     ebx, 100000
    jge     sieve                           ; ebx >= 100000 skip to sieve
    mov     byte [is_prime + ebx], 0        ; oznacz jako niepierwsza
    add     ebx, ecx                        ; nastepna wielokrotnosc
    jmp     mark_multiples


sieve_done:
    mov     ecx, 0                          ; przenies licznik na 0

print_primes:   
    inc     ecx
    cmp     ecx, 100000
    je      end_program
    cmp     byte [is_prime + ecx], 1
    jne     print_primes                    ; if number not prime, skip printing

print_num:
    mov     eax, ecx                        ; print output
    call    iprintLF
    jmp     print_primes

end_program:
    call    quit