section .text
bits 64
default rel

;; void LowPassFilter(float* buffIn, float* buffOut, size_t length, float freqIn, float freqOut);

global LowPassFilter

;; - Paramaters ----
;; rdi -> float*
;; rsi -> float*
;; rdx -> size_t
;; xmm0 -> float
;; xmm1 -> float
;; return void

LowPassFilter:
    ret
