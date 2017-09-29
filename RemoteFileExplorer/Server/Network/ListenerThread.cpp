#include "Server/Network/ListenerThread.h"

#include "Server/Network/SocketBuffer.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
ListenerThread::ListenerThread(
	std::uint16_t port,
	std::size_t threadNumber,
	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService)
	: port_(port),
	  threadNumber_(threadNumber),
	  fileExplorerService_(std::move(fileExplorerService))
{
	listenerThread_ = std::thread(&ListenerThread::ThreadMain_, this);
}

///////////////////////////////////////////////////////////////////////////////
ListenerThread::~ListenerThread()
{
	// ����, client handler thread���� �����Ų��.
	handlerThreads_.clear();

	// �״���, listener thread�� �����Ų��.
	terminatedFlag_.store(true);
	listenerThread_.join();
}

///////////////////////////////////////////////////////////////////////////////
int ListenerThread::ThreadMain_()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	hCompletionPort_ =
		CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

	handlerThreads_.reserve(threadNumber_);
	for (std::size_t i = 0; i < threadNumber_; ++i)
	{
		handlerThreads_.emplace_back(
			hCompletionPort_,
			[this](SOCKET hSocket) -> int
			{
				return this->DestroyClientSession(hSocket);
			});
	}

	SOCKET hServerSocket =
		WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port_);

	bind(hServerSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));

	listen(hServerSocket, (int)handlerThreads_.size());

	// ������ non-blocking���� ����.
	u_long cmdArg = 1;
	if (ioctlsocket(hServerSocket, FIONBIO, &cmdArg) == SOCKET_ERROR)
		return -1;

	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(hServerSocket, &fdSet);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10 * 1000; // 10ms // TODO:

	while (!terminatedFlag_.load())
	{
		SOCKET hClientSocket;
		SOCKADDR_IN clientAddress;
		int addrLen = sizeof(clientAddress);

		FD_ZERO(&fdSet);
		FD_SET(hServerSocket, &fdSet);

		int selectRet = select(1, &fdSet, nullptr, nullptr, &timeout);

		if (selectRet == 0)
			continue; // timeout
		else if (selectRet == SOCKET_ERROR)
		{
			int eee = WSAGetLastError();
			continue; // TODO: ��� ó��??
		}

		hClientSocket = accept(hServerSocket, (SOCKADDR*)&clientAddress, &addrLen);
		// TODO: ����ó�� ö���� �ϱ� (�ٸ� ���� �ٸ����� �ٸ� �ƶ�!!!)

		auto result = sessionMap_.insert(std::make_pair(hClientSocket,
			ClientSession(hClientSocket, clientAddress,
				fileExplorerService_->Clone())
		));

		if (!result.second)
		{
			// �̹� ������ �����ϴ� ���.
			// �̻��� ��Ȳ!!!
			closesocket(hClientSocket);
			continue;
		}

		ClientSession* clientSession = &(result.first->second);

		CreateIoCompletionPort(
			(HANDLE)hClientSocket, hCompletionPort_, (ULONG_PTR)clientSession, 0/*TODO:�˻�*/);

		// ����� Ŭ���̾�Ʈ�� ���� ���۸� �����ϰ� OVERLAPPED ����ü ���� �ʱ�ȭ.
		SocketBuffer* socketBuffer = SocketBuffer::AcquireBuffer();
		if (socketBuffer == nullptr)
		{
			// ����!!!!
			// TODO: ��� ó���ϱ�.
			continue;
		}

		DWORD flags = 0;

		WSARecv(
			hClientSocket,               // Ŭ���̾�Ʈ ����
			&(socketBuffer->wsabuf),     // ����
			1,		                     // ������ ��
			nullptr,
			&flags,
			&(socketBuffer->overlapped), // OVERLAPPED ����ü ������
			nullptr
		);
	}

	closesocket(hServerSocket);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ListenerThread::DestroyClientSession(SOCKET hSocket)
{
	auto it = sessionMap_.find(hSocket);
	if (it != std::end(sessionMap_))
		return -1;

	sessionMap_.erase(it);
	return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer