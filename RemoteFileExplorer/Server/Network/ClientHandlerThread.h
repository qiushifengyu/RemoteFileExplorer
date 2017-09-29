#pragma once

#include <winsock2.h>

#include <mutex>
#include <atomic>
#include <thread>

#include "Server/Network/ClientSession.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
class ClientHandlerThread final
{
public:
	ClientHandlerThread(
		HANDLE hCompletionPort,
		const std::function<int(SOCKET)>& fDestroyClientSession);
	~ClientHandlerThread();

	// Non-copyable and Non-moveable.
	ClientHandlerThread(const ClientHandlerThread&) = delete;
	ClientHandlerThread& operator=(const ClientHandlerThread&) = delete;
	ClientHandlerThread(ClientHandlerThread&&) {}// = delete; // TODO:
	ClientHandlerThread& operator=(ClientHandlerThread&&) {}// = delete; // TODO:

private:
	int ThreadMain_();

	HANDLE hCompletionPort_;
	std::function<int(SOCKET)> fDestroyClientSession_;
	std::atomic<bool> terminatedFlag_{ false };
	std::thread thread_;  // ������ �� �������� �;���. (�Ҹ���� ������)
};

} // namespace network
} // namespace server
} // namespace remoteFileExplorer