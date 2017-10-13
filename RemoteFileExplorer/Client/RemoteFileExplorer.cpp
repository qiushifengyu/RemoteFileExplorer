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
	if (serverConnector_.Disconnect() == 0)
		return 0;
	else
		return -1;
}

int RemoteFileExplorer::GetLogicalDriveInfo(
	std::vector<common::LogicalDrive>& drives)
{
	static const std::size_t MaxBufferSize = 64 * 1024;
	static std::uint8_t buffer[MaxBufferSize]; // TODO: ����ε� ���۰��� �����ϱ�.
	std::size_t bufferSize = MaxBufferSize;

	// TODO: ����� �������� ���� üũ�ϱ�.

	message::GetLogicalDriveInfoRequest request;
	if (request.Serialize(buffer, &bufferSize) != 0)
		throw RPCException();

	if (serverConnector_.Communicate(buffer, &bufferSize, MaxBufferSize) != 0)
		throw RPCException();

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetLogicalDriveInfoReply::_MessageFlag)
	{
		throw RPCException();
	}

	// TODO: ���߿� Message::Cast<T>(notCasted) �̷������� �� �� �ְ� ��������.
	auto& reply =
		message::GetLogicalDriveInfoReply::TypeCastFromMessage(*replyNotCasted);

	drives = reply.GetLogicalDrivesRvalueRef();

	return reply.GetStatusCode();
}

int RemoteFileExplorer::GetDirectoryInfo(
	const std::wstring& path,
	common::file_count_t offset,
	common::Directory& dir)
{
	static const std::size_t MaxBufferSize = 64 * 1024;
	static std::uint8_t buffer[MaxBufferSize]; // TODO: ����ε� ���۰��� �����ϱ�.
	std::size_t bufferSize = MaxBufferSize;

	// TODO: ����� �������� ���� üũ�ϱ�.

	message::GetDirectoryInfoRequest request{ std::wstring(path), offset };
	if (request.Serialize(buffer, &bufferSize) != 0)
		throw RPCException();

	if (serverConnector_.Communicate(buffer, &bufferSize, MaxBufferSize) != 0)
		throw RPCException();

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetDirectoryInfoReply::_MessageFlag)
	{
		throw RPCException();
	}

	// TODO: ���߿� Message::Cast<T>(notCasted) �̷������� �� �� �ְ� ��������.
	auto& reply =
		message::GetDirectoryInfoReply::TypeCastFromMessage(*replyNotCasted);
	
	dir = reply.GetDirectoryRvalueRef();

	return reply.GetStatusCode();
}

} // namespace client
} // namespace remoteFileExplorer