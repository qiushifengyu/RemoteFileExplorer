#pragma once

#include <winsock2.h>

#include <cassert>
#include <atomic>
#include <mutex>
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
	// ListenerThread���� ClientHandlerThread�� std::vector�� �����ϱ� ������,
	//   �̵� ������� ���ǰ� ���ֱ� �ؾ��Ѵ�.
	// ���� �� ����� ����� ���� ���� ������ ���� ������ ����.
	// ������, ����� ����� �ƴϱ� ������ ���� �ذ��� �ʿ��ϴ�.
	ClientHandlerThread(const ClientHandlerThread&) = delete;
	ClientHandlerThread& operator=(const ClientHandlerThread&) = delete;
	ClientHandlerThread(ClientHandlerThread&&) { assert(false); }
	ClientHandlerThread& operator=(ClientHandlerThread&&) { assert(false); }

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