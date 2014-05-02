#include "polling.h"
#include "sockinit.h"
#include "threadpool.h"

DataManager *DataManager::dm(NULL);

DataManager *DataManager::getInstance()
{
	if (dm == NULL)
		dm = new DataManager();
	return dm;
}

DataManager::DataManager() : receivepool(THREAD_INIT_AMOUNT, this, &DataManager::receiveData), handlepool(THREAD_INIT_AMOUNT, this, &DataManager::handleData), sendpool(THREAD_INIT_AMOUNT, this, &DataManager::sendData)
{

}
int DataManager::setHandleFunc(routineFunc *recvFunc, routineFunc *handleFunc, routineFunc *sendFunc)
{
	handleDU[0] = recvFunc;
	handleDU[1] = handleFunc;
	handleDU[2] = sendFunc;
	return 0;
}

int DataManager::polling()
{
	int listenfd = sockinit(), connfd, nfds, epollfd;
	int n;
    struct epoll_event ev, events[MAX_EVENTS];
    epollfd = epoll_create(10);
    if (epollfd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
	    perror("epoll_ctl: listenfd");
	    exit(EXIT_FAILURE);
	}
	for (;;) {
	    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
	    if (nfds == -1) {
		    perror("epoll_pwait");
			exit(EXIT_FAILURE);
		}
	    for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == listenfd) {
			    connfd = accept4(listenfd, (struct sockaddr *)NULL, NULL, SOCK_NONBLOCK);
				if (connfd == -1) {
					perror("accept");
					exit(EXIT_FAILURE);
				}

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = connfd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
					perror("epoll_ctl: connfd");
					exit(EXIT_FAILURE);
				}
			} else {
			//	do_use_fd(events[n].data.fd);
				receivepool.insertData((void *)(events[n].data.fd));
			}
		}
	}
	return 0;
}

void *DataManager::receiveData(void *pArgs)
{
//	pArgs = (void *)this;
	dataUnit *du = new dataUnit;
	int fd = (long)pArgs & 0xFFFFFFFF;
	int n;
	du->fd = fd;
	char buf[MAX_BUF_SIZE];
	while ( (n = read(fd, buf, MAX_BUF_SIZE)) > 0)
	{
		du->data = malloc(n);
		du->len = n;
		memcpy(du->data, buf, n);
		handlepool.insertData((void *)du);
	}
	return NULL;
}

void *DataManager::handleData(void *pArgs)
{
//	pArgs = (void *)this;
	dataUnit *du = (dataUnit *)pArgs;
	char buf[MAX_BUF_SIZE];
	strcpy(buf, "server: ");
	memcpy(buf + 8, du->data, du->len);
	du->len += 8;
	free(du->data);
	du->data = malloc(du->len);
	memcpy(du->data, buf, du->len);
	sendpool.insertData((void *)du);
	return NULL;
}

void *DataManager::sendData(void *pArgs)
{
//	pArgs = (void *)this;
	dataUnit *du = (dataUnit *)pArgs;
	write(du->fd, du->data, du->len);
	return NULL;
}
