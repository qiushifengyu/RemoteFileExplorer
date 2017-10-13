#include "Server/Network/ClientHandlerThread.h"

#include "Server/Network/ClientPacketHandler.h"
#include "Server/Network/IOContext.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
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
	ClientSession* clientSession = nullptr;  // TODO: dangling ���� �ذ��ϱ�.
	IOContext* ioContext = nullptr;
	OVERLAPPED* overlapped = nullptr;

	while (!terminatedFlag_.load())
	{
		BOOL success = GetQueuedCompletionStatus(
			hCompletionPort_,    // Completion Port
			&bytesTransferred,   // ���۵� ����Ʈ��
			(PULONG_PTR) &clientSession,   // ClientSession ������
			&overlapped, // OVERLAPPED ����ü ������.
			10  // 10ms. TODO: �ϵ��ڵ� ���ϱ�.
		);

		// TODO: ����.
		if (!success && overlapped == nullptr)
		{
			// �Ƹ��� time-out.
			continue;
		}

		IOContext* ioContext = IOContext::PointerCastFrom(overlapped);
		SOCKET hSocket = clientSession->GetSocketHandle();

		if (!success || bytesTransferred == 0) //EOF ���۽�.
		{
			// ���� �߿�!!!
			(void) fDestroyClientSession_(hSocket);
			(void) closesocket(hSocket);
			delete ioContext;
			continue;
		}

		if (ioContext->GetType() == IOContextType::Send)
		{
			delete ioContext;
		}
		else
		{
			IORecvContext* recvContext = &IORecvContext::TypeCastFrom(*ioContext);
			recvContext->UpdateBuffer(bytesTransferred);

			std::uint8_t* recvBuffer;
			std::size_t recvBufferSize;

			std::tie(recvBuffer, recvBufferSize) =
				recvContext->GetBufferStatus();

			std::size_t processedBufferSize = 0;

			while (true)
			{
				if (recvBufferSize < sizeof(common::message_size_t))
					break;

				// TODO: �ý��� ����� ǥ��� ���Ӽ��� ���� ������ �̽� ����.
				common::message_size_t messageSize =
					*reinterpret_cast<common::message_size_t*>(recvBuffer);

				recvBuffer += sizeof(common::message_size_t);
				recvBufferSize -= sizeof(common::message_size_t);

				if (recvBufferSize < messageSize)
					break;

				recvBuffer += messageSize;
				recvBufferSize -= messageSize;
				processedBufferSize += (messageSize + sizeof(common::message_size_t));

				// TODO: ����� ����.
				std::uint8_t* sendBuffer = new std::uint8_t[64 * 1024];
				std::size_t sendBufferSize = 64 * 1024;

				if (HandleClientPacket(
					*clientSession,
					recvBuffer - messageSize,
					recvBufferSize + messageSize,
					sendBuffer,
					&sendBufferSize) != 0)
				{
					// ���� �߿�!!!
					(void) fDestroyClientSession_(hSocket);
					(void) closesocket(hSocket);
					delete recvContext;
					continue;
				}

				IOSendContext* sendContext = new IOSendContext(
					sizeof(common::message_size_t) + sendBufferSize);
				std::uint8_t* buffer = sendContext->GetBuffer();

				*reinterpret_cast<common::message_size_t*>(buffer)
					= static_cast<common::message_size_t>(sendBufferSize);

				std::memcpy(
					buffer + sizeof(common::message_size_t),
					sendBuffer,
					sendBufferSize);

				// Send the message.
				int b = WSASend(
					hSocket,
					&sendContext->GetUpdatedWsabufRef(),
					1,
					nullptr,
					0,
					&sendContext->GetOverlappedRef(),
					nullptr);

				int bb = WSAGetLastError();

				delete[] sendBuffer;
			}

			recvContext->ConsumeBuffer(processedBufferSize);

			if (recvContext->BufferIsFull())
			{
				(void) fDestroyClientSession_(hSocket);
				(void) closesocket(hSocket);
				delete recvContext;
			}

			DWORD flags = 0;

			WSARecv(
				hSocket,                     // Ŭ���̾�Ʈ ����
				&recvContext->GetUpdatedWsabufRef(),     // ����
				1,		                     // ������ ��
				nullptr,
				&flags,
				&recvContext->GetOverlappedRef(), // OVERLAPPED ����ü ������
				nullptr);
		}
	}

	return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer