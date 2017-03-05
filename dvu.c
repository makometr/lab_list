#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
char *delete_enter(char*); 
typedef struct person
{
	char name[255];
	int age;
	struct person *next;
	struct person *prev;
} PERSON;

int main(){
	WINDOW *menu, *logs, *list;
	int start_x_menu, start_x_logs, start_x_list;
	int start_y_menu, start_y_logs, start_y_list;
	int w_menu, w_logs, w_list;
	int h_menu, h_logs, h_list;

	initscr();
	cbreak();
    noecho();
    curs_set(FALSE);
	keypad(stdscr, TRUE);
    start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair (2, COLOR_GREEN, COLOR_BLACK);

	start_x_menu = 0;
	start_y_menu = 0;
	w_menu = 20;
	h_menu = 11;

	start_x_logs = w_menu;
	start_y_logs = start_y_menu;
	w_logs = 30;
	h_logs = 25;

	start_x_list = start_x_logs + w_logs;
	start_y_list = start_y_menu;
	w_list = w_logs;
	h_list = LINES;

	menu = newwin(h_menu, w_menu, start_y_menu, start_x_menu);
	logs = newwin(h_logs, w_logs, start_y_logs, start_x_logs);
	list = newwin(h_list, w_list, start_y_list, start_x_list);
	refresh();

	wbkgdset(menu, COLOR_PAIR(1));
	wclear(menu);
	box(menu, 0, 0);
	mvwprintw(menu,0,8,"MENU");
	mvwprintw(menu,2,4,"Add record");
	mvwprintw(menu,4,4,"Delete record");
	mvwprintw(menu,6,4,"Save changes");
	mvwprintw(menu,8,4,"Quit");
	wrefresh(menu);

	wbkgdset(logs, COLOR_PAIR(2));
	wclear(logs);
	box(logs, 0, 0);
	mvwprintw(logs,0,12,"LOGS");
	wrefresh(logs);


	wbkgdset(list, COLOR_PAIR(1));
	wclear(list);
	box(list, 0, 0);
	mvwprintw(list,0,12,"LIST");
	wrefresh(list);


	/*printw("Openning file... ");
	FILE *f = fopen("source.txt", "r");
	if (f == NULL){
		perror("\nError");
		return 0;
	}
	else 
		printw("Complete\n"	);

	PERSON *head = NULL;
	PERSON *current = NULL;
	int i = 0;
	int data_error = 0;

	while (feof(f) == 0){
		PERSON *tmp = (PERSON*)malloc(sizeof(PERSON));
		fgets(tmp->name, 255, f);
		strcpy(tmp->name, delete_enter(tmp->name));
		if (fscanf(f, "%d\n", &tmp->age) == EOF){
			printw("Problem with data-reading. Check file source.txt\n");
			data_error = 1;
		}
		if (i++ == 0){ 
			head = tmp;
			tmp->prev = NULL;
		}
		else {
			current->next = tmp;
			tmp->prev = current;
		}
		current = tmp;
		tmp->next = NULL;
	}

	printw("Closing file... ");
	if (fclose(f) != 0)
		perror("\nError");
	else 
		printw("Complete\n"	);

	if (data_error != 0)
		return 0;
	printw("Press Esc to exit.\n");
	printw("Press F2 to add a new record.\n");
	printw("Press F3 to delete.\n");
	printw("Press the right-arrow to see the next element.\n");
	printw("Press the left-arrow key to see the next element.\n");
	PERSON *ptr = head;
	i = 0;
	printw("%i) Name: %s Age: %d\n", ++i, ptr->name, ptr->age);
	int key;
	while((key = getch()) != KEY_F(1)){
        switch(key){	
            case KEY_RIGHT:
				if (ptr->next != NULL){
					ptr = ptr->next;
					printw("%i) Name: %s Age: %d\n", ++i, ptr->name, ptr->age);
				}
				else printw("The end of data.\n");
				break;
			case KEY_LEFT:
				if (ptr->prev != NULL){
					ptr = ptr->prev;
					printw("%i) Name: %s Age: %d\n", --i, ptr->name, ptr->age);
				}
				else printw("The end of data.\n");	
			default:
				break;
		}
	}
	/*
	while (ptr){
		printw("Name: %s Age: %d\n", ptr->name, ptr->age);
		ptr = ptr->next;
	}
	*/
	getch();
	endwin();
	return 0;
}

char* delete_enter(char* string){
	string[strlen(string)-1] = '\0';
	string[strlen(string)] = ' ';
	return string;
}