#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#define MAX_LIST_MENU 4
#define HEIGHT_OF_LOGS 32
#define MAX_LEGTH_OF_NAME_PAPER 24

void refresh_active_menu(WINDOW*, int active_string, char menu_list[MAX_LIST_MENU][16]);
void logs_out(WINDOW* WIN, char* string);

typedef struct paper
{
	char name[MAX_LEGTH_OF_NAME_PAPER];
	int price;
	struct paper *next;
	struct paper *prev;
} PAPER;

int price_length(int price);
int create_list(FILE *f,PAPER **HEAD);
void win_list_update(WINDOW* WIN, PAPER *head, int);
void refresh_active_list(WINDOW* WIN, PAPER *head, int active_string, int number_of_records);
int newActiveWindow(int active_string);
void draw_addMenu(WINDOW* WIN);
void getNewRecordAddMenu(WINDOW *WIN, PAPER **head, WINDOW *logs, int);
void delete_record(PAPER **head, WINDOW *LOGS, int active_string);
void saveChanges(FILE **f, PAPER *head);

int main(){
	char menu_list[MAX_LIST_MENU][16] = {"Add record", "Delete record", "Save changes", "Quit"};
	WINDOW *menu, *logs, *list, *addMenu;
	PAPER* HEAD = NULL;
	int start_x_menu, start_x_logs, start_x_list, start_x_addMenu;
	int start_y_menu, start_y_logs, start_y_list, start_y_addMenu;
	int w_menu, w_logs, w_list, w_addMenu;
	int h_menu, h_logs, h_list, h_addMenu;

	int active_string_menu = 1;
	int active_string_list = 1;
	int activeWindow;
	int key;
	int number_of_records;
	bool is_quite;
	bool is_change = FALSE;

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

	wattron(logs, COLOR_PAIR(2));
	FILE *f = fopen("source.txt", "r");
	if (f == NULL){
		logs_out(logs, "Openning file... Error.");
		is_quite = TRUE;
		halfdelay(10);
	}
	else {
		logs_out(logs, "Openning file... Complete.");
		number_of_records = create_list(f, &HEAD);
		win_list_update(list, HEAD, number_of_records);
		logs_out(logs, "List has been created");
		wrefresh(list);
	}
	activeWindow = 1;
	while((key = getch()) != KEY_F(1) && is_quite != TRUE){
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
						if (active_string_menu == 4){
							is_quite = TRUE;
							halfdelay(1);
							break;
						}
						if (active_string_menu == 3){
							if (is_change == FALSE){
								logs_out(logs, "No changes to save.");
							}
							else {
								saveChanges(&f, HEAD);
								logs_out(logs, "Changes are saved.");
								is_change = FALSE;
							}
							break;
						}
						activeWindow = newActiveWindow(active_string_menu);
						if (activeWindow == 3){
								if (number_of_records == 0){
									logs_out(logs, "Nothing to delete.");
									activeWindow = 1;
									break;
								}
								else {
									refresh_active_list(list, HEAD, active_string_list, number_of_records);
									is_change = TRUE;
								}
						}
						if (activeWindow == 4){
							draw_addMenu(addMenu);
							getNewRecordAddMenu(addMenu, &HEAD, logs, number_of_records);
							number_of_records++;
							is_change = TRUE;
							win_list_update(list, HEAD, number_of_records);
							activeWindow = 1;
						}
						break;
				}
			break;
			}
			case 3:{ // list //
				switch(key){
					case KEY_UP:
						active_string_list--;
						if (active_string_list == 0)
							active_string_list = number_of_records;
						refresh_active_list(list, HEAD, active_string_list, number_of_records);
						break;
					case KEY_DOWN:
						active_string_list++;
						if (active_string_list > number_of_records)
							active_string_list = 1;
						refresh_active_list(list, HEAD, active_string_list, number_of_records);
						break;
					case '\n':
						delete_record(&HEAD, logs,  active_string_list);
						number_of_records--;
						active_string_list = 1;
						win_list_update(list, HEAD, number_of_records);
						activeWindow = 1;
						break;
				}
			break;
			}
		}
	}
	endwin();
	return 0;
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

