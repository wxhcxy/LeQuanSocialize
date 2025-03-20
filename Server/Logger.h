

#ifndef XOP_LOGGER_H
#define XOP_LOGGER_H

#include <cstring>
#include <fstream> //文件流操作，支持将日志写入文件
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

class Logger
{
public:
    //删除拷贝构造函数和拷贝赋值运算符：禁止拷贝构造和拷贝赋值 Logger 对象，确保单例模式的有效性
    Logger &operator=(const Logger &) = delete;
    Logger(const Logger &) = delete;
    static Logger &Instance(); //提供静态方法 Instance()，用于获取 Logger 类的唯一实例，实现单例模式
    ~Logger();

private:
	Logger();

	std::mutex mutex_;
	std::ofstream ofs_;
};

#define LOG_DEBUG(fmt, ...)

#endif

