#include "Server/Server.h"

#include <winsock2.h>

#include <thread>
#include <map>
#include <vector>
#include <atomic>

#include "Server/Network/ListenerThread.h"
#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
Server::~Server()
{
    if (started_)
    {
        Stop();
    }
}

///////////////////////////////////////////////////////////////////////////////
int Server::Start(
    std::uint16_t port,
    std::size_t threadNumber /* 0 means default number */)
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);

    if (started_)
        return -1;

    // client handler thread의 개수는 기본적으로 processor 개수의 2배이다.
    if (threadNumber == 0)
    {
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        threadNumber = systemInfo.dwNumberOfProcessors * 2;
    }

    listenerThread_.reset();
    listenerThread_ = std::make_unique<network::ListenerThread>();
    if (listenerThread_->Start(
        port,
        threadNumber,
        fileExplorerService_->Clone()) != 0)
    {
        return -1;
    }

    started_ = true;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Server::Stop()
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);

    if (!started_)
        return -1;

    listenerThread_.reset();
    started_ = false;

    return 0;
}

} // namespace server
} // namespace remoteFileExplorer