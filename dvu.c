#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#define MAX_LIST_MENU 4
#define HEIGHT_OF_LOGS 32

char *delete_enter(char*);
void refresh_active_menu(WINDOW*, int active_string, char menu_list[MAX_LIST_MENU][16]);
unsigned int logs_out(WINDOW*, unsigned int, char*);

typedef struct paper
{
	char name[255];
	int price;
	struct paper *next;
	struct paper *prev;
} PAPER;

int price_length(int price);
int create_list(FILE *f,PAPER **HEAD);
void win_list_update(WINDOW* WIN, PAPER *head);
void refresh_active_list(WINDOW* WIN, PAPER *head, int active_string);
int newActiveWindow(int active_string);

int main(){
	char menu_list[MAX_LIST_MENU][16] = {"Add record", "Delete record", "Save changes", "Quit"};
	WINDOW *menu, *logs, *list, *addMenu;
	PAPER* HEAD = NULL;
	int start_x_menu, start_x_logs, start_x_list, start_x_addMenu;
	int start_y_menu, start_y_logs, start_y_list, start_y_addMenu;
	int w_menu, w_logs, w_list, w_addMenu;
	int h_menu, h_logs, h_list, h_addMenu;

	unsigned int y_log_curr = 0;
	int active_string_menu = 1;
	int active_string_list = 1;
	int activeWindow;
	int key;
	int error = 0;
	int number_of_records;

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
	h_logs = HEIGHT_OF_LOGS;

	start_x_list = start_x_logs + w_logs;
	start_y_list = start_y_menu;
	w_list = w_logs;
	h_list = LINES;

	start_x_addMenu = start_x_menu;
	start_y_addMenu = start_y_menu + h_menu + 1;
	w_addMenu = w_menu;
	h_addMenu = h_menu + 2;

	menu = newwin(h_menu, w_menu, start_y_menu, start_x_menu);
	logs = newwin(h_logs, w_logs, start_y_logs, start_x_logs);
	list = newwin(h_list, w_list, start_y_list, start_x_list);
	addMenu = newwin(h_addMenu, w_addMenu, start_y_addMenu, start_x_addMenu);
	refresh();

	wbkgdset(menu, COLOR_PAIR(1));
	wclear(menu);
	box(menu, 0, 0);
	mvwprintw(menu,0,8,"MENU");
	mvwprintw(menu,2,3,"%s", menu_list[0]);
	mvwprintw(menu,4,3,"%s", menu_list[1]);
	mvwprintw(menu,6,3,"%s", menu_list[2]);
	mvwprintw(menu,8,3,"%s", menu_list[3]);
	refresh_active_menu(menu, active_string_menu, menu_list);
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

	wbkgdset(addMenu, COLOR_PAIR(2));
	wclear(addMenu);
	box(addMenu, 0, 0);
	mvwprintw(addMenu,0,5,"ADD RECORD");
	mvwprintw(addMenu,2,3,"Name of Paper:");
	mvwprintw(addMenu,4,3,"_");
	mvwprintw(addMenu,6,3,"Price:");
	mvwprintw(addMenu,8,3,"_");
	mvwprintw(addMenu,10,1,"Press Enter to add");
	wrefresh(addMenu);

	wattron(logs, COLOR_PAIR(2));
	FILE *f = fopen("source.txt", "r");
	if (f == NULL){
		y_log_curr = logs_out(logs, y_log_curr, "Openning file... Error.");
		error = 1;
	}
	else {
		y_log_curr = logs_out(logs, y_log_curr, "Openning file... Complete.");
		number_of_records = create_list(f, &HEAD);
		win_list_update(list, HEAD);
		y_log_curr = logs_out(logs, y_log_curr, "List has been created");
		wrefresh(list);
	}
	activeWindow = 1;
	while((key = getch()) != KEY_F(1)){
		switch(activeWindow){
			case 1:{ // menu //
        		switch(key){	
            		case KEY_UP:
						active_string_menu--;
						if (active_string_menu == 0)
							active_string_menu = MAX_LIST_MENU;
						refresh_active_menu(menu, active_string_menu, menu_list);
						break;
			
					case KEY_DOWN:
						active_string_menu++;
						if (active_string_menu > MAX_LIST_MENU)
							active_string_menu = 1;
						refresh_active_menu(menu, active_string_menu, menu_list);
						break;
					case '\n':
						activeWindow = newActiveWindow(active_string_menu);
						if (activeWindow == 3)
							refresh_active_list(list, HEAD, active_string_list);
						break;
				}
			break;
			}
			case 3: // list //
				switch(key){
					case KEY_UP:
						active_string_list--;
						if (active_string_list == 0)
							active_string_list = number_of_records;
						refresh_active_list(list, HEAD, active_string_list);
						break;
					case KEY_DOWN:
						active_string_list++;
						if (active_string_list > number_of_records)
							active_string_list = 1;
						refresh_active_list(list, HEAD, active_string_list);
						break;
					case '\n':
						activeWindow = 1;
						break;
				}
			}
	}


	/*


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
	printw("%i) Name: %s price: %d\n", ++i, ptr->name, ptr->price);
	int key;
	while((key = getch()) != KEY_F(1)){
        switch(key){	
            case KEY_RIGHT:
				if (ptr->next != NULL){
					ptr = ptr->next;
					printw("%i) Name: %s price: %d\n", ++i, ptr->name, ptr->price);
				}
				else printw("The end of data.\n");
				break;
			case KEY_LEFT:
				if (ptr->prev != NULL){
					ptr = ptr->prev;
					printw("%i) Name: %s price: %d\n", --i, ptr->name, ptr->price);
				}
				else printw("The end of data.\n");	
			default:
				break;
		}
	}
	/*
	while (ptr){
		printw("Name: %s price: %d\n", ptr->name, ptr->price);
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

void refresh_active_menu(WINDOW *WIN, int active,char menu_list[MAX_LIST_MENU][16]){
	wattron(WIN, COLOR_PAIR(1));
	mvwprintw(WIN,2,3,"%s", menu_list[0]);
	mvwprintw(WIN,4,3,"%s", menu_list[1]);
	mvwprintw(WIN,6,3,"%s", menu_list[2]);
	mvwprintw(WIN,8,3,"%s", menu_list[3]);
	wattron(WIN, COLOR_PAIR(2));
	mvwprintw(WIN,active*2,3,menu_list[active-1]);
	wrefresh(WIN);
	wattron(WIN, COLOR_PAIR(1));
}

unsigned int logs_out(WINDOW* WIN, unsigned int y_log_curr ,char* string){
	if (++y_log_curr > HEIGHT_OF_LOGS/2 - 2){
		wbkgdset(WIN, COLOR_PAIR(2));
		wclear(WIN);
		box(WIN, 0, 0);
		mvwprintw(WIN,0,12,"LOGS");
		wrefresh(WIN);
		y_log_curr = 1;
	}

	mvwprintw(WIN,y_log_curr*2, 1, "%s", string);
	wrefresh(WIN);
	return y_log_curr;
}

int create_list(FILE *f, PAPER **head){
	(*head) = NULL;
	PAPER *current = NULL;
	int i = 0;

	while (feof(f) == 0){
		PAPER *tmp = (PAPER*)malloc(sizeof(PAPER));
		fscanf(f, "%s\n", &tmp->name);
		fscanf(f, "%d\n", &tmp->price);
		if (i++ == 0){ 
			(*head) = tmp;
			tmp->prev = NULL;
		}
		else {
			current->next = tmp;
			tmp->prev = current;
		}
		current = tmp;
		tmp->next = (*head);
		(*head)->prev = current;
	}
	return i;
}

void win_list_update(WINDOW* WIN, PAPER *head){
	wbkgdset(WIN, COLOR_PAIR(1));
	wclear(WIN);
	box(WIN, 0, 0);
	mvwprintw(WIN,0,12,"LIST");

	int y = 2;
	PAPER* ptr = head;
	do {
		mvwprintw(WIN,y,1,"%s", ptr->name);
		for (int i = 1 + strlen(ptr->name); i < 28 - price_length(ptr->price) + 1; i++)
			mvwprintw(WIN, y, i ,".");
		mvwprintw(WIN,y,28 - price_length(ptr->price),"%d$", ptr->price);
		ptr = ptr->next;
		y+=2;
	}
	while (ptr != head);

	wrefresh(WIN);

}


void refresh_active_list(WINDOW* WIN, PAPER *head, int active_string){
	PAPER* ptr = head;
	for(int i = 0; i < active_string - 1; i++)
		ptr = ptr->next;
	win_list_update(WIN, head);
	wattron(WIN, COLOR_PAIR(2));
	mvwprintw(WIN, active_string * 2, 1,"%s", ptr->name);
		for (int i = 1 + strlen(ptr->name); i < 28 - price_length(ptr->price) + 1; i++)
			mvwprintw(WIN, active_string * 2, i ,".");
		mvwprintw(WIN, active_string * 2, 28 - price_length(ptr->price),"%d$", ptr->price);
	wrefresh(WIN);
	wattron(WIN, COLOR_PAIR(1));

}

int price_length(int price){
	int length = 0;
	while (price != 0)
		{
			price /= 10;
			length++;
		}
	return length;
}

int newActiveWindow(int active_string){
	switch(active_string){
		case 1: 
			return 4;
		case 2:
			return 3;
		case 3: 
			return 1;
		case 4: 
			return 1;
	}
}
