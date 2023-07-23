#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/procfs.h>
#include <sys/time.h>
#include <proc/readproc.h>
#include <inttypes.h>

#define BUFFER_SIZE 50


/* Functions Prototypes. */
int InterfaceAndOption();
bool touch(const char *filename);
void AllRunningProcess();
void RunningProcessOFPID(pid_t pid);
void pwd();


/* -------------------------------------- DRIVER FUNCTION START ------------------------------------ */
int main()
{
	int op, retval;
	
	/* Loop For Run this program infinitely until Programmer Terminate it.  */
	for(int k=0; ; k++)
	{
	
		op = InterfaceAndOption();
	
		// Run All Processes
		if(op == 1)
		{	
			AllRunningProcess();
		}
		// Create a Child Process 
		else if(op == 2)
		{
	    		int val = 10;
    			pid_t parent = getpid();
   			printf("\nParent: My process id is = %d\n", parent);
 			
 			/* Child process is creating */
    			pid_t pid = fork();		
    			if (pid == -1)
  			{
			        printf("Parent: Something wrong, exiting\n");
			        exit(errno);   
			} 
    			else if (pid > 0)
    			{
    				// parent
         			int status;
        
        	 		printf("Parent: I forked a child, its process id is = %d\n\n", pid);
			        printf("Parent: Waiting for child process to finish ...\n\n");
         			
         			waitpid(pid, &status, 0);
       
			        printf("\nParent: Child process is Done\nReturning to Parent Process.\n");
         			
    			}
    			else 
    			{
        			//  child
        			pid_t child = getpid();
        			pid_t myparent = getppid();
        			
        			printf("     PID    PPID   CPU  \tMEM\t\tVSZ\t\tRSS\t START\t   Program\n");
				/* Show information for these pid. */
				RunningProcessOFPID(child);
				RunningProcessOFPID(myparent);
 				sleep(200);
 				//exit(0);
    }	
		}
		// Terminate Process With PID
		else if(op == 3)
		{
			pid_t pid;
			printf(" Enter PID: ");
	    		scanf("%d", &pid);
	    		
	    		/* Send signal to terminate process. */
	    		retval = kill(pid,SIGTERM);
	    		
	    		if (retval == 0)
	    		{
	    			printf("\n Termination Successful. \n");
	    		}
	    		else 
	    		{
	    			printf("\nTermination Failure : %d \n", errno);
    				perror("");
    				printf("\n");    		
    			}
		}
		// Create New Directory
		else if(op == 4)
		{
        		char dirName[20];
    			int ret = 0;
	
			printf("Enter directory name: ");
    			scanf("%s", dirName);
			
			/* Make directory with read, write, execute permission for user, group and others */
    			ret = mkdir(dirName, 0777);
	
    			if (ret == 0)
        		{
        			printf("\nDirectory created successfully\n");
    			}
    			else
    			{
    			        printf("\nUnable to create directory %s\n", dirName);
    			        perror("");
    			}
    		}
    		// Delete Directory
    		else if(op == 5)
		{
			char dirName[16];
    			int ret = 0;
	
    			printf("Enter directory name: ");
    			scanf("%s", dirName);
			
			/* Deleting given directory */
    			ret = rmdir(dirName);
	
    			if (ret == 0)
    			{
       				printf("Given empty directory removed successfully\n");
    			}
    			else
        		{
        			printf("Unable to remove directory %s\n", dirName);
               			perror("");
        		}	
		}
		// Change Path/ Directory
		else if(op == 6)
		{
			/* Dynamically allocation memory. */
			char *change_dir = (char *) malloc (1024);
			
			printf("\nCurrent Directory: ");
			pwd();
			
			printf("\nEnter New Path/Directory: ");
			scanf("%s", change_dir);
			
			/* Changing location */
			int cd = chdir(change_dir);
			
			if(cd == 0)
			{
				printf("\nNew Directory: ");
				pwd();
			}
			else
			{
				perror("");
			}
			
			/* Free the allocated memory. */
			free(change_dir);
		}
		// Create New File
		else if(op == 7)
		{	
			char fileName[20];
			printf("\n Enter A File_Name.extension (filename.txt): ");
       			scanf("%s", fileName);
			
			/* Checking valid extentions */
			if( strstr(fileName,".c") != NULL || strstr(fileName,".cpp") != NULL || 		 
			    strstr(fileName,".cc") != NULL || strstr(fileName,".txt") != NULL || 
			    strstr(fileName,".py") != NULL  )     
			{
				/* Creating File with valid extension*/
				touch(fileName);
			} 
			else 
			{
				printf("\n Invalid File extension \n ");		
       			}
		}
		// Delete File
		else if(op == 8)
		{
			char fileName[20];
    			int del;
    			printf("Enter File_Name: ");
    			scanf("%s", fileName);
			
			/* Deleting given file  */
			del = remove(fileName);
			
			if ( del == 0) {
        			printf("\nThe file is deleted successfully.\n");
    			} else {
        			perror("\nThe file is not deleted.\n");
    			}
		} 
		// Read File
		else if(op == 9)
		{
			char fileName[20];
			int  file;
  			char buffer[BUFFER_SIZE];
  			int  read_size;
  			
  			printf("Enter File_Name: ");
    			scanf("%s", fileName);
			printf("\n\n");
  			
  			/* Open file in Read-only mode. */
  			file = open(fileName, O_RDONLY);
  			
  			/* File not exist. */
  			if (file == -1)
    			{
      				fprintf(stderr, "Error: %s: file not found\n", fileName);
      				//return (-1);
    			}
    			
    			/* Reading the file and assign to buffer. */
  			while ((read_size = read(file, buffer, BUFFER_SIZE)) > 0)
  			{
    				
    				write(1, &buffer, read_size);
			}
			printf("\n");
			
  			close(file);
		}
		// ls
		else if(op == 10)
		{
			DIR *dp = NULL;
			struct dirent *dptr = NULL;
			
			/* Dynamically allocation memory. */
			char *run_dir = (char *) malloc (1024);
			
        		/* Assinging the current Path to run_dir.  */
        		getcwd(run_dir, 1024);
        		
        		/* Open the directory stream*/
			dp = opendir(run_dir);
    			
    			if (dp == NULL)
    			{
    				perror("\nopendir() error\n");
  			} 
  			else 
  			{
				printf("\n The contents of current directory [%s] are as follows\n\n \t  NAME\t\t\t   SIZE\n", run_dir);
        		
        			/* Read the directory contents. */
        			while((dptr = readdir(dp)) != NULL )
        			{
        			
        				long int res = 0;
    					/* Check if entry type is Directory or not. */  
        				if(dptr->d_type == DT_DIR)
        				{
        					DIR *d = opendir( dptr->d_name );
						
						/* Unsuccessful opendir() */
  						if( d == NULL ) 
  						{
    							fprintf( stderr, "Cannot open current working directory\n" );
    							perror("");
    							return 1;
  						}
	
  						struct dirent *de;
 	 					struct stat buf; 
	
			  			for( de = readdir( d ); de != NULL; de = readdir( d ) ) 
			  			{
    							int exists = stat( de->d_name, &buf );
							
							/* Cannot read these file statistics.  */
    							if( exists < 0 ) 
    							{
    						      		continue;
    							} 
    							else 
    							{
      								res = buf.st_size;
    							}
  						}
	
  						closedir( d );
        				} 
        				/* Check if entry type is Regular File or not. */  
        				else if(dptr->d_type == DT_REG)
        				{
        					/* Opening the file. */
        					FILE* fp = fopen(dptr->d_name, "r");
     						        			      			
        					fseek(fp, 0L, SEEK_END);
  	
	    					// calculating the size of the file
    						res = ftell(fp);        			
        				}
           				
            					printf(" %18s\t\t %jd bytes\n", dptr->d_name, (intmax_t)res);
        			}	
        		}
        		printf("\n\n");
        		
        		/* Free the allocated memory. */
        		free(run_dir);
        		
        		/* Close the directory stream. */
        		closedir(dp);
		}
		// Exit Program
		else if(op == 11)
		{
			/* Terminate this program. */
    			kill(getpid(), SIGTERM);
		}
   		else
   		{
			perror("\nEnter valid Command \n");
		}
	}	
	

	return 0;
}

