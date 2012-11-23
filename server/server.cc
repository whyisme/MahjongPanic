#include <stdio.h>
#include <unistd.h>
#include "polling.h"

int main(void)
{
	int pid;
	if ( (pid = fork()) == 0)
	{
		printf("Starting Server...\n");
		DataManager *dm = DataManager::getInstance();
		dm->polling();
		return 0;
	}
	printf("Server PID:\t%d\n", pid);
    return 0;
}
