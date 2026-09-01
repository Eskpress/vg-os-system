#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

// Retorna o caractere correspondente ao scancode, já considerando
// shift/capslock. Retorna 0 se for tecla sem caractere (ex: F1) ou
// scancode de "solta tecla" tratado internamente.
char teclado_ler(void);

#endif