#include "Server/Network/ClientHandlerThread.h"

#include "Server/Network/ClientPacketHandler.h"
#include "Server/Network/SocketBuffer.h"

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
	OVERLAPPED* dummy = nullptr;

	while (!terminatedFlag_.load())
	{
		BOOL success = GetQueuedCompletionStatus(
			hCompletionPort_,    // Completion Port
			&bytesTransferred,   // ���۵� ����Ʈ��
			(PULONG_PTR) &clientSession,   // ClientSession ������
			&dummy, // OVERLAPPED ����ü ������.
			10  // 10ms. TODO: �ϵ��ڵ� ���ϱ�.
		);

		// TODO: ����.
		if (!success && dummy == nullptr)
		{
			// �Ƹ��� time-out.
			continue;
		}

		SOCKET hSocket = clientSession->GetSocketHandle();

		if (!success || bytesTransferred == 0) //EOF ���۽�.
		{
			// ���� �߿�!!!
			if (fDestroyClientSession_(hSocket) != 0)
			{

			}
			closesocket(hSocket);
			//SocketBuffer::ReleaseBuffer(socketBuffer);
			continue;
		}

		clientSession->UpdateReceiveBuffer(bytesTransferred);

		std::uint8_t* recvBuffer;
		std::size_t recvBufferSize;

		std::tie(recvBuffer, recvBufferSize) =
			clientSession->GetReceiveBufferStatus();

		std::size_t processedBufferSize = 0;

		while (true)
		{
			if (recvBufferSize < sizeof(std::uint32_t))
				break;

			// TODO: �ý��� ����� ǥ��� ���Ӽ��� ���� ������ �̽� ����.
			std::uint32_t messageSize =
				*reinterpret_cast<std::uint32_t*>(recvBuffer);

			recvBuffer += sizeof(std::uint32_t);
			recvBufferSize -= sizeof(std::uint32_t);
			//processedBufferSize += sizeof(std::uint32_t);

			if (recvBufferSize < messageSize)
				break;

			recvBuffer += messageSize;
			recvBufferSize -= messageSize;
			processedBufferSize += (messageSize + sizeof(std::uint32_t));

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
				// Ignore failure.
				continue;
			}

			WSABUF wsabuf;
			DWORD sendBytes;

			// Send the length of message.
			std::uint32_t sendMessageLength =
				static_cast<std::uint32_t>(sendBufferSize);
			wsabuf.buf = reinterpret_cast<char*>(&sendMessageLength);
			wsabuf.len = sizeof(std::uint32_t);
			// TODO: ���� ��, socket �ݴ������� ����ó���ϱ�. (�ٸ� ����)
			//       Send byte �� �˻�.
			//       Send�� �񵿱������� �ٲٱ�.
			int a = WSASend(hSocket, &wsabuf, 1,
				&sendBytes, 0, nullptr, nullptr);

			int aa = WSAGetLastError();

			// Send the message.
			wsabuf.buf = reinterpret_cast<char*>(sendBuffer);
			wsabuf.len = sendBufferSize;
			int b =  WSASend(hSocket, &wsabuf, 1,
				&sendBytes, 0, nullptr, nullptr);

			int bb = WSAGetLastError();

			delete[] sendBuffer;
		}

		clientSession->ConsumeReceiveBuffer(processedBufferSize);

		if (clientSession->ReceiveBufferIsFull())
		{
			// TODO: ���� message �������ֱ�.
			// Message�� ��⿡ receiver buffer�� ũ�Ⱑ ������ ���,
			// Message�� �׳� ������, ���ο� �޼����� �޴´�.
			clientSession->ResetReceiveBuffer();
		}

		DWORD flags = 0;

		WSARecv(
			hSocket,                     // Ŭ���̾�Ʈ ����
			&clientSession->GetUpdatedWsabufRef(),     // ����
			1,		                     // ������ ��
			nullptr,
			&flags,
			&clientSession->GetInitializedOVELAPPED(), // OVERLAPPED ����ü ������
			nullptr);
	}

	return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer