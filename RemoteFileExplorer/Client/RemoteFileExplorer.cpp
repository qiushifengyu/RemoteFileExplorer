#include "RemoteFileExplorer.h"

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

///////////////////////////////////////////////////////////////////////////////
std::string RemoteFileExplorer::Echo(const char * str)
{
	static std::uint8_t buffer[1024]; // TODO: ����ε� ���۰��� �����ϱ�.

	// TODO: ����� �������� ���� üũ�ϱ�.

	message::EchoRequestMessage request(str);
	std::size_t bufferSize = 1024;
	request.Serialize(buffer, &bufferSize);  // TODO: ����ó�� (�ٸ�����)
	serverConnector_.Communicate(buffer, &bufferSize, 1024);
	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() != message::EchoReplyMessage::_MessageFlag)
		return "(((ERROR)))";  // TODO: ����ó�� ��� ����.

	// TODO: ���߿� Message::Cast<T>(notCasted) �̷������� �� �� �ְ� ��������.
	auto& reply =
		message::EchoReplyMessage::TypeCastFromMessage(*replyNotCasted);

	return reply.GetString();
}

} // namespace client
} // namespace remoteFileExplorer