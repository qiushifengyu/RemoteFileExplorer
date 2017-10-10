#include "Client/RemoteFileExplorer.h"

#include "Message/GetLogicalDriveInfoMessage.h"
#include "Message/GetDirectoryInfoMessage.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::Connect(std::uint8_t ipAddress[4], std::uint16_t port)
{
	if (serverConnector_.Connect(ipAddress, port) == 0)
		return 0;
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::Disconnect()
{
	if (serverConnector_.Disconnect())
		return 0;
	else
		return -1;
}

int RemoteFileExplorer::GetLogicalDriveInfo(
	std::vector<common::LogicalDrive>& drives)
{
	static std::uint8_t buffer[4096]; // TODO: ����ε� ���۰��� �����ϱ�.
	const std::size_t maxBufferSize = 4096;
	std::size_t bufferSize = 4096;

	// TODO: ����� �������� ���� üũ�ϱ�.

	message::GetLogicalDriveInfoRequest request;
	if (request.Serialize(buffer, &bufferSize) != 0)
		return -1;

	serverConnector_.Communicate(buffer, &bufferSize, maxBufferSize);

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetLogicalDriveInfoReply::_MessageFlag)
	{
		return -1;
	}

	// TODO: ���߿� Message::Cast<T>(notCasted) �̷������� �� �� �ְ� ��������.
	auto& reply =
		message::GetLogicalDriveInfoReply::TypeCastFromMessage(*replyNotCasted);

	drives = reply.GetLogicalDrivesRvalueRef();

	return 0;
}

int RemoteFileExplorer::GetDirectoryInfo(
	const std::wstring& path,
	common::Directory& dir)
{
	static std::uint8_t buffer[4096]; // TODO: ����ε� ���۰��� �����ϱ�.
	const std::size_t maxBufferSize = 4096;
	std::size_t bufferSize = 4096;

	// TODO: ����� �������� ���� üũ�ϱ�.

	message::GetDirectoryInfoRequest request{ std::wstring(path) };
	if (request.Serialize(buffer, &bufferSize) != 0)
		return -1;

	serverConnector_.Communicate(buffer, &bufferSize, maxBufferSize);

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetDirectoryInfoReply::_MessageFlag)
	{
		return -1;
	}

	// TODO: ���߿� Message::Cast<T>(notCasted) �̷������� �� �� �ְ� ��������.
	auto& reply =
		message::GetDirectoryInfoReply::TypeCastFromMessage(*replyNotCasted);

	dir = reply.GetDirectoryRvalueRef();

	return 0;
}

} // namespace client
} // namespace remoteFileExplorer