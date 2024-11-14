#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//Static Data from /proc/cpuinfo
int count_occur(const char *line, const char *search) {
    int count = 0;
    const char *tmp = line;

    // Ищем все вхождения подстроки
    while ((tmp = strstr(tmp, search)) != NULL) {
        count++;
        tmp++; // Сдвигаем указатель дальше
    }
    return count; // Возвращаем количество вхождений
}
int num_core(int file_descriptor1){
	int c = 0;
	char info[256];
        const char *search = "core id"; 
        ssize_t num;
        while ((num = read(file_descriptor1, info, sizeof(info) - 1)) > 0) {
                        info[num] = '\0';
                        c += count_occur(info, search); // Подсчитываем вхождения
         }
	return c;
}
int find_num_value(int file_descriptor, char st1[], char st2[]){
        char buffer[256];
        int n = read(file_descriptor, buffer, sizeof(buffer));
	if (n < 0){
                return 0;
        }
        buffer[n] = '\0';
        char *line = buffer;
        int fr = 0;
        while ((line = strstr(line, st1)) != NULL ) {
                if (sscanf(line, st2, &fr) == 1){
                        break;}
                line++;
        }
        return fr;
}



double find_num_valuef(int file_descriptor, char st1[]){
        off_t file_size = lseek(file_descriptor, 0, SEEK_END); // размер файла
        lseek(file_descriptor, 0, SEEK_SET); // перевести в начало

        char *buffer = (char*)malloc(1500); 
        ssize_t bytes_read = read(file_descriptor, buffer, file_size);

        if (bytes_read != file_size) { // если файл прочитан не полностью или не прочитан - очистить буффер и ничего не выводить
                free(buffer);
                close(file_descriptor);
                return 0;
        }
        buffer[file_size] = '\0'; // строки в C  заканчиваются на \0
        char *found = strstr(buffer, st1); // указатель на строку, где лежит нужная
        float fr;
        char *clean_buffer = found;

        char *pass = (char*)malloc(100);

        sscanf(clean_buffer, "%s %f", pass, &fr);
        return fr;
}


char* find_proc(int file_descriptor, char fr[50]){
	char buffer[256];
        int n = read(file_descriptor, buffer, sizeof(buffer));
	if (n <= 0){
		return "-";
	}
        buffer[n] = '\0';
        char *line = buffer;
	while ((line = strstr(line, "model name")) != NULL) {
		char* beg = strchr(line, ':');
		if (beg != NULL){
			beg++;
			char* end = strchr(beg, '\n');
			if (end!= NULL){
			size_t length = end - beg > 49 ? 49: end - beg;
			strncpy(fr, beg, length);
			fr[length] = '\0';
		}else{
			strncpy(fr, beg, 49);
			fr[49] = '\0';
		}
		break;
		}
        	line++;
    	}
        return fr;
}
//    technology:          lithium-ion
//    warning-level:       none
int battery_health(int curs_x){
	int file_descriptor_bat = open("./info_battery.txt", O_RDONLY);
	if (file_descriptor_bat == 0) {
		//start_color();
		//init_pair();             Потом разберусь как менять цвет надписей, такуюже добавить в местах исключений для чтения файла
		printw("The required data cannot be obtained");
		move(curs_x, 0);
                curs_x++;
		printw("Designed battery capacity: -\n");
                move(curs_x, 0);
                curs_x++;
                printw("Current battery capacity: -\n");
                move(curs_x, 0);
                curs_x++;
                printw("Capacity: -\n");
                move(curs_x, 0);
                curs_x++;
                printw("Voltage: -\n");
                move(curs_x, 0);
                curs_x++;
	}
	else{
	system("upower -i /org/freedesktop/UPower/devices/battery_BAT0 > info_battery.txt");
	double ef, efd, vol, cap;
	char sb3[] = "energy-full";
        ef = find_num_valuef(file_descriptor_bat, sb3);

	char sb1[] = "energy-full-design";
        efd = find_num_valuef(file_descriptor_bat, sb1);

	char sb7[] = "voltage";
        vol = find_num_valuef(file_descriptor_bat, sb7);

	char sb5[] = "capacity";
        cap = find_num_valuef(file_descriptor_bat, sb5);

	move(curs_x, 0);
        curs_x++;
	printw("Designed battery capacity: %.3f Wh\n", efd);
        move(curs_x, 0);
        curs_x++;
	printw("Current battery capacity: %.3f Wh\n", ef);
        move(curs_x, 0);
        curs_x++;
        printw("Capacity: %.2f %%\n", cap);
        move(curs_x, 0);
        curs_x++;
        printw("Voltage: %.3f V\n", vol);
        move(curs_x+1, 0);
        curs_x+=2;
        }
	close(file_descriptor_bat);
	return curs_x;
}

//----------Monitoring Data----------------
void find_uptime(int curs_x){
	int file_descriptor2, n;
        while (1){
        move(curs_x, 0);
        if ((file_descriptor2 = open("/proc/uptime", O_RDONLY)) == 0){
                printw("Machine uptime: -");
                return;
        }
        char buffer[50];
        if ((n = read(file_descriptor2, buffer, 50)) != 0){
                printw("Machine uptime: %s", buffer);
        }
        close(file_descriptor2);
        refresh();
        usleep(680000);
        }
	return;
}
//можно включить мониторинг подключения к сети через ping	
int main(){
	int curs_x = 0;
	initscr();
        move(curs_x, 0);
        curs_x ++;
        printw("-----Static Machine Data------");
        move(curs_x+1, 0);
        curs_x+=2;
        int file_descriptor1;
	if ((file_descriptor1 = open("/proc/cpuinfo", O_RDONLY)) ==  0){
        	printw("Processor model name: -\n");
		move(curs_x, 0);
        	curs_x++;
		printw("Сlock frequency: -\n");
                move(curs_x, 0);
                curs_x++;
		printw("Сache size: -\n");
                move(curs_x, 0);
                curs_x++;
		printw("Number of cores: -\n");
                move(curs_x+1, 0);
                curs_x+=2;
	}
	else{
		char fr[50];
                char *proc_name = find_proc(file_descriptor1, fr);
                printw("Processor model name: %s\n", proc_name);
                move(curs_x, 0);
                curs_x++;
		deleteln();
        	lseek(file_descriptor1, 0, SEEK_SET);//сдвинули указатель в начало файла
		char sf1[] = "cpu MHz", sf2[] = "cpu MHz               : %d";
		int freq = find_num_value(file_descriptor1,sf1,sf2);
        	printw("Сlock frequency: %d MHz", freq);
        	move(curs_x, 0);
        	curs_x++;
        	lseek(file_descriptor1, 0, SEEK_SET);
		char sc1[] = "cache size", sc2[] = "cache size    : %d KB";
        	int cache = find_num_value(file_descriptor1,sc1,sc2);
        	printw("Cache size: %d KB", cache);
        	move(curs_x, 0);
        	curs_x++;
		lseek(file_descriptor1, 0, SEEK_SET);
		int count_core = num_core(file_descriptor1);
                printw("Number of cores: %d\n", count_core);
                move(curs_x + 1, 0);
                curs_x+= 2;
		close(file_descriptor1);
	}
	printw("-----Battery Status-----");
	move(curs_x+1, 0);
        curs_x+= 1;
	curs_x = battery_health(curs_x);
        printw("-----Monitoring Data-----");
	move(curs_x+1, 0);
        curs_x+= 1;
	find_uptime(curs_x);
	clear();
	endwin();
	return 0;
        //очистить файловые дескрипторы
}
