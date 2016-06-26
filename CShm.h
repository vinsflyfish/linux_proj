#ifndef __HEAD_CSHM
#define __HEAD_CSHM
#include "unp.h"
#include <sys/shm.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>



class CShm{
private:
	int m_nShmid;
	key_t m_key;
	char* m_pcMemory;
	size_t m_nSize;
public:
	static const int MAX_SHM_SIZE = 2048;
	static const int MAX_BUFF_SIZE = 1024;
	char __buf[MAX_BUFF_SIZE];

	CShm(int shmid,key_t key,char* buff,size_t size):m_nShmid(shmid),m_key(key),m_pcMemory(buff),m_nSize(size){
	}
	CShm(){}

	key_t GenKeyByExecDir(){
		memset(__buf,'\0',sizeof(__buf));
		ssize_t size =  readlink("/proc/self/exe", __buf, MAX_BUFF_SIZE);
		if(size < 0){
			perror("readlink error");
			return -1;
		}
		char* strDir = dirname(__buf);
		key_t mykey = ftok(strDir,10);
		if(mykey < 0){
			perror("ftok error");
			return -1;
		}
		return mykey;
	}

	int Init(size_t size = 0,key_t key = -1){
		if(key == -1)
			m_key = GenKeyByExecDir();
		else{
			m_key = key;
		}
		if(m_key == -1){
			return -1;
		}
		if(size == 0){
			m_nShmid = shmget(m_key,MAX_SHM_SIZE,IPC_CREAT|0660);
			m_nSize = MAX_SHM_SIZE;
		}
		else{
			m_nShmid = shmget(m_key,size,IPC_CREAT|0660);
			m_nSize = size;
		}
		if(m_nShmid < 0){
			perror("shmget error");
			return -1;
		}
		return 0;
	}

	char* attach(){
		m_pcMemory = (char*)shmat(m_nShmid,NULL,0);
		if(m_pcMemory == (char*)-1){
			perror("shmat error");
			return NULL;
		}
		return m_pcMemory;
	}

	void detach(){
		shmdt(m_pcMemory);
	}

	char* Memory(){
		return m_pcMemory;
	}

	size_t Size(){
		return m_nSize;
	}
};
#endif
