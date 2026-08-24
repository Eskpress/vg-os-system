static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void kernel_main(void) {
    volatile char *video_memory = (volatile char*)0xB8000;
    
    const char* logo[] = {
        "  /$$      /$$  /$$$$$$            /$$$$$$  /$$$$$$ ",
        " | $$    | $$ /$$__  $$          /$$__  $$ /$$__  $$",
        " | $$    | $$| $$  \\__/         | $$  \\ $$| $$  \\__/",
        " |  $$ / $$/ | $$ /$$$$         | $$  | $$|  $$$$$$ ",
        "  \\  $$ $$/  | $$|_  $$         | $$  | $$ \\____  $$",
        "   \\  $$$/   | $$  \\ $$         | $$  | $$ /$$  \\ $$",
        "    \\  $/    |  $$$$$$/         |  $$$$$$/|  $$$$$$/",
        "     \\_/      \\______/           \\______/  \\______/ "
    };

    for (int linha = 0; linha < 8; linha++) {
        int coluna = 0;
        while (logo[linha][coluna] != '\0') {
            int posicao = (linha * 80 + coluna) * 2;
            video_memory[posicao] = logo[linha][coluna];
            video_memory[posicao + 1] = 0x0A;
            coluna++;
        }
    }

    const char* mensagem = "Sistema Operacional VG OS - Versao 0.025)";
    int coluna_msg = 15;
    int i_msg = 0;
    
    while (mensagem[i_msg] != '\0') {
        int posicao = (10 * 80 + coluna_msg) * 2;
        video_memory[posicao] = mensagem[i_msg];
        video_memory[posicao + 1] = 0x0F;
        coluna_msg++;
        i_msg++;
    }

    const char* prompt = "root@vgos:~# "; 
    int cursor_x = 0;
    int cursor_y = 12; 
    
    while (prompt[cursor_x] != '\0') {
        int posicao = (cursor_y * 80 + cursor_x) * 2;
        video_memory[posicao] = prompt[cursor_x];
        video_memory[posicao + 1] = 0x0F;
        cursor_x++;
    }

    const char teclado_map[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
      '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
      '*',   0, ' ',   0
    };

    while (1) {
        int pos_cursor = (cursor_y * 80 + cursor_x) * 2;
        
        video_memory[pos_cursor] = '_';
        video_memory[pos_cursor + 1] = 0x8A;

        if (inb(0x64) & 1) { 
            unsigned char scancode = inb(0x60); 
            
            if ((scancode & 0x80) == 0) { 
                char letra = teclado_map[scancode]; 
                
                if (letra == '\b') {
                    if (cursor_x > 13) {
                        video_memory[pos_cursor] = ' '; 
                        video_memory[pos_cursor + 1] = 0x0F;
                        cursor_x--; 
                        pos_cursor = (cursor_y * 80 + cursor_x) * 2;
                        video_memory[pos_cursor] = ' '; 
                        video_memory[pos_cursor + 1] = 0x0F;
                    }
                } 
                else if (letra == '\n') {
                    video_memory[pos_cursor] = ' '; 
                    video_memory[pos_cursor + 1] = 0x0F;
                    
                    cursor_y++; 
                    if (cursor_y > 24) cursor_y = 0; 
                    cursor_x = 0; 
                    
                    int i_p = 0;
                    while (prompt[i_p] != '\0') {
                        int pos = (cursor_y * 80 + cursor_x) * 2;
                        video_memory[pos] = prompt[i_p];
                        video_memory[pos + 1] = 0x0F;
                        cursor_x++;
                        i_p++;
                    }
                } 
                else if (letra != 0 && cursor_x < 79) {
                    video_memory[pos_cursor] = letra;
                    video_memory[pos_cursor + 1] = 0x0F;
                    cursor_x++;
                }
            }
        }
    }
}
