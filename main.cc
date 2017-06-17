#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

//Used your write example on slack for my handler printing.
void handler(int signalNum) {
	int returnWrite;
	const char *s;
	switch(signalNum) {
		case 1:
			s = "Signal recieved from child: SIGHUP\n";
			returnWrite = write(1, s, strlen(s));
			if(returnWrite < 0) {
				int err = errno;
				perror("Write returned an error for SIGHUP");
				exit(err);
			}
			break;
		case 10:
			s = "Signal recieved from child: SIGUSR1\n";
			 returnWrite = write(1, s, strlen(s));
			if(returnWrite < 0) {
				int err = errno;
				perror("Write returned an error for SIGUSR1");
				exit(err);
			}
			break;
		case 29:
			s = "Signal recieved from child: SIGIO\n";
			returnWrite = write(1, s, strlen(s));
			if(returnWrite < 0) {
				int err = errno;
				perror("Write returned an error for SIGIO\n");
				exit(err);
			}
			break;
		default:
			s = "Signal recieved from child: UNKNOWN\n";
			returnWrite = write(1, s, strlen(s));
			if(returnWrite < 0) {
				int err = errno;
				perror("Write returned an error for UNKNOWN\n");
				exit(err);
			}
			break;
	}
}

int main(int argc, char *argv[]) {
	int status;
	
	struct sigaction *action1 = new (struct sigaction);
	struct sigaction *action2 = new (struct sigaction);
	struct sigaction *action3 = new (struct sigaction);

	action1->sa_handler = handler;
	sigemptyset(&(action1->sa_mask));
	assert( sigaction (1, action1, NULL) == 0);

	action2->sa_handler = handler;
	sigemptyset(&(action2->sa_mask));
	assert( sigaction (10, action2, NULL) == 0);

	action3->sa_handler = handler;
	sigemptyset(&(action3->sa_mask));
	assert( sigaction (29, action3, NULL) == 0);



	pid_t proccessId = fork();
	if(proccessId < 0) {
		perror("Fork Error");
		exit(EXIT_FAILURE);
	}	


	if(proccessId != 0) {
		printf("Parent PID: %d\n", getpid());
		pid_t waitReturn;
		
		// Talked with people on slack. 
		// Casy Blanton put his waitpid code up on Slack 
		// modified his to fit my code/style 
		do {
			waitReturn = waitpid(proccessId, &status, 0);
		} while(waitReturn == -1 && errno == EINTR);

		if(waitReturn == -1) {
			int err = errno;
			perror("waitpid returned with error");
			exit(err);
		}

		if(WIFEXITED(status)) {
			printf("exited with status=%d\n", WEXITSTATUS(status));
		} else if(WIFSIGNALED(status)) {
			printf("Killed by signal %d\n", WTERMSIG(status));
		} else if(WIFSTOPPED(status)) {
			printf("stopped by signal %d\n", WSTOPSIG(status));
		} else if(WIFCONTINUED(status)) {
			printf("continued\n");
		}
	} else {
		printf("Child PID: %d\n", getpid());
		pid_t parnetPID = getppid();

		if(parnetPID > 0) {
			printf("Process: %d sending signal SIGHUP\n", getpid());
			int killReturn = kill(parnetPID, 1); 
			if(killReturn != 0) {
				int err = errno;
				perror("First kill returned errors");
				exit(err);
			}

			printf("Process: %d sending signal SIGUSR1\n", getpid());
			killReturn = kill(parnetPID, 10); 
			if(killReturn != 0) {
				int err = errno;
				perror("Second kill returned errors");
				exit(err);
			}

			printf("Process: %d sending signal SIGIO\n", getpid());
			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Third kill returned errors");
				exit(err);
			}

			printf("Process: %d sending signal SIGIO\n", getpid());
			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Fourth kill returned errors");
				exit(err);
			}

			printf("Process: %d sending signal SIGIO\n", getpid());
			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Fifth kill returned errors");
				exit(err);
			}
		}
		exit(EXIT_SUCCESS);
	}

	delete action1;
	delete action2;
	delete action3;

	exit(EXIT_SUCCESS);
}