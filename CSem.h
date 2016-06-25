#ifndef __CSEM_HEADER
#define __CSEM_HEADER

#include "unp.h"
#include <semaphore.h>

class CSem{
private:
	sem_t m_semid;
	
public:

	CSem(){}
	~CSem(){
	}

	int Init(unsigned int nSeed){
		int r = sem_init(&m_semid,0,nSeed);
		if(r < 0){
			perror("sem init fail!");
			return -1;
		}
		return 0;
	}

	int Post(){
		int r = sem_post(&m_semid);
		if(r < 0){
			perror("sem post error");
			return -1;
		}
		return 0;
	}

	int Wait(){
		int r = sem_wait(&m_semid);
		if(r < 0){
			perror("sem wait error");
			return -1;
		}
		return 0;
	}

	int Destroy(){
		int r = sem_destroy(&m_semid);
		if(r < 0){
			perror("sem destory error");
			return -1;
		}
		return 0;
	}
};

#endif
