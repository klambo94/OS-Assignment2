#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

void handler(int signalNum) {
	switch(signalNum) {
		case 1:
			printf("Process: %d, Signal received: SIGHUP with signal number: %d\n", getpid(), signalNum);
			break;
		case 10:
			printf("Process: %d, Signal received: SIGUSR1 with signal number: %d\n", getpid(), signalNum);
			break;
		case 29:
			printf("Process: %d, Signal received: SIGIO with signal number: %d\n", getpid(), signalNum);
			break;
		default:
			printf("Process: %d, Unrecongized signal: %d\n", getpid(), signalNum);
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
		int waitReturn = waitpid(proccessId, &status, WCONTINUED);
		
		if(waitReturn < 0) {
			int err = errno;
			if(errno == EINTR) {
				while(errno == EINTR) {
					waitReturn = waitpid(proccessId, &status, WCONTINUED);
				}
			}

			 if (errno != EINTR) {
				perror("Error occured during waitpid");
				exit(err);
			}
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
		int sleepTimeInSeconds = 5;

		if(parnetPID > 0) {
			int killReturn = kill(parnetPID, 1); 
			if(killReturn != 0) {
				int err = errno;
				perror("First kill returned errors");
				exit(err);
			}

			int sleepReturn = sleep(sleepTimeInSeconds);
			if(sleepReturn != 0) {
				printf("sleep was inturruped with %d seconds left\n", sleepReturn);
			}

			killReturn = kill(parnetPID, 10); 
			if(killReturn != 0) {
				int err = errno;
				perror("Second ill returned errors");
				exit(err);
			}

			sleepReturn = sleep(sleepTimeInSeconds);
			if(sleepReturn != 0) {
				printf("sleep was inturruped with %d seconds left\n", sleepReturn);
			}

			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Third kill returned errors");
				exit(err);
			}

			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Fourth kill returned errors");
				exit(err);
			}

			killReturn = kill(parnetPID, 29); 
			if(killReturn != 0) {
				int err = errno;
				perror("Fifth kill returned errors");
				exit(err);
			}
		}
	}

	delete action1;
	delete action2;
	delete action3;

	exit(EXIT_SUCCESS);
}