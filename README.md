# linux_proj
共享内存与信号量的基本使用demo

简单的Makefile内容如下：
```
all:main read
.PHONY:all

main:main.cpp
        g++ -g -o main $^ -lpthread
        
read:read.cpp
        g++ -g -o read $^ -lpthread

clean:
        rm -rf main read
```

