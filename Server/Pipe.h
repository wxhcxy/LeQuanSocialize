

#ifndef XOP_PIPE_H
#define XOP_PIPE_H

#include "TcpSocket.h"

class Pipe
{
public:
	Pipe();
	bool  Create();
	int   Write(void *buf, int len);
	int   Read(void *buf, int len);
	void  Close();

    int Read() const { return pipe_fd_[0]; }  //返回管道的读取端套接字描述符
    int Write() const { return pipe_fd_[1]; } //返回管道的写入端套接字描述符

private:
    int pipe_fd_[2];
    //用于存储管道的文件描述符：
    //  pipe_fd_[0]：管道的读取端
    //  pipe_fd_[1]：管道的写入端
};

#endif
