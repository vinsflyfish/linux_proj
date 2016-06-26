#ifndef __CSEM_HEADER
#define __CSEM_HEADER

#include "unp.h"
#include <semaphore.h>

class CSem{
private:
	sem_t *m_semid;
	char* m_path;
public:

	CSem(){}
	~CSem(){
	}

	int Init(const char* path,int value = 0){
		size_t size = strlen(path);
		m_path = new char[size+1];
		strncpy(m_path,path,size);
		m_path[size]='\0';
		m_semid = sem_open(m_path,O_CREAT, 0644, value);
		if(m_semid == SEM_FAILED){
			perror("sem init fail!");
			return -1;
		}
		return 0;
	}

	int Post(){
		int r = sem_post(m_semid);
		if(r < 0){
			perror("sem post error");
			return -1;
		}
		return 0;
	}

	int Wait(){
		int r = sem_wait(m_semid);
		if(r < 0){
			perror("sem wait error");
			return -1;
		}
		return 0;
	}

	int Destroy(){
		sem_close(m_semid);
//		int r = sem_unlink(m_path);
//		if(r < 0){
//			perror("sem destory error");
//			return -1;
//		}
//		return 0;
	}
};

#endif
