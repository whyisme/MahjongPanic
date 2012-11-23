#ifndef POLLING_H

#define POLLING_H
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "constparas.h"
#include "sockinit.h"
#include "threadpool.h"

struct dataUnit
{
	int fd;
	void *data;
	int len;
};

class DataManager
{
public:
	static DataManager *getInstance();
	int polling();
	int setHandleFunc(routineFunc*, routineFunc*, routineFunc*);
private:
	DataManager();
	static DataManager *dm;
	routineFunc receiveData;
	routineFunc handleData;
	routineFunc sendData;
	routineFunc* handleDU[3];
	threadpool<DataManager> receivepool; 
	threadpool<DataManager> handlepool;
	threadpool<DataManager> sendpool;

};

#endif
