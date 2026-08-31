#include "font.h"
#include "logo.h"
#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

const char kbd_US[128] = {0,    27,  '1', '2',  '3',  '4',  '5', '6', '7',  '8',
                          '9',  '0', '-', '=',  '\b', '\t', 'q', 'w', 'e',  'r',
                          't',  'y', 'u', 'i',  'o',  'p',  '[', ']', '\n', 0,
                          'a',  's', 'd', 'f',  'g',  'h',  'j', 'k', 'l',  ';',
                          '\'', '`', 0,   '\\', 'z',  'x',  'c', 'v', 'b',  'n',
                          'm',  ',', '.', '/',  0,    '*',  0,   ' '};

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

void desenhar_logo(uint32_t *fb, uint32_t pitch, int start_x, int start_y) {
  int index = 0;
  for (uint32_t y = 0; y < logo_height; y++) {
    for (uint32_t x = 0; x < logo_width; x++) {
      uint32_t cor = logo_pixels[index++];
      if (cor != 0x00000000)
        fb[((start_y + y) * pitch / 4) + (start_x + x)] = cor;
    }
  }
}

// Desenha uma única letra (escala 2x)
void desenhar_char(char c, int x, int y, uint32_t cor, uint32_t *fb,
                   uint32_t pitch) {
  if (c < 0 || c > 127)
    return;
  char *bitmap = font8x8_basic[(int)c];

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (bitmap[row] & (1 << col)) {
        for (int dy = 0; dy < 2; dy++)
          for (int dx = 0; dx < 2; dx++)
            fb[((y + row * 2 + dy) * pitch / 4) + (x + col * 2 + dx)] = cor;
      } else {
        for (int dy = 0; dy < 2; dy++)
          for (int dx = 0; dx < 2; dx++)
            fb[((y + row * 2 + dy) * pitch / 4) + (x + col * 2 + dx)] =
                0x00000000;
      }
    }
  }
}

// Desenha uma frase inteira reaproveitando o desenhar_char
void desenhar_string(const char *str, int start_x, int start_y, uint32_t cor,
                     uint32_t *fb, uint32_t pitch) {
  int x = start_x;
  for (int i = 0; str[i] != '\0'; i++) {
    desenhar_char(str[i], x, start_y, cor, fb, pitch);
    x += 16;
  }
}

void kernel_main(multiboot_info_t *mbd) {
  if (!(mbd->flags & (1 << 12)))
    return;

  uint32_t *fb = (uint32_t *)(uintptr_t)mbd->framebuffer_addr;
  uint32_t pitch = mbd->framebuffer_pitch;
  uint32_t width = mbd->framebuffer_width;
  uint32_t height = mbd->framebuffer_height;

  // Fundo preto
  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++)
      fb[(y * pitch / 4) + x] = 0x00000000;
  }

  // Desenha a imagem da logo
  int logo_x = (width - logo_width) / 2;
  int logo_y = (height - logo_height) / 3;
  desenhar_logo(fb, pitch, logo_x, logo_y);

  int text_y = logo_y + logo_height + 30;
  desenhar_string("Sistema Operacional VG OS Versao 0.3.1 beta", 50, text_y,
                  0x00FFFFFF, fb, pitch);

  text_y += 30; // Pula linha
  desenhar_string("root@vgos:~# ", 50, text_y, 0x0000FF00, fb, pitch);

  // Configura o cursor para iniciar exatamente depois da palavra "root@vgos:~#
  // " "root@vgos:~# " tem 13 caracteres. 13 * 16 pixels de largura = 208
  int cursor_base_x = 50 + 208;
  int cursor_x = cursor_base_x;
  int cursor_y = text_y;

  // Loop do Teclado
  // O Loop do Terminal
  while (1) {
    if (inb(0x64) & 1) {
      uint8_t scancode = inb(0x60);

      if (!(scancode & 0x80)) {
        char tecla = kbd_US[scancode];

        if (tecla != 0) {
          if (tecla == '\b') {
            // Lógica do Backspace (impede apagar o prompt)
            if (cursor_x > cursor_base_x) {
              cursor_x -= 16;
              desenhar_char(' ', cursor_x, cursor_y, 0x00FFFFFF, fb, pitch);
            }
          } else if (tecla == '\n') {
            // Lógica do Enter: Pula a linha e gera um NOVO prompt
            cursor_y += 24;
            desenhar_string("root@vgos:~# ", 50, cursor_y, 0x0000FF00, fb,
                            pitch);
            cursor_base_x = 50 + 208;
            cursor_x = cursor_base_x;
          } else {
            if (cursor_x + 16 >= width - 50) {
              cursor_y += 24;
              cursor_base_x = 50;
              cursor_x = 50;
            }
            desenhar_char(tecla, cursor_x, cursor_y, 0x00FFFFFF, fb, pitch);
            cursor_x += 16;
          }
        }
      }
    }
  }
}