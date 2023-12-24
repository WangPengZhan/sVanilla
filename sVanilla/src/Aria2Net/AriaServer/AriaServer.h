#pragma once
#include <future>

// #include <Windows.h>

namespace aria2net
{

class AriaServer
{
public:
    AriaServer();
    ~AriaServer();

    void startServerAsync();
    void closeServer();
    void forceCloseServer();

    void setErrorFunc(std::function<void()> func);
    void setCloseFunc(std::function<void()> func);

private:
    std::future<bool> m_future;
    std::function<void()> m_errorFunc;
    std::function<void()> m_closeFunc;
};

}  // namespace aria2net
