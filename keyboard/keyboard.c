#include "keyboard.h"
#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static const char kbd_US[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

// Mesma disposição, só com as letras maiúsculas e símbolos "shiftados"
static const char kbd_US_shift[128] = {
    0,   27,  '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '"',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};

#define SC_SHIFT_L 0x2A
#define SC_SHIFT_R 0x36
#define SC_SHIFT_L_UP 0xAA
#define SC_SHIFT_R_UP 0xB6
#define SC_CAPSLOCK 0x3A
#define SC_CTRL_L 0x1D
#define SC_CTRL_L_UP 0x9D
#define SC_L 0x26
#define SC_C 0x2E
#define SC_V 0x2F

static int shift_pressionado = 0;
static int capslock_ativo = 0;
static char ctrl_pressionado = 0;

static char aplica_case(char c) {
  // Caps Lock só afeta letras; Shift já resolve símbolos via kbd_US_shift
  int maiuscula_pedida =
      shift_pressionado ^
      capslock_ativo; // XOR: um dos dois inverte, os dois juntos cancelam
  if (c >= 'a' && c <= 'z')
    return maiuscula_pedida ? (c - 32) : c;
  if (c >= 'A' && c <= 'Z')
    return maiuscula_pedida ? c : (c + 32);
  return c;
}

int teclado_ler(void) {
  if (!(inb(0x64) & 1))
    return TECLA_NENHUMA;

  uint8_t scancode = inb(0x60);

  switch (scancode) {
  case SC_SHIFT_L:
  case SC_SHIFT_R:
    shift_pressionado = 1;
    return TECLA_NENHUMA;
  case SC_SHIFT_L_UP:
  case SC_SHIFT_R_UP:
    shift_pressionado = 0;
    return TECLA_NENHUMA;
  case SC_CAPSLOCK:
    capslock_ativo = !capslock_ativo;
    return TECLA_NENHUMA;
  case SC_CTRL_L:
    ctrl_pressionado = 1;
    return TECLA_NENHUMA;
  case SC_CTRL_L_UP:
    ctrl_pressionado = 0;
    return TECLA_NENHUMA;
  }

  if (scancode & 0x80)
    return TECLA_NENHUMA; // solta de outra tecla, ignora

  if (ctrl_pressionado) {
    switch (scancode) {
    case SC_L:
      return TECLA_CTRL_L;
    case SC_C:
      return TECLA_CTRL_C;
    case SC_V:
      return TECLA_CTRL_V;
    }
    return TECLA_NENHUMA; // outras combinações com Ctrl: ignoradas por enquanto
  }

  char base = shift_pressionado ? kbd_US_shift[scancode] : kbd_US[scancode];
  return (int)(unsigned char)aplica_case(base);
}