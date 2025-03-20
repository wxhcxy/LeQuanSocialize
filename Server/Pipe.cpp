

#include "Pipe.h"
#include "SocketUtil.h"
#include <random>
#include <string>
#include <array>

Pipe::Pipe()
{

}

//创建匿名管道，返回布尔值表示是否成功
bool Pipe::Create()
{
	if (pipe2(pipe_fd_, O_NONBLOCK | O_CLOEXEC) < 0) {
		return false;
	}
	return true;
}

int Pipe::Write(void *buf, int len)
{
    return ::write(pipe_fd_[1], buf, len);
}

int Pipe::Read(void *buf, int len)
{
    return ::read(pipe_fd_[0], buf, len);
}

//关闭管道的读取端和写入端，释放资源
void Pipe::Close()
{
	::close(pipe_fd_[0]);
	::close(pipe_fd_[1]);
}
