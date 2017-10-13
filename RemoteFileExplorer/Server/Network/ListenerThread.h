#pragma once

#include <winsock2.h>

#include <mutex>
#include <atomic>
#include <map>
#include <vector>
#include <thread>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/Network/ClientSession.h"
#include "Server/Network/ClientHandlerThread.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
// ���� ������ �����ϸ�, Client���� ������ ����ϰ� �����ϴ� ������ �Ѵ�.
// �׸��� ClientHandlerThread�鵵 ���⼭ �����ȴ�.
class ListenerThread final
{
public:
    ListenerThread() = default;
    ~ListenerThread();

    // Non-copyable and Non-moveable.
    ListenerThread(const ListenerThread&) = delete;
    ListenerThread& operator=(const ListenerThread&) = delete;
    ListenerThread(ListenerThread&&) = delete;
    ListenerThread& operator=(ListenerThread&&) = delete;

    int Start(
        std::uint16_t port,
        std::size_t threadNumber,
        std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);

private:
    int ListenLoop_();

    int DestroyClientSession(SOCKET hSocket);

    std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
    std::uint16_t port_;
    std::size_t threadNumber_;

    std::map<SOCKET, ClientSession> sessionMap_;
    std::mutex sessionMapMutex_;

    std::atomic<bool> isStarted_{ false };
    SOCKET hServerSocket_;
    HANDLE hCompletionPort_;

    std::vector<ClientHandlerThread> handlerThreads_;
    std::atomic<bool> terminatedFlag_{ false };
    std::thread listenerThread_;  // ������ �� �������� �;���. (�Ҹ���� ������)
};

} // namespace network
} // namespace server
} // namespace remoteFileExplorer