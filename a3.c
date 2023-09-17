#include "a3.h"

pid_t display_user_pipe(int pipe_users[2]){
    pipe(pipe_users);
    if(pipe_users < 0){
        fprintf(stderr, "%s", "Pipe is not working.\n");   
        exit(1);
    }
   
    pid_t pid_users = fork();
    if(pid_users < 0){
        fprintf(stderr, "%s", "Fork is not working.\n");
        exit(1);
    }
    if(pid_users == 0){
        signal(SIGINT, SIG_DFL);
        close(pipe_users[0]); 
        display_user(pipe_users[1]); 
        close(pipe_users[1]); 
        exit(0);
    } else { 
        close(pipe_users[1]);
    }
    return pid_users;
}

void display_user_write(int pipe_users[2], char users[1024]){
    ssize_t bytes;
    while ((bytes = read(pipe_users[0], users, MAX_STR_LEN - 1)) > 0) {
        users[bytes] = '\0';
        printf("%s", users);
    }
    if (read(pipe_users[0], users, MAX_STR_LEN) == -1)
    {
        fprintf(stderr, "%s", "Pipe read is not working.\n");  
        exit(1);
    }
    close(pipe_users[0]);
}

pid_t display_memory_pipe(int pipe_memory[2], char memory_info[][MAX_STR_LEN], float nums[][MAX_STR_LEN], int iter, int graphics, int sequential, int samples, float prev){
    pipe(pipe_memory);
    if(pipe_memory < 0){
        fprintf(stderr, "%s", "Pipe is not working.\n");   
        exit(1);
    }
   
    pid_t pid_memory = fork();
    if(pid_memory < 0){
        fprintf(stderr, "%s", "Fork is not working.\n");
        exit(1);
    }
    if(pid_memory == 0){
        signal(SIGINT, SIG_DFL);
        close(pipe_memory[0]); 
        display_memory(pipe_memory[1],memory_info, nums, iter, graphics, sequential,samples, prev); 
        close(pipe_memory[1]); 
        exit(0);
    } else { 
        close(pipe_memory[1]);
    } 
    return pid_memory;
}

void display_memory_write(int pipe_memory[2], char memory[1024]){
    ssize_t bytes;
    while ((bytes = read(pipe_memory[0], memory, MAX_STR_LEN - 1)) > 0) {
        memory[bytes] = '\0';
        printf("%s", memory);
    }
    if (read(pipe_memory[0], memory, MAX_STR_LEN) == -1)
    {
        fprintf(stderr, "%s", "Pipe read is not working.\n");  
        exit(1);
    }
    close(pipe_memory[0]);
}

pid_t display_cpu_pipe(int pipe_cpu[2], double cpu[][MAX_STR_LEN], int iter, int graphics, int delay){
    pipe(pipe_cpu);
    if(pipe_cpu < 0){
        fprintf(stderr, "%s", "Pipe is not working.\n");   
        exit(1);
    }
    pid_t pid_cpu = fork();
    if(pid_cpu < 0){
        fprintf(stderr, "%s", "Fork is not working.\n");
        exit(1);
    }
    if(pid_cpu == 0){
        signal(SIGINT, SIG_DFL);
        close(pipe_cpu[0]); 
        display_cpu(pipe_cpu[1], cpu, iter, graphics, delay);
        close(pipe_cpu[1]); 
        exit(0);
    } else { 
        close(pipe_cpu[1]);
    } 
    return pid_cpu;
}

void display_cpu_write(int pipe_cpu[2], char cpu[1024]){
    ssize_t bytes;
    while ((bytes = read(pipe_cpu[0], cpu, MAX_STR_LEN - 1)) > 0) {
        cpu[bytes] = '\0';
        printf("%s", cpu);
    }
    if (read(pipe_cpu[0], cpu, MAX_STR_LEN) == -1)
    {
        fprintf(stderr, "%s", "Pipe read is not working.\n");  
        exit(1);
    }
    close(pipe_cpu[0]);
}

void handle_sigint(int sig) {
    char quit_program;
    printf("sure you want to quit? (y/n)");
    scanf("%c", &quit_program);
    if (quit_program == 'y' || quit_program == 'Y') {
            exit(0);
    } else {
        return;
        }
}


