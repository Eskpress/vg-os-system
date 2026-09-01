#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define TECLA_NENHUMA 0
#define TECLA_CTRL_L -1 // limpar terminal
#define TECLA_CTRL_C -2 // copiar linha atual
#define TECLA_CTRL_V -3 // colar

// Retorna o caractere correspondente ao scancode, já considerando
// shift/capslock. Retorna 0 se for tecla sem caractere (ex: F1) ou
// scancode de "solta tecla" tratado internamente.
// Retorna: 0 = nada, >0 = código ASCII do caractere, <0 = comando especial
int teclado_ler(void);

#endif