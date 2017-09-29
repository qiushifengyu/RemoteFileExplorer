#pragma once

#include <winsock2.h>

#include <mutex>
#include <atomic>
#include <map>
#include <vector>
#include <thread>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/Detail/ClientSession.h"
#include "Server/Detail/ClientHandlerThread.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
class ListenerThread final
{
public:
	ListenerThread(
		std::uint16_t port,
		std::size_t threadNumber,
		std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);
	~ListenerThread();

	// Non-copyable and Non-moveable.
	ListenerThread(const ListenerThread&) = delete;
	ListenerThread& operator=(const ListenerThread&) = delete;
	ListenerThread(ListenerThread&&) = delete;
	ListenerThread& operator=(ListenerThread&&) = delete;

private:
	int ThreadMain_();

	int DestroyClientSession(SOCKET hSocket);

	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
	std::uint16_t port_;
	std::size_t threadNumber_;

	std::map<SOCKET, ClientSession> sessionMap_; // TODO: �� �Ϳ� ���� ����ȭ �߰��ϱ�.

	HANDLE hCompletionPort_;

	std::vector<ClientHandlerThread> handlerThreads_;
	std::atomic<bool> terminatedFlag_{ false };
	std::thread listenerThread_;  // ������ �� �������� �;���. (�Ҹ���� ������)
};

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer