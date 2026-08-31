#include "logo.h"
#include <stdint.h>

typedef struct {
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  uint32_t syms[4];
  uint32_t mmap_length;
  uint32_t mmap_addr;
  uint32_t drives_length;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
  uint8_t framebuffer_type;
  uint8_t color_info[6];
} __attribute__((packed)) multiboot_info_t;

static inline unsigned char inb(unsigned short port) {
  unsigned char ret;
  __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

/*
 * Cobertura mínima: A-Z, 0-9, espaço e os símbolos usados no
 * prompt/mensagens (@ : ~ # - . _ , ! ( ) = /).
 * Cada glifo é 8 bytes, 1 bit por pixel (bit 7 = pixel mais à
 * esquerda da linha).
 */

typedef struct {
  char c;
  uint8_t rows[8];
} glifo_t;

static const glifo_t FONTE[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {'A', {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00}},
    {'B', {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00}},
    {'C', {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00}},
    {'D', {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00}},
    {'E', {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00}},
    {'F', {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00}},
    {'G', {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00}},
    {'H', {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00}},
    {'I', {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}},
    {'J', {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00}},
    {'K', {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00}},
    {'L', {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00}},
    {'M', {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00}},
    {'N', {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00}},
    {'O', {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}},
    {'P', {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00}},
    {'Q', {0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00}},
    {'R', {0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66, 0x00}},
    {'S', {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00}},
    {'T', {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}},
    {'U', {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}},
    {'V', {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00}},
    {'W', {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}},
    {'X', {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00}},
    {'Y', {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00}},
    {'Z', {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00}},
    {'0', {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}},
    {'1', {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}},
    {'2', {0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00}},
    {'3', {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}},
    {'4', {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00}},
    {'5', {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}},
    {'6', {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00}},
    {'7', {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00}},
    {'8', {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00}},
    {'9', {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00}},
    {'@', {0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x62, 0x3C, 0x00}},
    {':', {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}},
    {'~', {0x00, 0x00, 0x30, 0x7A, 0x0E, 0x00, 0x00, 0x00}},
    {'#', {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}},
    {'-', {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00}},
    {'.', {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}},
    {'_', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00}},
    {',', {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30}},
    {'!', {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00}},
    {'(', {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00}},
    {')', {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00}},
    {'=', {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00}},
    {'/', {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}},
};
#define FONTE_QTD (sizeof(FONTE) / sizeof(FONTE[0]))

static const uint8_t *buscar_glifo(char c) {
  if (c >= 'a' && c <= 'z')
    c -= 32; // minúscula -> maiúscula
  for (uint32_t i = 0; i < FONTE_QTD; i++) {
    if (FONTE[i].c == c)
      return FONTE[i].rows;
  }
  return FONTE[0].rows; // desconhecido = espaço em branco
}

static uint32_t *FB;
static uint32_t PITCH_PX;
static uint32_t TELA_W, TELA_H;

static inline void plotar(int x, int y, uint32_t cor) {
  if (x < 0 || y < 0 || (uint32_t)x >= TELA_W || (uint32_t)y >= TELA_H)
    return;
  FB[y * PITCH_PX + x] = cor;
}

void desenhar_logo(int start_x, int start_y) {
  int index = 0;
  for (uint32_t y = 0; y < logo_height; y++) {
    for (uint32_t x = 0; x < logo_width; x++) {
      uint32_t cor = logo_pixels[index];
      if (cor != 0x00000000) {
        plotar(start_x + x, start_y + y, cor);
      }
      index++;
    }
  }
}

void desenhar_char(int x, int y, char c, uint32_t cor, int escala) {
  const uint8_t *glifo = buscar_glifo(c);
  for (int linha = 0; linha < 8; linha++) {
    uint8_t bits = glifo[linha];
    for (int col = 0; col < 8; col++) {
      if (bits & (0x80 >> col)) {
        for (int sy = 0; sy < escala; sy++)
          for (int sx = 0; sx < escala; sx++)
            plotar(x + col * escala + sx, y + linha * escala + sy, cor);
      }
    }
  }
}

