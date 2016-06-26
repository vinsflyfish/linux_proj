#ifndef _HEADER_CQUEUE
#define _HEADER_CQUEUE
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdint.h>

const uint8_t QUEUE_HEAD_SIZE = sizeof(uint32_t) + sizeof(uint64_t);

class CQueue{
	public:
		char *m_pcBuff;
		unsigned *_head;
		unsigned *_tail;
		char *m_pcBlock;
		uint32_t m_sizeBlock;

	CQueue(char* pBuff,uint32_t size):m_pcBuff(pBuff){
		memset(pBuff,0,QUEUE_HEAD_SIZE);
		m_pcBlock = pBuff + QUEUE_HEAD_SIZE;
		//when in shm the head and tail just in the shm, so other process can assess it!
		_head = (unsigned*)pBuff;
		_tail = _head + 1;
		m_sizeBlock = size - QUEUE_HEAD_SIZE;
	}

	void createHeader(uint64_t flow,uint32_t datalen,char* sHead){
		memcpy(sHead,&datalen,sizeof(datalen));
		memcpy(sHead + sizeof(datalen),&flow,sizeof(flow));
	}

	int enqueue(uint64_t flow,uint32_t datalen,const char* data){
		char sHead[QUEUE_HEAD_SIZE];
		unsigned head = *_head;
		unsigned tail = *_tail;
		size_t freelen = head > tail ? head - tail : head + m_sizeBlock - tail;
		if(freelen <= datalen + QUEUE_HEAD_SIZE) 
			return -1;
		createHeader(flow,datalen,sHead);
		//copy header
		size_t nTailLeft = m_sizeBlock - tail;
		if(nTailLeft >= QUEUE_HEAD_SIZE){
			memcpy(m_pcBlock+tail,sHead,sizeof(sHead));
			*_tail = *_tail + sizeof(sHead);
			tail += sizeof(sHead);
		}
		else{
			memcpy(m_pcBlock + tail,sHead,nTailLeft);
			memcpy(m_pcBlock,sHead+nTailLeft,sizeof(sHead)-nTailLeft);
			*_tail = sizeof(sHead) - nTailLeft;
			tail = sizeof(sHead) - nTailLeft;
		}
		//copy data
		nTailLeft = m_sizeBlock - tail;
		if(nTailLeft >= datalen){
			memcpy(m_pcBlock+ tail,data,datalen);
			*_tail = *_tail + datalen;
			tail += datalen;
		}
		else{
			memcpy(m_pcBlock+ tail,data,nTailLeft);
			memcpy(m_pcBlock,data+nTailLeft,datalen-nTailLeft);
			*_tail = datalen - nTailLeft;
			tail = datalen - nTailLeft;
		}
		return 0;
	}

	int dequeue(uint64_t &flow,uint32_t &datalen,char* data){
		if(*_head == *_tail){
			datalen = 0;
			return 0;
		}
		unsigned head = *_head;
		unsigned tail = *_tail;

		size_t uselen = tail > head ? tail - head : m_sizeBlock + tail - head;
		if(uselen < QUEUE_HEAD_SIZE) return -1;
		size_t tailleft = m_sizeBlock - head;
		char sHead[QUEUE_HEAD_SIZE];
		if(tailleft >= QUEUE_HEAD_SIZE){
			memcpy(sHead,m_pcBlock+head,QUEUE_HEAD_SIZE);
			*_head += QUEUE_HEAD_SIZE;
			head += QUEUE_HEAD_SIZE;
		}
		else{
			size_t headleft = QUEUE_HEAD_SIZE - tailleft;
			memcpy(sHead,m_pcBlock + head,tailleft);
			memcpy(sHead+tailleft,m_pcBlock,headleft);
			*_head = headleft;
			head = headleft;
		}
		datalen = *(uint32_t*)sHead;
		flow = *(uint64_t*)(sHead+sizeof(datalen));

		tailleft = m_sizeBlock - head;
		if(tailleft >= datalen){
			memcpy(data,m_pcBlock + head,datalen);
			*_head += datalen;
			head += datalen;
		}
		else{
			size_t dataleft = datalen - tailleft;
			memcpy(data,m_pcBlock + head,tailleft);
			memcpy(data+tailleft,m_pcBlock,dataleft);
			*_head = dataleft;
			head = dataleft;
		}
		return 0;		
	}

};
#endif
