#pragma once

#include "Server/Network/ClientSession.h"
#include "Message/Message.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// Client���Լ� �� message�� ó���Ѵ�.
// �׸��� �� ó�� ����� �ش��ϴ� message�� sendBuffer�� ��Ƽ� ��ȯ�Ѵ�.
int HandleClientMessage(
    network::ClientSession& session,
    const std::uint8_t* recvBuffer,
    std::size_t recvBufferSize,
    std::uint8_t* sendBuffer,
    std::size_t& sendBufferSize);

///////////////////////////////////////////////////////////////////////////////
// Client���Լ� �� message�� ó���ϴ� �� �ٸ� overload ����.
// �� ������ buffer ���°� �ƴ� �� �� ��üȭ�� message ��ü ���·� ���ȴ�.
std::unique_ptr<message::ServerMessage> HandleClientMessage(
    network::ClientSession& session,
    message::ClientMessage& clientMessage);

} // namespace server
} // namespace remoteFileExplorer