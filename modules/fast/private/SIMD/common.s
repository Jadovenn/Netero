section .text
bits 64

;; ------------------------------------
;;  WIN64 common section
;; ------------------------------------

%ifdef WIN64

%macro alloc_framestack
	push	rbp
	mov		rbp, rsp
	sub		rsp, 28h
%endmacro

%macro free_framestack
	mov		rsp, rbp
	pop		rbp
%endmacro

%endif ;; WIN64

;; ------------------------------------
;;  ELF64 common section
;; ------------------------------------

%ifdef ELF64
%endif ;; ELF64

;; ------------------------------------
;;  MACHO64 common section
;; ------------------------------------

%ifdef MACHO64
%endif ;; MACHO64

