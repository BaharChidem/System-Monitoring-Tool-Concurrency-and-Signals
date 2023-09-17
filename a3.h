#include<stdio.h>
#include<stdlib.h>
#include<sys/resource.h>
#include<sys/utsname.h> 
#include<sys/sysinfo.h>
#include<utmp.h>
#include<math.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include <sys/wait.h>
#include <signal.h>

#define SAMPLE_DEF 10
#define DELAY_DEF 1
#define MAX_STR_LEN 1024

#ifndef __A3_header
#define __A3_header

/*displays the system information*/
void display_sysinfo();

/*displays the current users*/
void display_user(int pipe_users);

/*displays the mmory usage and number of samples with how many seconds*/
void display_info(int delay, int samples);

/*displays cpu percantage*/
void display_cpu(int pipe_cpu,double cpu[][MAX_STR_LEN], int iter, int graphics, int delay);

/*displays number of cores*/
void display_cores(int samples);

/*displays memory and graphics*/
void display_memory(int pipe_memory,char memory_info[][MAX_STR_LEN], float nums[][MAX_STR_LEN], int iter, int graphics, int sequential, int samples, float prev);

/*gets the previous itetarion of used virtual memory*/
void get_prev(float nums[][MAX_STR_LEN], int iter);

#endif