#ifndef __HEAD_FIFO_CLASS
#define __HEAD_FIFO_CLASS
#include "unp.h"
#include <string>
#include <string.h>
#include <sys/stat.h>
class CFifo{
private:
	std::string m_strPath;
	int m_fd;
public:
	CFifo(){}
	int Init(const std::string &path){
		m_strPath = path;
		int mode = 0666 | O_NONBLOCK | O_NDELAY;
		if(mkfifo(path.c_str(),mode)<0){
			if(errno!= EEXIST){
				return -1;
			}
		}
		
		if ((m_fd = open(m_strPath.c_str(), O_RDWR)) < 0) {
		 		return -1;
		}
		int val = fcntl(m_fd, F_GETFL, 0);
		if(val == -1){
			return -1;
		}
		if (val & O_NONBLOCK)
			return 0;
		int ret = fcntl(m_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY);
		if(ret < 0){
			perror("init faile");
			return -1;
		}
		return 0;
	}
	
	int Notify(){
		if(m_fd < 0) return -1;
		int ret = write(m_fd,"\0",1);
		if(ret < 0){
			perror("error on write");
			return -1;
		}
		return 0;
	}

	int Send(const char* str,size_t size = 0){
		if(m_fd < 0) return -1;
		if(size == 0) size = strlen(str);
		int ret = write(m_fd,str,size);
		if(ret < 0){
			perror("write error");
			return -1;
		}
		return ret;
	}

	void Clear(){
		if(m_fd < 0) return;
		static char tmpBuff[128];
		int ret = read(m_fd,tmpBuff,sizeof(tmpBuff));
		if(ret <0){
			perror("read error");
		}
		return;
	}

	int Read(char* buff,size_t size){
		if(m_fd < 0) return -1;
		int ret = read(m_fd,buff,size);
		if(ret < 0){
			perror("read error");
			return -1;
		}
		return ret;
	}


};
#endif
