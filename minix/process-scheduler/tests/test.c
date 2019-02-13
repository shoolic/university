#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lib.h>

int main(int argc, char ** argv)
{
	int pid;
	int new_pri;
	int check_pri;
   	int i;
   	message m;

	if(argc != 2)
	{
		printf("wrong number of arguments, expected <program_name> <process_priority>\n");
		return 1;
	}
	
        new_pri = atoi(argv[1]);
	pid = getpid();

	m.m1_i1 = pid;
	m.m1_i2 = new_pri;
	_syscall(MM, SETPRI, &m);

	m.m1_i1 = pid;
	check_pri = _syscall(MM, GETPRI, &m);
    
	if(errno != 0) {
        	printf("error with code %d occured\n", errno);
        	return 2;
   	}

	printf("Process with pid %d changed priority to %d\n", pid, check_pri);
       
/*	for(i = 0; i < 2147483647; i++); */
	printf("Process with pid %d and pri %d finished\n", pid, check_pri);
	return 0;
}
