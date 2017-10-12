#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <memory>

#include "Utils/utils.h"

// TODO: line length 80�� ���� ��Ű���� �ٲٱ�. (�ٸ�����)

// TODO: Message�� ���� �ٽ� �����غ���.....

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
enum class MessageFlag : std::uint8_t
{
	/* ������ ��������� ���� �����ϵ��� �Ѵ�. */
	/* Client Message (Client -> Server) �� ��� flag ���� ¦���̴�. */
	/* Server Message (Server -> Client) �� ��� flag ���� Ȧ���̴�. */
	/* ���̹� ��Ģ --- �⺻�����δ� �Ʒ��� ������.
	     Client Message : xxxReqeust , Server Message : xxxReply */

	GetLogicalDriveInfoRequest = 0x00,
	GetLogicalDriveInfoReply = 0x01,

	GetDirectoryInfoRequest = 0x02,
	GetDirectoryInfoReply = 0x03,

	// ���� ���������� Ȯ��, Message Flag�� �������� ����� ���� ����.
	_ReservedForExtension1 = 0xFE,
	_ReservedForExtension2 = 0xFF
};

///////////////////////////////////////////////////////////////////////////////
class Message
{
public:
	explicit Message(MessageFlag messageFlag) : messageFlag_(messageFlag) {}
	virtual ~Message() = default;

	bool IsClientMessage() const { return utils::to_underlying(messageFlag_) % 2 == 0; }
	bool IsServerMessage() const { return utils::to_underlying(messageFlag_) % 2 == 1; }

	MessageFlag GetMessageFlag() const { return messageFlag_; }

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) = 0;

	static std::unique_ptr<Message> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

private:
	/******************** DATA FIELDS ********************/
	MessageFlag messageFlag_;
};

///////////////////////////////////////////////////////////////////////////////
// Client�� Server���� ������ Message
class ClientMessage : public Message
{
public:
	explicit ClientMessage(MessageFlag messageFlag) : Message(messageFlag) {}

	static ClientMessage& TypeCastFromMessage(Message& message);
};

///////////////////////////////////////////////////////////////////////////////
// Server�� Client���� ������ Message
class ServerMessage : public Message
{
public:
	explicit ServerMessage(MessageFlag messageFlag) : Message(messageFlag) {}

	static ServerMessage& TypeCastFromMessage(Message& message);
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
/*static*/ inline ClientMessage& ClientMessage::TypeCastFromMessage(Message& message)
{
	assert(message.IsClientMessage());
	return reinterpret_cast<ClientMessage&>(message);
}

/*static*/ inline ServerMessage& ServerMessage::TypeCastFromMessage(Message& message)
{
	assert(message.IsServerMessage());
	return reinterpret_cast<ServerMessage&>(message);
}

} // namespace message
} // namespace remoteFileExplorer