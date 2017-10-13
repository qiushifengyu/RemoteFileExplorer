#pragma once

#include <mutex>
#include <atomic>
#include <memory>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/Network/ListenerThread.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// Client����� ����, ���, ��û ó�� ���� ����ϴ� ���� ���� ��ü�̴�.
// Thread Safe
class Server final
{
public:
    Server(std::unique_ptr<FileExplorerServiceInterface> fileExplorerService)
        : fileExplorerService_(std::move(fileExplorerService)) {}
    ~Server();

    // Non-copyable and Non-moveable.
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    int Start(
        std::uint16_t port,
        std::size_t threadNumber = 0 /* 0 means default number */);
    int Stop();

private:
    std::mutex mutex_;
    std::atomic<bool> started_{ false };
    std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
    std::unique_ptr<network::ListenerThread> listenerThread_;
};

} // namespace server
} // namespace remoteFileExplorer