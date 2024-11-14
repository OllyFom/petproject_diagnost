/*#include <ncurses.h>
#include <stdio.h>
#include  <panel.h>
int main(){
	initscr(); //инициализация нашего окошка
	int row, col;
	//getmaxyx(stdscr, row, col);
	//move(row/2, col/2);
	//printw("Hello, World!");
	WINDOW *win1 = newwin(10, 20, 2, 2);
	PANEL *panel1 = new_panel(win1);
	box(win1, 0, 0);
	update_panels();
	doupdate();
	del_panel(panel1);
	refresh();
	getch();
	endwin();
}*/
#include <ncurses.h>
#include <panel.h>

int main() {
  initscr();
  start_color(); 
  // ... инициализация цветов ...
  
  // Создание панели
  WINDOW *win1 = newwin(10, 20, 2, 2);
  PANEL *panel1 = new_panel(win1); 
  // ...  задание цвета ...
  box(win1, 0, 0);  // Отрисовка рамки
  
  // Создание другой панели
  WINDOW *win2 = newwin(15, 30, 5, 5);
  PANEL *panel2 = new_panel(win2); 
  // ...  задание цвета ...
  box(win2, 0, 0);
  
  // Установка порядка панелей
  update_panels(); 
  doupdate();
  
  // ... Взаимодействие с пользователем ...
  
  // Удаление панелей
  del_panel(panel1); 
  del_panel(panel2); 
  endwin();
  return 0;
}
