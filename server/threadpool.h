#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>

typedef void *(routineFunc) (void *);

struct datanode
{
	void *data;
	datanode *next;
};

template<class X>
class threadpool
{
public:
	typedef void *(X::*pIncomingFunc) (void *);
	threadpool(int size, X* pThis, pIncomingFunc start_routine);
//	int init();
	int insertData(void *data);
	~threadpool();
private:
	X* x;
	static routineFunc routine;
	pIncomingFunc incoming_routine;
	pthread_mutex_t m_pt_mutex;
	pthread_cond_t m_pt_cond;
	pthread_t *m_pPoolList;
	int m_nSize;
	datanode *m_pHead, *m_pTail;
};

#include "threadpool.cc"
#endif