void logs_out(WINDOW* WIN, char* string){
	static int y_log_curr = 0;
	if (++y_log_curr > HEIGHT_OF_LOGS/2 - 2){
		wbkgdset(WIN, COLOR_PAIR(2));
		wclear(WIN);
		box(WIN, 0, 0);
		mvwprintw(WIN,0,12,"LOGS");
		wrefresh(WIN);
		y_log_curr = 1;
	}

	mvwprintw(WIN,y_log_curr * 2, 1, "%s", string);
	wrefresh(WIN);
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

void win_list_update(WINDOW* WIN, PAPER *head, int number_of_records){
	wbkgdset(WIN, COLOR_PAIR(1));
	wclear(WIN);
	box(WIN, 0, 0);
	mvwprintw(WIN,0,12,"LIST");
	wrefresh(WIN);
	if (number_of_records == 0)
		return ;
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


void refresh_active_list(WINDOW* WIN, PAPER *head, int active_string, int number_of_records){
	if (active_string == 0)
		return ;
	win_list_update(WIN, head, number_of_records);
	PAPER* ptr = head;
	for(int i = 0; i < active_string - 1; i++)
		ptr = ptr->next;
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

void draw_addMenu(WINDOW* WIN){
		wbkgdset(WIN, COLOR_PAIR(1));
		wclear(WIN);
		box(WIN, 0, 0);
		mvwprintw(WIN,0,5,"ADD RECORD");
		mvwprintw(WIN,2,3,"Name of Paper:");
		mvwprintw(WIN,4,3,"_");
		mvwprintw(WIN,6,3,"Price:");
		mvwprintw(WIN,8,3,"_");
		mvwprintw(WIN,10,1,"Press Enter to add");
		wrefresh(WIN);
	}

void getNewRecordAddMenu(WINDOW *WIN, PAPER **head, WINDOW *LOG, int number_of_records){
	echo();
	char new_name[MAX_LEGTH_OF_NAME_PAPER];
	int new_price;
	int OK_record = 0;

	wattron(WIN, COLOR_PAIR(1));

	mvwgetstr(WIN, 4, 3, new_name);
	box(WIN, 0, 0);
	mvwprintw(WIN,0,5,"ADD RECORD");
	wrefresh(WIN);
	while(strlen(new_name) == 0){
		logs_out(LOG, "No symbol. Repeat.");
		mvwgetstr(WIN, 4, 3, new_name);
		box(WIN, 0, 0);
		mvwprintw(WIN,0,5,"ADD RECORD");
		wrefresh(WIN);
	}

	OK_record = mvwscanw(WIN, 8, 3, "%d", &new_price);
	box(WIN, 0, 0);
	mvwprintw(WIN,0,5,"ADD RECORD");
	wrefresh(WIN);
	while(OK_record != 1) {
		mvwprintw(WIN, 8, 3, "                ");
		mvwprintw(WIN, 9, 1, "                   ");
		logs_out(LOG, "Format error. Repeat.");
		OK_record = mvwscanw(WIN, 8, 3, "%d", &new_price);
		box(WIN, 0, 0);
		mvwprintw(WIN,0,5,"ADD RECORD");
		wrefresh(WIN);
	}

	PAPER *tmp = (PAPER*)malloc(sizeof(PAPER));
	strcpy(tmp->name, new_name);
	tmp->price = new_price;
	if (number_of_records == 0){
		(*head) = tmp;
		logs_out(LOG, "New head, epta!");
	}

	((*head)->prev)->next = tmp;
	tmp->prev = (*head)->prev;
	(*head)->prev = tmp;
	tmp->next = (*head);
	logs_out(LOG, "New record created.");

	wrefresh(WIN);
	noecho();
	wbkgdset(WIN, COLOR_PAIR(2));
	wclear(WIN);
	wrefresh(WIN);
	wbkgdset(WIN, COLOR_PAIR(1));
}

void delete_record(PAPER **head, WINDOW *LOGS, int active_string){
	PAPER* ptr = (*head);
	for (int i = 0; i < active_string - 1; i++)
		ptr = ptr->next;
	if ((*head)->next != *head){
	(ptr->prev)->next = ptr->next;
	(ptr->next)->prev = ptr->prev;
	if (ptr == (*head))
		(*head) = ptr->next;
	}
	free(ptr);
	logs_out(LOGS, "Record deleted.");
}

void saveChanges(FILE **f, PAPER *head){
	fclose(*f);
	FILE *tmp = fopen ("source.txt", "w");
	PAPER *ptr = head;
	do 
	{
		fprintf(tmp, "%s\n", ptr->name);
		fprintf(tmp, "%d\n", ptr->price);
		ptr = ptr->next;
	}
	while (ptr != head);
	fclose(tmp);
	(*f) = fopen("source.txt", "r");

}