/* -------------------------------------- DRIVER FUNCTION END ------------------------------------ */


int InterfaceAndOption()
{
	int option;
	printf("\n\t1.  All Running Process\n\t2.  Create a Child Process\n\t3.  Terminate Process With PID");
	printf("\n\t4.  Create New Directory\n\t5.  Delete Directory\n\t6.  Go to another Directory\n\t7.  Create New File");
	printf("\n\t8.  Delete File\n\t9.  Read File\n\t10. List of files\n\t11. Exit Program\n\n   Enter Option: ");
	scanf(" %d", &option );

	return option;
}


bool touch(const char *filename)
{
	/* Make directory with read, write, execute permission for user, group and others. */
	int fd = creat(filename, 0777);
	
	if (fd == -1) {
		perror("\n *** Unable to CREATE New FILE. ***\n\n");
		return false;
	} else {
		printf("\n *** Successfully CREATE New FILE. ***\n\n");
		return true;
	}
}


void AllRunningProcess()
{
	/* Opening following proc files. */
	PROCTAB* proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
	
	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));
	
	printf("     PID    PPID   CPU  \tMEM\t\tVSZ\t\tRSS\t START\t   Program\n");
	
	/* Reading all information of those files. */
	while (readproc(proc, &proc_info) != NULL) 
	{
		printf("%7d\t%7d\t%5d\t", proc_info.tid, proc_info.ppid, proc_info.pcpu);
      			printf("%11ld\t%11lu\t%11lu\t", proc_info.resident, proc_info.vm_size, proc_info.vm_rss );
      			printf("%5llu\t   %s\n", proc_info.stime, proc_info.cmd);     		
      	}
      	
	/* Closing the proc files. */
	closeproc(proc);
}


void RunningProcessOFPID(pid_t pid)
{
	/* Opening following proc files. */
	PROCTAB* proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
	
	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));
	
	/* Reading all information of those files. */
	while (readproc(proc, &proc_info) != NULL) 
	{
		/* Printing information only for given PID. */
		if(proc_info.tid == pid)
		{
			printf("%7d\t%7d\t%5d\t", proc_info.tid, proc_info.ppid, proc_info.pcpu);
      			printf("%11ld\t%11lu\t%11lu\t", proc_info.resident, proc_info.vm_size, proc_info.vm_rss );
      			printf("%5llu\t   %s\n", proc_info.stime, proc_info.cmd);         		
      		}
      	}
	
	/* Closing the proc files. */
	closeproc(proc);
}

void pwd()
{
	/* Dynamically allocation memory. */
	char *run_dir = (char *) malloc (1024);
	
	/* Assinging the current Path to run_dir.  */
        if (getcwd(run_dir, 1024) != NULL)
        {	
        	fprintf(stdout, " %s\n", run_dir);
        }
        /* Free the allocated memory. */
        free(run_dir);
}


