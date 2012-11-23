#ifndef THREAD_POOL_CC
#define THREAD_POOL_CC
#include "threadpool.h"
template <class X>
threadpool<X>::threadpool(int size, X *pThis, pIncomingFunc start_routine) : m_nSize(size), x(pThis), incoming_routine(start_routine), m_pHead(NULL), m_pTail(NULL), m_pt_mutex(PTHREAD_MUTEX_INITIALIZER), m_pt_cond(PTHREAD_COND_INITIALIZER)
{	
	if (m_nSize > 0)
	{
		m_pPoolList = new pthread_t[m_nSize];
		for (int i = 0; i < m_nSize; i++)
		{
			pthread_create(&m_pPoolList[i], NULL, &routine, this); 
		}
	}
}

//int threadpool::init()
//{
//}

template <class X>
threadpool<X>::~threadpool()
{
	for (int i = 0; i < m_nSize; i++)
	{
		delete m_pPoolList;
	}
	while (m_pHead != NULL)
	{
		m_pTail = m_pHead;
		m_pHead = m_pHead->next;
		delete m_pTail;
	}
}

template <class X>
int threadpool<X>::insertData(void *data)
{
	datanode *pdn = new datanode;
//	pdn->data = malloc(len);
//	memcpy(pdn->data, data, len);
	pdn->data = data;
	pdn->next = NULL;
	pthread_mutex_lock(&m_pt_mutex);
	if (m_pHead == NULL)
	{
		m_pHead = pdn;
		pthread_cond_signal(&m_pt_cond);
	}
	else
		m_pTail->next = pdn;
	m_pTail = pdn;
	pthread_mutex_unlock(&m_pt_mutex);
	return 0;
}

template <class X>
void *threadpool<X>::routine(void *pArgs)
{
	threadpool *ptp = (threadpool *)pArgs;
	while(true)
	{
		pthread_mutex_lock(&(ptp->m_pt_mutex));
		while (ptp->m_pHead == NULL)
			pthread_cond_wait(&(ptp->m_pt_cond), &(ptp->m_pt_mutex));
		datanode *pdn = ptp->m_pHead;
		ptp->m_pHead = ptp->m_pHead->next;
		(ptp->x->*(ptp->incoming_routine))(pdn->data);
		delete pdn;
		pthread_mutex_unlock(&(ptp->m_pt_mutex));
	}
	return NULL;
}

#endif
