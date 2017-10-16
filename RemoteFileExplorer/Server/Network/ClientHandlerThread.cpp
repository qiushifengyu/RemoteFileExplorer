#include "Server/Network/ClientHandlerThread.h"

#include "Server/ClientMessageHandler.h"
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
    // fDestroyClientSession_ �� ȣ���ϰ� ���� clientSession�� invalidate�ȴ�.
    // �� ���� �����ϵ��� ����!!
    ClientSession* clientSession = nullptr;
    IOContext* ioContext = nullptr;
    OVERLAPPED* overlapped = nullptr;

    while (!terminatedFlag_.load())
    {
        DWORD bytesTransferred;

        // I/O �۾� �Ϸ� ������ �޴´�.
        BOOL success = GetQueuedCompletionStatus(
            hCompletionPort_,
            &bytesTransferred,
            (PULONG_PTR) &clientSession,
            &overlapped,
            10  // 10ms.
        );

        // Time Out.
        if (!success && overlapped == nullptr)
            continue;

        IOContext* ioContext = IOContext::PointerCastFrom(overlapped);
        SOCKET hSocket = clientSession->GetSocketHandle();

        // I/O ���� ���� or ���� ���� (���� ����)
        if (!success || bytesTransferred == 0)
        {
            // ���� �߿�!!!
            (void) fDestroyClientSession_(hSocket);
            delete ioContext;
            continue;
        }

        // �Ϸ�� I/O�� �۽��� ���,
        if (ioContext->GetType() == IOContextType::Send)
        {
            delete ioContext;
        }
        // �Ϸ�� I/O�� ������ ���,
        else
        {
            IORecvContext* recvContext =
                &IORecvContext::TypeCastFrom(*ioContext);

            // ���� ���ۿ� byteTransferred ��ŭ�� ���ŵ����� �˸���.
            recvContext->UpdateBuffer(bytesTransferred);

            std::uint8_t* recvBuffer;
            std::size_t recvBufferSize;

            // ���� '���� ������ ������'��
            //   '���� ������ �����Ͱ� ���ִ� �κ��� ũ��'�� ��´�.
            std::tie(recvBuffer, recvBufferSize) =
                recvContext->GetBufferStatus();

            // ���� ������ ó���� �Ϸ�� �κ��� ũ�⸦ �����ϴ� ����.
            std::size_t processedBufferSize = 0;
            bool sessionDestroyFlag = false;

            // ���� ���� ���� ����Ǿ� �ִ� �޼������� �ĺ��ϰ� ó���Ѵ�.
            while (true)
            {
                // ���Ź��۷� ���� message size�� �б� �õ��Ѵ�.
                if (recvBufferSize < sizeof(common::message_size_t))
                    break;

                common::message_size_t messageSize =
                    *reinterpret_cast<common::message_size_t*>(recvBuffer);

                recvBuffer += sizeof(common::message_size_t);
                recvBufferSize -= sizeof(common::message_size_t);

                // ���� message�� ���Ź��ۿ� �� ��á�� ���,
                //   ó���� �Ұ����ϹǷ� �����Ѵ�.
                if (recvBufferSize < messageSize)
                    break;

                recvBuffer += messageSize;
                recvBufferSize -= messageSize;
                processedBufferSize +=
                    (messageSize + sizeof(common::message_size_t));

                // Thread Local Storage�� ��ġ�ϴ� ����. (message ó���� �ʿ���.)
                thread_local const std::size_t MaxThreadBufferSize = 64 * 1024;
                thread_local std::uint8_t threadBuffer[MaxThreadBufferSize];
                std::size_t threadBufferSize = MaxThreadBufferSize;

                // Message�� ó���Ѵ�.
                if (HandleClientMessage(
                    *clientSession,
                    recvBuffer - messageSize,
                    recvBufferSize + messageSize,
                    threadBuffer,
                    threadBufferSize) != 0)
                {
                    sessionDestroyFlag = true;
                    break;
                }

                // Request ó�� ����� �����ϱ� ���� �غ��Ѵ�.
                IOSendContext* sendContext = new IOSendContext(
                    sizeof(common::message_size_t) + threadBufferSize);
                std::uint8_t* sendBuffer = sendContext->GetBuffer();

                *reinterpret_cast<common::message_size_t*>(sendBuffer)
                    = static_cast<common::message_size_t>(threadBufferSize);

                std::memcpy(
                    sendBuffer + sizeof(common::message_size_t),
                    threadBuffer,
                    threadBufferSize);

                // Reply �޼����� �񵿱� �۽��Ѵ�.
                int ret = WSASend(
                    hSocket,
                    &sendContext->GetUpdatedWsabufRef(),
                    1,
                    nullptr,
                    0,
                    &sendContext->GetOverlappedRef(),
                    nullptr);

                // �۽� ��û�� ���� ���� ���,
                if (ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
                {
                    sessionDestroyFlag = true;
                    break;
                }
            }

            // Ŭ���̾�Ʈ�� ������ �����ϱ⿡ �ɰ��� ������ �߻����� ���,
            //   Ŭ���̾�Ʈ���� ������ ���´�. (������ �����Ѵ�.)
            if (sessionDestroyFlag)
            {
                // ���� �߿�!!!
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }

            // ���Ź��� �� processedBufferSize ��ŭ�� ó�������� �˸���.
            recvContext->ConsumeBuffer(processedBufferSize);

            // ���Ź��۰� ���� ����̴�.
            // �� ���� message �� ���� ũ�Ⱑ ���Ź����� �ִ�ũ�⺸�� ū ����.
            // �̷� ���� ó���� �Ұ����ϹǷ�, Ŭ���̾�Ʈ���� ������ ���´�.
            if (recvContext->CheckBufferIsFull())
            {
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }

            // �񵿱� ���� ��û
            DWORD flags = 0;
            int ret = WSARecv(
                hSocket,
                &recvContext->GetUpdatedWsabufRef(),
                1,
                nullptr,
                &flags,
                &recvContext->GetOverlappedRef(),
                nullptr);

            // ���� ��û�� ���� ���� ���,
            if (ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
            {
                // ���� �߿�!!!
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }
        }
    }

    return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer