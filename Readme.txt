B09 Assignment 3:
	
Assignment Functions:

a3_concurrency.c:

	- void display_sysinfo()

		This function does not return anything and has no arguments. The function uses struct utsname. The structure is in <sys/utsname.h>.
		The man page: https://man7.org/linux/man-pages/man2/uname.2.html
		To get the system information I used uname() function returns system information in the structure pointed to. On success it returns 0.
		Therefore, I checked it with an if statement. 
		By using struct I displayed all the information that was required.

	- int display_user()

		This function returns an integer and has no arguments. The function uses struct utmp. The structure is in <utmp.h>.
		The man page: https://man7.org/linux/man-pages/man5/utmp.5.html
		To get the users I used 
			setutent(): The function sets the file pointer to the beginning of the utmp file
			getutent(): The function reads a line from the current position of the utmp file 
			endutent(): The function closes the utmp file
		In while loop I update the struct pointer with getutent function. Then for each line I checked if the pointed user is normal process
		which is USER_PROCESS and I updated the int num variable accordingly. Then I returned the num variable.

	- void display_info(int delay, int samples)

		This function does not return anything and has two arguments: int delay and int samples. The function uses struct rusage. The structure is in <sys/resource.h>.
		The man page: https://man7.org/linux/man-pages/man0/sys_resource.h.0p.html
		The number of samples and the delay is printed according to function arguments.
		In order to print memory usage I used getrusage function that gets information about resource utilization. If the value of the first argument is
		RUSAGE_SELF, returns the info about resources used by the current process and second argument of the function is struct rusage *r_usage. So I put the 						    
		struct pointer. To display the memory usage I use ru_maxrss field in the struct which gives maximum resident set size.

	- void display_cpu(int pipe_cpu, double cpu[][MAX_STR_LEN], int iter, int graphics, int delay)
	
		This functions calculates the cpu usage according to the information that was given on A3 instructions.
		total cpu utilization time: Ti = useri + nicei + sysi + idlei + IOwaiti + irqi + softirqi 
        	idle time: Ii = idlei where i represents a sampling time.
	

	- void display_cores()

		This function does not return anything and has no arguments. The function reads /proc/stat file and while it's not the end of file I counted the number of lines 
		that starts with "cpu". At the end I did counter -1 because the first line is the total information.

	- void get_prev(float nums[][MAX_STR_LEN], int iter)
	
		This function helps us the store the previous iteration value of used virtual so that we can pipe it and use it.

	- void display_memory(int delay, int samples, int flag, int iter , int user)

		This function does not return anything but has four arguments: int delay, samples, flag, iter, user. The function uses struct sysinfo. The structure is in <sys/
		sysinfo.h>. 
		The man page: https://man7.org/linux/man-pages/man2/sysinfo.2.html
		To get memory information I used sysinfo() returns certain statistics on memory and swap usage.
		The total physical memory: totalram
		Used physical memory: totalram - freeram
		Total virtual memory: total swap + freeswap
		Used virtual memory : (totalram - freeram) + (total swap - freeswap)
		To convert byte to GB I divided the values by (1024 * 1024 * 1024)
	
		In order to display cpu usage I got the first calculation of cpu and made the function to sleep for given delay seconds and got the second calculation of cpu.
		If the command line is system or no argument, I put the cursor to the top left and then for each for loop iteration, I calculated where the cursor should be and
		print the memory information. To print cpu percentage I put the cursor at top left again and calculate the line according to command line.
		If the command lines related the sequential then the escape code calculation is based on number of users samples and the iteration. 

		In order to calculate cpu usage I subtract the two values I got between delay seconds and divided by the first one and multiplied with 100. 
		
		This function also takes care of the graphics part and graphics and sequential part.


a3.c:
	- pid_t display_user_pipe(int pipe_users[2])
		
		This function pipes and for the information for the users

	- void display_user_write(int pipe_users[2], char users[1024])

		This function reads the information from pipe and write it to the screen

	-pid_t display_memory_pipe(int pipe_memory[2], char memory_info[][MAX_STR_LEN], float nums[][MAX_STR_LEN], int iter, int graphics, int sequential, int samples, float prev)

		This function pipes and for the information for the memory part
	
	- void display_memory_write(int pipe_memory[2], char memory[1024])

		This function reads the information from pipe and write it to the screen

	- pid_t display_cpu_pipe(int pipe_cpu[2], double cpu[][MAX_STR_LEN], int iter, int graphics, int delay)

		This function pipes and for the information for the cpu

	- void display_cpu_write(int pipe_cpu[2], char cpu[1024])

		This function reads the information from pipe and write it to the screen
		
	- void handle_sigint(int sig)
	
		This function deals with Ctrl+C

	- void handle_sigtstp(int sig)

		This function deals with Ctrl+ Z


Notes about my output:

	Please do not run the program with one positional argument, in order to work both positional arguments should be given as in A1.

	You can use the following:

	--system
	--user
	--sequential
	--tdelay=N
	--samples=N

	it works with positional arguments (both together not one)

	if you want to use positional arguments with other flags please put them at first, not in the middle or end.
	
	I implemented the way that if the user puts --user flag the it only does user except system.
	system can use user however for sequential since it's unnecessary, the combination of user, sequential, prints only users sequentially please if you want to do
	sequential graphics then don't put users. (Because of my cases sequential and user combination does sequential users)

	but once sequential, graphics, user and system is put we can see the normal output is printed.

	rest of the command line arguments start with -- works together (user edge case I mentioned it above)
	The executable is ./program

Important note about makefile:

Since I did this in a2 submission and I don't know which file extension way worked I did the same thing for A3. I have makefile as saved as makefile and also plaintext file to have the backup. 
They both same but I usually get confused which way of saving them is better from vs code.



		