// Desenha uma string; retorna a largura em pixels ocupada
int desenhar_texto(int x, int y, const char *texto, uint32_t cor, int escala) {
  int cx = x;
  while (*texto) {
    desenhar_char(cx, y, *texto, cor, escala);
    cx += 8 * escala;
    texto++;
  }
  return cx - x;
}

// Apaga uma área retangular (usado para apagar 1 caractere no backspace/cursor)
void apagar_area(int x, int y, int largura, int altura, uint32_t cor_fundo) {
  for (int j = 0; j < altura; j++)
    for (int i = 0; i < largura; i++)
      plotar(x + i, y + j, cor_fundo);
}

static const char TECLADO_MAP[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' ', 0};

#define COR_FUNDO 0x00000000
#define COR_TEXTO 0xFF00FF7F
#define ESCALA_TEXTO 1

void kernel_main(multiboot_info_t *mbd) {
  if (!(mbd->flags & (1 << 12))) {
    return;
  }

  FB = (uint32_t *)(uintptr_t)mbd->framebuffer_addr;
  PITCH_PX = mbd->framebuffer_pitch / 4;
  TELA_W = mbd->framebuffer_width;
  TELA_H = mbd->framebuffer_height;

  // Fundo preto
  for (uint32_t y = 0; y < TELA_H; y++)
    for (uint32_t x = 0; x < TELA_W; x++)
      plotar(x, y, COR_FUNDO);

  // 1) Imagem (logo.h) centralizada no topo
  int logo_x = (TELA_W - logo_width) / 2;
  int logo_y = 20;
  desenhar_logo(logo_x, logo_y);

  // 2) Logo escrita, abaixo da imagem
  const char *logo_texto = "VG OS";
  const int logo_texto_escala = 4;
  int logo_texto_largura = 8 * logo_texto_escala * 5;
  int texto_y = logo_y + logo_height + 20;
  desenhar_texto((TELA_W - logo_texto_largura) / 2, texto_y, logo_texto,
                 COR_TEXTO, logo_texto_escala);

  int msg_y = texto_y + 8 * logo_texto_escala + 20;
  desenhar_texto(20, msg_y, "SISTEMA OPERACIONAL VG OS - VERSAO 0.03 Alpha",
                 COR_TEXTO, 1);

  const char *prompt = "ROOT@VGOS:~# ";
  int prompt_x = 20;
  int linha_y = msg_y + 30;
  int cursor_x = prompt_x + desenhar_texto(prompt_x, linha_y, prompt, COR_TEXTO,
                                           ESCALA_TEXTO);
  int limite_x = prompt_x; // não apaga o prompt no backspace

  int charw = 8 * ESCALA_TEXTO;

  while (1) {
    // "cursor" piscando de forma simples (bloco sólido)
    apagar_area(cursor_x, linha_y, charw, 8 * ESCALA_TEXTO, 0x00888888);

    if (inb(0x64) & 1) {
      unsigned char scancode = inb(0x60);

      if ((scancode & 0x80) == 0) { // tecla pressionada (não solta)
        char letra = TECLADO_MAP[scancode];

        if (letra == '\b') {
          if (cursor_x > prompt_x + (int)(charw * 13)) {
            apagar_area(cursor_x, linha_y, charw, 8 * ESCALA_TEXTO, COR_FUNDO);
            cursor_x -= charw;
            apagar_area(cursor_x, linha_y, charw, 8 * ESCALA_TEXTO, COR_FUNDO);
          }
        } else if (letra == '\n') {
          apagar_area(cursor_x, linha_y, charw, 8 * ESCALA_TEXTO, COR_FUNDO);
          linha_y += 8 * ESCALA_TEXTO + 2;
          if ((uint32_t)linha_y > TELA_H - 16)
            linha_y = msg_y + 30; // volta ao topo do console (simples)
          cursor_x = prompt_x + desenhar_texto(prompt_x, linha_y, prompt,
                                               COR_TEXTO, ESCALA_TEXTO);
        } else if (letra != 0) {
          apagar_area(cursor_x, linha_y, charw, 8 * ESCALA_TEXTO, COR_FUNDO);
          desenhar_char(cursor_x, linha_y, letra, COR_TEXTO, ESCALA_TEXTO);
          cursor_x += charw;
        }
      }
    }
  }
}
