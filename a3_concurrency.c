#include "a3.h"

void display_sysinfo(){         // this function displays the information about the system has no arguments and no return type i.e.void

    struct utsname info;        // the structure is in <sys/utsname.h>
        if(uname(&info) < 0){   // check whether uname return the information
            fprintf(stderr, "%s", "Error in display_sysinfo() when accessing utsname struct.\n");
            exit(1);
        }

        printf("---------------------------------\n");
        printf("### System Information ###\n");        
        printf("System name = %s\n", info.sysname);     // system name
        printf("Machine name = %s\n", info.nodename);   // machine name
        printf("Version = %s\n", info.version);         // version
        printf("Release = %s\n", info.release);         // release
        printf("Architecture = %s\n",info.machine);     // architecture
        printf("---------------------------------\n");
}

void display_user(int pipe_users){  
    char users[1024];
    snprintf(users,sizeof(users),"---------------------------------\n");
	write(pipe_users,users,strlen(users));
	snprintf(users,sizeof(users)," ### Sessions/users ### \n");
	write(pipe_users,users,strlen(users));
        struct utmp *user;                         // the structure is in <utmp.h>
        setutent();                                // the function sets the file pointer to the beginning of the utmp file                               
        while((user = getutent())){                // the function reads a line from the current position of the utmp file 
            if(user->ut_type == USER_PROCESS){     // we check if the pointed user is normal process
                snprintf(users, sizeof(users)," %s              %s (%s)\n", user->ut_user, user->ut_line, user->ut_host);
                write(pipe_users, users, strlen(users));
            }
        }
        endutent();                                 // closes the utmp file
        snprintf(users,sizeof(users),"---------------------------------\n");
        write(pipe_users, users, strlen(users));
}

void display_info(int delay, int samples){          // this function displays the samples/delay and memory usage has two arguments : int delay and samples
                                                    // the function has no return type i.e. void
    printf("Nbr of samples: %d -- every %d secs\n", samples, delay);

        struct rusage usage;                        // this structure is in <sys/resource.h>
        if(getrusage(RUSAGE_SELF, &usage) < 0){     // getrusage gets information about resource utilization. 
            fprintf(stderr, "%s", "Error in display_info() - Error in using getrusage()\n");
            exit(1);                                // If the value of the first argument is RUSAGE_SELF,return the info about resources used by the current process.
        }                                           // second argument of the function is struct rusage *r_usage
        printf("Memory usage: %ld kilobytes\n", usage.ru_maxrss);  // ru_maxrss is a field in the stuct which gives maximum resident set size
        printf("---------------------------------\n");
}


void display_cpu(int pipe_cpu, double cpu[][MAX_STR_LEN], int iter, int graphics, int delay){   // this function returns float and has one argument: int delay
    char cpu_arr[1024];
    float user1, nice1, system1, idle1, iowait1, irq1, softirq1, total1;
    FILE *f = fopen("/proc/stat", "r");     // reads the /proc/stat file
    if(f == NULL){
        fprintf(stderr, "%s", "The file could not be opened. Exiting.\n"); 
        exit(1);
    }
    fscanf(f,"cpu %f %f %f %f %f %f %f",&user1, &nice1, &system1, &idle1, &iowait1, &irq1, &softirq1); // stores the values from file to the variables.
    total1 = (user1 + nice1 + system1 +idle1 + iowait1 + irq1 + softirq1);
    float total2 = total1 - idle1;
    fclose(f);

    sleep(delay);

    float user2, nice2, system2, idle2, iowait2, irq2, softirq2, total3;
    FILE *f2 = fopen("/proc/stat", "r");     // reads the /proc/stat file
    if(f2 == NULL){
        fprintf(stderr, "%s", "The file could not be opened. Exiting.\n"); 
        exit(1);
    }
    fscanf(f2,"cpu %f %f %f %f %f %f %f",&user2, &nice2, &system2, &idle2, &iowait2, &irq2, &softirq2); // stores the values from file to the variables.
    total3 = (user2 + nice2 + system2 +idle2 + iowait2 + irq2 + softirq2);
    float total4 = total3 - idle2;
    fclose(f2);

    float cpu_usage = fabs((double)((total4 - total2)/(total3-total1))*100);
    cpu[iter][0] = cpu_usage;
    
    if(graphics == 1){
        snprintf(cpu_arr, sizeof(cpu_arr),"total cpu use = %f%%\n",cpu[iter][0]);
        write(pipe_cpu, cpu_arr,strlen(cpu_arr));
	    snprintf(cpu_arr, sizeof(cpu_arr),"\033[%dB",iter + 1);
        write(pipe_cpu, cpu_arr,strlen(cpu_arr));
        float change = round((cpu_usage - cpu[iter-1][0])* 100.0)/ 100.0;
        if (change > 0.00){
            for (int i =0; i < change; i++){
                snprintf(cpu_arr,sizeof(cpu_arr),"|");
                write(pipe_cpu, cpu_arr,strlen(cpu_arr));
            }
            snprintf(cpu_arr,sizeof(cpu_arr)," %f%%\n", cpu_usage);
            write(pipe_cpu, cpu_arr,strlen(cpu_arr));
        }
        else{
            for(int i = 0; i > change; i--)
            {
			    snprintf(cpu_arr,sizeof(cpu_arr),"|");
                write(pipe_cpu, cpu_arr,strlen(cpu_arr));
            }
            snprintf(cpu_arr,sizeof(cpu_arr)," %f%%\n", cpu_usage);
            write(pipe_cpu, cpu_arr,strlen(cpu_arr));
        }
    }
    else{
        snprintf(cpu_arr,sizeof(cpu_arr),"total cpu use  %.2f%%\n",cpu[iter][0]);
	    write(pipe_cpu, cpu_arr,strlen(cpu_arr));
    }
}

