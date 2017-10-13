#include "Server/Network/ClientMessageHandler.h"

#include "Message/GetLogicalDriveInfoMessage.h"
#include "Message/GetDirectoryInfoMessage.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
int HandleClientMessage(
	ClientSession& session,
	const std::uint8_t* recvBuffer,
	std::size_t recvBufferSize,
	std::uint8_t* sendBuffer,
	std::size_t& sendBufferSize)
{
	std::unique_ptr<message::Message> message =
		message::Message::Deserialize(recvBuffer, recvBufferSize);

	// ���� �ùٸ� Ŭ���̾�Ʈ �޼����� ó���Ѵ�.
	// ��ȿ���� ���� ��Ŷ�̳� server message�� ó������ �ʴ´�.
	if (message == nullptr || !message->IsClientMessage())
		return -1;

	message::ClientMessage& clientMessage =
		reinterpret_cast<message::ClientMessage&>(*message);

	// Ŭ���̾�Ʈ �޽����� ó���Ѵ�.
	std::unique_ptr<message::ServerMessage> serverMessage =
		HandleClientMessage(session, clientMessage);

	if (serverMessage == nullptr)
		return -1;

	if (serverMessage->Serialize(sendBuffer, sendBufferSize) != 0)
		return -1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<message::ServerMessage> HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage)
{
	using namespace message;

	switch (clientMessage.GetMessageFlag())
	{
	case MessageFlag::GetLogicalDriveInfoRequest:
	{
		GetLogicalDriveInfoRequest& message =
			GetLogicalDriveInfoRequest::TypeCastFrom(clientMessage);

		common::FileExplorerInterface& fileExplorer =
			session.GetFileExplorer();

		common::status_code_t statusCode = 0;
		std::vector<common::LogicalDrive> drives;

		if (fileExplorer.GetLogicalDriveInfo(drives) != 0)
			statusCode = -1;

		return std::make_unique<GetLogicalDriveInfoReply>(
			statusCode,
			std::move(drives));
	}
	break;
	case MessageFlag::GetDirectoryInfoRequest:
	{
		GetDirectoryInfoRequest& message =
			GetDirectoryInfoRequest::TypeCastFrom(clientMessage);

		common::FileExplorerInterface& fileExplorer =
			session.GetFileExplorer();

		common::status_code_t statusCode = 0;
		common::Directory dir;

		if (fileExplorer.GetDirectoryInfo(
			message.GetPathRef(),
			message.GetOffset(),
			dir) != 0)
		{
			statusCode = -1;
		}

		return std::make_unique<GetDirectoryInfoReply>(
			statusCode,
			std::move(dir));
	}
	break;
	default:
		return nullptr;
	}
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer