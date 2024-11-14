#include <sys/utsname.h>
#include <stdio.h>
int main(){
	struct utsname system_info;
	 if (uname(&system_info) != 0) { 
  		perror("Ошибка uname()");
  			return 1;  
 		}
	printf("Name of operation system: %s\n", system_info.sysname);
	printf("Architecture: %s\n", system_info.machine);
}
