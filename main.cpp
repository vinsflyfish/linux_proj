#include<iostream>
#include "unp.h"
#include "CSem.h"
#include <sys/shm.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>

using namespace std;
const int MAX_SHM_SIZE = 1024;
const int MAX_BUFF_SIZE = 1024;

static char __buf[MAX_BUFF_SIZE];

int main(){
	CSem sem;
	ssize_t size =  readlink("/proc/self/exe", __buf, MAX_BUFF_SIZE);
	if(size < 0){
		perror("readlink error");
		return -1;
	}
	printf("current exe file :%s\n",__buf);
	char* strDir = dirname(__buf);
	printf("Current dir is :%s\n",strDir);
	key_t mykey = ftok(strDir,10);
	if(mykey < 0){
		perror("ftok error");
		return -1;
	}
	printf("mykey value is %d\n",mykey);
	if(sem.Init(mykey)<0) return -1;
	int shmid = shmget(mykey,MAX_SHM_SIZE,IPC_CREAT|0660);
	if(shmid < 0){
		perror("shmget error");
		return -1;
	}
	printf("gen key is :%d\n",shmid);
	char* sharedmem = (char*)shmat(shmid,NULL,0);
	if(sharedmem == (char*)-1){
		perror("shmat error");
		return -1;
	}
	memset(sharedmem,0,MAX_SHM_SIZE);
	snprintf(sharedmem,MAX_BUFF_SIZE-1,"pid is :%d",getpid());
	printf("current pid is : %d\n",getpid());
	sem.Post();
	shmdt(sharedmem);
	sem.Destroy();
	return 0;
}
