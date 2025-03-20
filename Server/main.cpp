#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include <iostream>

int main()
{
    int count = 3;
    EventLoop event_loop(count);

    TcpServer tcpServer(&event_loop);
    tcpServer.Start("0.0.0.0", 6789);

    std::cout << "!!!main()线程标识: " << std::this_thread::get_id() << std::endl;

    //开启循环
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "hello, world" << std::endl;

    tcpServer.Stop(); //在程序结束时，调用Stop()方法停止服务器。

    return 0;
}
