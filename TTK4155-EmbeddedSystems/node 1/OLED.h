#ifndef OLED_H_
#define OLED_H_

void oled_init();
void oled_reset();
void oled_home();
void oled_goto_line(int line);
void oled_goto_column(int column);
void oled_clear_line(int line);
void oled_pos(int row, int column);
void oled_print_char(char c);
void oled_print(char* c);

#endif