void display_cores(int samples){       // this function displays number of cores has no arguments and does not return anything
        FILE *f;
        f = fopen("/proc/stat", "r"); // reading the file
        if(f == NULL){
            fprintf(stderr, "%s", "The file could not be opened. Exiting.\n"); 
            exit(1);
        }
        char cpu[20];
        int wc = 0;
        int count = 0;
        while(fscanf(f, "%s", cpu) != EOF){     // count the all lines where starts with cpu form the file
            if(strncmp(cpu, "cpu", 3) == 0){
                count++;
            }
            wc++;
        }
        fclose(f);
        printf("Number of cores: %d\n", count-1); // -1 because we ignore the first line wheere it starts with cpu
}

void get_prev(float nums[][MAX_STR_LEN], int iter){
     struct sysinfo memory;         // this stucture is in <sys/sysinfo.h>
    if(sysinfo(&memory) < 0){      // sysinfo func returns certain statistics on memory and swap usage
        fprintf(stderr, "%s", "Error in get_prev() when accessing sysinfo struct.\n");
        exit(1);
    }
    float used_v = (unsigned long)((memory.totalram - memory.freeram) + (memory.totalswap- memory.freeswap)) / pow(1024,3);
    nums[iter][0] = used_v;
}

void display_memory(int pipe_memory,char memory_info[][MAX_STR_LEN], float nums[][MAX_STR_LEN], int iter, int graphics, int sequential, int samples, float prev){
    char memory_arr[1024];
    struct sysinfo memory;         // this stucture is in <sys/sysinfo.h>
    if(sysinfo(&memory) < 0){      // sysinfo func returns certain statistics on memory and swap usage
        fprintf(stderr, "%s", "Error in display_memory() when accessing sysinfo struct.\n");
        exit(1);
    }
    int i = iter;
    if(i == 0 && sequential == 0){
        snprintf(memory_arr, sizeof(memory_arr),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        write(pipe_memory,memory_arr,strlen(memory_arr));
    }
    i++;
    float total_p = (unsigned long)(memory.totalram) / pow(1024,3);     // total physical memory in GB
    float used_p = (unsigned long)(memory.totalram - memory.freeram) / pow(1024,3); // used physical memory in GB 
    float total_v = (unsigned long)(memory.totalswap + memory.totalram) / pow(1024,3); // total virtual memory in GB
    float used_v = (unsigned long)((memory.totalram - memory.freeram) + (memory.totalswap- memory.freeswap)) / pow(1024,3); // used virtual memory in GB
    sprintf(memory_info[iter]," %.2f GB /%.2f GB  --   %.2f GB /%.2f GB\n", used_p, total_p, used_v, total_v);
    
    if(sequential == 1 && graphics == 0){
        for(int j =0; j<i; j++){
            if(j == 0){
                snprintf(memory_arr, sizeof(memory_arr),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
            if(j == iter){
                snprintf(memory_arr, sizeof(memory_arr),"%s",memory_info[iter]);
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
            else{
                snprintf(memory_arr, sizeof(memory_arr),"\n");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
        }
        while(i<samples){
            snprintf(memory_arr, sizeof(memory_arr),"\n");
            write(pipe_memory,memory_arr,strlen(memory_arr));
            i++;
        }
    }

    else if(graphics == 1 && sequential == 0){
        if(iter == 0){
            snprintf(memory_arr, sizeof(memory_arr),"\033[H");   // escape code to put cursor top left
            write(pipe_memory,memory_arr,strlen(memory_arr));
            snprintf(memory_arr, sizeof(memory_arr),"\033[%dB", iter+4);
            write(pipe_memory,memory_arr,strlen(memory_arr));
            snprintf(memory_arr, sizeof(memory_arr), "%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |o  %.2f (%.2f)\n", used_p, total_p, used_v, total_v, 0.0, used_v);
            write(pipe_memory,memory_arr,strlen(memory_arr));
        }
        else{
            float change = round((used_v - prev) *100.0) / 100.0;
            snprintf(memory_arr, sizeof(memory_arr),"\033[H");   // escape code to put cursor top left
            write(pipe_memory,memory_arr,strlen(memory_arr));
            snprintf(memory_arr, sizeof(memory_arr),"\033[%dB", iter+4);
            write(pipe_memory,memory_arr,strlen(memory_arr));

            snprintf(memory_arr, sizeof(memory_arr),"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |", used_p, total_p, used_v, total_v);
            write(pipe_memory,memory_arr,strlen(memory_arr));
            if (change > 0.00) {
                for (int i = 0; i < change * 100; i++) {
                    snprintf(memory_arr, sizeof(memory_arr),"#");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                    
                }
                snprintf(memory_arr, sizeof(memory_arr),"*");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            } 
            else if (change < 0.00) {
                for (int i = 0; i > change * 100; i--) {
                    snprintf(memory_arr, sizeof(memory_arr),":");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                }
                snprintf(memory_arr, sizeof(memory_arr),"@");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            } 
            else if (change == 0.00){
                if (signbit(change)) {
                    snprintf(memory_arr, sizeof(memory_arr),"|@");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                } else {
                    snprintf(memory_arr, sizeof(memory_arr),"|o");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                }
            }
        snprintf(memory_arr, sizeof(memory_arr)," %.2f (%.2f)\n", change, used_v);
        write(pipe_memory,memory_arr,strlen(memory_arr));
    }
} 

    else if(graphics == 1 && sequential == 1){
        for(int j =0; j<i; j++){
            if(j == 0){
                snprintf(memory_arr, sizeof(memory_arr),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
            if(iter == 0){
                snprintf(memory_arr, sizeof(memory_arr),"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |o  %.2f (%.2f)\n", used_p, total_p, used_v, total_v, 0.0, used_v);
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
            else if(j == iter){
                float change = round((used_v - prev)*100.0) / 100.0;
                snprintf(memory_arr, sizeof(memory_arr),"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |", used_p, total_p, used_v, total_v);
                write(pipe_memory,memory_arr,strlen(memory_arr));
            if (change > 0.00) {
                for (int i = 0; i < change * 100; i++) {
                    snprintf(memory_arr, sizeof(memory_arr),"#");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                }
                snprintf(memory_arr, sizeof(memory_arr),"*");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            } 
            if (change < 0.00) {
                for (int i = 0; i > change * 100; i--) {
                    snprintf(memory_arr, sizeof(memory_arr),":");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                }
                snprintf(memory_arr, sizeof(memory_arr),"@");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            } 
            if (change == 0.00){
                if (signbit(change)) {
                    snprintf(memory_arr, sizeof(memory_arr),"|@");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                } else {
                    snprintf(memory_arr, sizeof(memory_arr),"|o");
                    write(pipe_memory,memory_arr,strlen(memory_arr));
                }
            }
            snprintf(memory_arr, sizeof(memory_arr)," %.2f (%.2f)\n", change, used_v);
            write(pipe_memory,memory_arr,strlen(memory_arr));
            }
            else{
                snprintf(memory_arr, sizeof(memory_arr),"\n");
                write(pipe_memory,memory_arr,strlen(memory_arr));
            }
        }
        while(i<samples){
            snprintf(memory_arr, sizeof(memory_arr),"\n");
            write(pipe_memory,memory_arr,strlen(memory_arr));
            i++;
        }
    }
    else{
        snprintf(memory_arr, sizeof(memory_arr),"\033[H");
	    write(pipe_memory,memory_arr,strlen(memory_arr));
	    snprintf(memory_arr, sizeof(memory_arr),"\033[%dB",iter+4);
	    write(pipe_memory,memory_arr,strlen(memory_arr));
	    snprintf(memory_arr,sizeof(memory_arr)," %s",memory_info[iter]);
	    write(pipe_memory,memory_arr,strlen(memory_arr));
    }  
}