void handle_sigtstp(int sig){
	return;
}

int main(int argc, char **argv){
    int delay = DELAY_DEF;
    int samples = SAMPLE_DEF;
    int system = 0;
    int user = 0;
    int sequential = 0;
    int graphics = 0;

    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    
    printf("\033[2J\033[H"); //esape code to clear screen and put cusor to top left

    if(argc == 0){
        system = 1;
        user = 1;
    }
    for(int i=0; i<argc; i++){
            if(strcmp(argv[i], "--system") == 0){
                system = 1;
            }

            else if(strcmp(argv[i], "--user") == 0){
                user = 1;
            }
            else if(strcmp(argv[i], "--sequential") == 0){
                sequential = 1;
            }
            else if(sscanf(argv[i], "--samples=%d", &samples) == 1) { 
            }
            else if(sscanf(argv[i], "--tdelay=%d", &delay) == 1) {
            }
            else if(strcmp(argv[i], "--graphics") == 0){
                graphics = 1;
            }
            else if(atoi(argv[i]) != 0){
                samples = atoi(argv[1]);
                delay = atoi(argv[2]);
            }

    }

    char memory_info[samples][MAX_STR_LEN];
    float nums[samples][MAX_STR_LEN];
    double cpu[samples][MAX_STR_LEN];
   

    if(sequential == 0)
        display_info(delay, samples);
    for(int i=0; i<samples; i++){
        int pipe_memory[2];
        int pipe_users[2];
        int pipe_cpu[2];

        pid_t pid_memory; 
        pid_t pid_user; 
        pid_t pid_cpu;

        char memory[1024];
        char users[1024];
        char cpu2[1024];

        int status;
        if(system == 1 && user == 0 && sequential == 0){
            get_prev(nums, i);
            pid_memory = display_memory_pipe(pipe_memory, memory_info, nums, i, graphics, sequential,samples, nums[i-1][0]);
            display_memory_write(pipe_memory, memory);
            waitpid(pid_memory,&status, 0);
            printf("\033[H");
            printf("\033[%dB", samples+4);
            display_cores(samples);
            pid_cpu = display_cpu_pipe(pipe_cpu, cpu, i, graphics, delay);
            display_cpu_write(pipe_cpu, cpu2);
            waitpid(pid_cpu,&status, 0);
        }
        else if(system == 0 && user == 1){
            pid_user = display_user_pipe(pipe_users);
             waitpid(pid_user,&status,0);
            if(sequential == 0){
                 printf("\033[H");
                printf("\033[%dB",3);
            }
            display_user_write(pipe_users, users);
            sleep(delay);
        }
        else if(sequential == 1){
            display_info(delay, samples);
            get_prev(nums, i);
            pid_memory = display_memory_pipe(pipe_memory, memory_info, nums, i, graphics, sequential,samples, nums[i-1][0]);
            display_memory_write(pipe_memory, memory);
            waitpid(pid_memory,&status, 0);
            if(system == 0 || user == 1){
                pid_user = display_user_pipe(pipe_users);
                display_user_write(pipe_users, users);
                waitpid(pid_user,&status, 0);
            }
            pid_cpu = display_cpu_pipe(pipe_cpu, cpu, i, graphics, delay);
            display_cpu_write(pipe_cpu, cpu2);
            waitpid(pid_cpu,&status, 0);
            display_sysinfo();
        }
        else{
            get_prev(nums, i);
            pid_memory = display_memory_pipe(pipe_memory, memory_info, nums, i, graphics, sequential,samples, nums[i-1][0]);
            display_memory_write(pipe_memory, memory);
            pid_user = display_user_pipe(pipe_users);
            waitpid(pid_memory,&status, 0);
            printf("\033[H");
            printf("\033[%dB", samples+4);
            display_user_write(pipe_users, users);
            waitpid(pid_user,&status, 0);
            display_cores(samples);
            pid_cpu = display_cpu_pipe(pipe_cpu, cpu, i, graphics, delay);
            display_cpu_write(pipe_cpu, cpu2);
            waitpid(pid_cpu,&status, 0);
        }
    }
    if(sequential == 0)
        display_sysinfo();
    
    return 0;
}