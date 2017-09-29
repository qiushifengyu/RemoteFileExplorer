#include "Server/Detail/ClientHandlerThread.h"

#include "Server/Detail/ClientPacketHandler.h"
#include "Server/Detail/SocketBuffer.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
ClientHandlerThread::ClientHandlerThread(
	HANDLE hCompletionPort,
	const std::function<int(SOCKET)>& fDestroyClientSession)
	: hCompletionPort_(hCompletionPort),
	  fDestroyClientSession_(fDestroyClientSession)
{
	thread_ = std::thread(&ClientHandlerThread::ThreadMain_, this);
}

///////////////////////////////////////////////////////////////////////////////
ClientHandlerThread::~ClientHandlerThread()
{
	terminatedFlag_.store(true);
	thread_.join();
}

///////////////////////////////////////////////////////////////////////////////
int ClientHandlerThread::ThreadMain_()
{
	DWORD bytesTransferred;
	ClientSession* clientSession;  // TODO: dangling ���� �ذ��ϱ�.
	SocketBuffer* socketBuffer;

	while (!terminatedFlag_.load())
	{
		BOOL success = GetQueuedCompletionStatus(
			hCompletionPort_,    // Completion Port
			&bytesTransferred,   // ���۵� ����Ʈ��
			(PULONG_PTR) &clientSession,
			(LPOVERLAPPED*) &socketBuffer, // OVERLAPPED ����ü ������.
			10  // 10ms. TODO: �ϵ��ڵ� ���ϱ�.
		);

		if (!success)
		{
			// �Ƹ��� time-out.
			continue;
		}

		SOCKET hSocket = clientSession->GetSocketHandle();

		if (bytesTransferred == 0) //EOF ���۽�.
		{
			// ���� �߿�!!!
			fDestroyClientSession_(hSocket);
			closesocket(hSocket);
			SocketBuffer::ReleaseBuffer(socketBuffer);
			continue;
		}

		std::size_t bufferSize = bytesTransferred;
		HandleClientPacket(
			*clientSession,
			socketBuffer->buffer,
			&bufferSize,
			SocketBuffer::MaxBufferSize);

		socketBuffer->wsabuf.len = (ULONG) bufferSize;
		WSASend(hSocket, &(socketBuffer->wsabuf),
			1, nullptr, 0, nullptr, nullptr);

		DWORD flags = 0;

		socketBuffer->Reset();
		WSARecv(
			hSocket,                     // Ŭ���̾�Ʈ ����
			&(socketBuffer->wsabuf),     // ����
			1,		                     // ������ ��
			nullptr,
			&flags,
			&(socketBuffer->overlapped), // OVERLAPPED ����ü ������
			nullptr);
	}

	return false;
}

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer