#include "Message/EchoMessage.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
int EchoRequestMessage::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	std::size_t remainedBufferSize = *bufferSize;
	std::size_t preSerializedSize = remainedBufferSize;

	if (ClientMessage::Serialize(buffer, &preSerializedSize) != 0)
		return -1;

	buffer += preSerializedSize;
	remainedBufferSize -= preSerializedSize;

	std::size_t length = string_.length();

	// �ι��ڱ��� ����Ѵ�.
	if (length + 1 > remainedBufferSize)
		return -1;

	// �̹� ������ ũ�Ⱑ �� ���Ǿ����Ƿ� �����ϴ�.
	strcpy(reinterpret_cast<char*>(buffer), string_.c_str());

	*bufferSize = preSerializedSize + length + 1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int EchoReplyMessage::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	std::size_t remainedBufferSize = *bufferSize;
	std::size_t preSerializedSize = remainedBufferSize;

	if (ServerMessage::Serialize(buffer, &preSerializedSize) != 0)
		return -1;

	buffer += preSerializedSize;
	remainedBufferSize -= preSerializedSize;

	std::size_t length = string_.length();

	// �ι��ڱ��� ����Ѵ�.
	if (length + 1 > remainedBufferSize)
		return -1;

	// �̹� ������ ũ�Ⱑ �� ���Ǿ����Ƿ� �����ϴ�.
	strcpy(reinterpret_cast<char*>(buffer), string_.c_str());
	
	*bufferSize = preSerializedSize + length + 1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<EchoRequestMessage> EchoRequestMessage::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	const char* charBuffer = reinterpret_cast<const char*>(buffer);
	return std::make_unique<EchoRequestMessage>(std::string(charBuffer));
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<EchoReplyMessage> EchoReplyMessage::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	const char* charBuffer = reinterpret_cast<const char*>(buffer);
	return std::make_unique<EchoReplyMessage>(std::string(charBuffer));
}

} // namespace message
} // namespace remoteFileExplorer