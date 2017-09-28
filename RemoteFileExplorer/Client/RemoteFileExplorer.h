#pragma once

// TODO: ��� �����η� �ٲٱ� (�ٸ�����)
#include "Common/FileExplorerInterface.h"
#include "ServerConnector.h"
#include <memory>
#include "Message/EchoMessage.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
// Thread Unsafe.
class RemoteFileExplorer final : public common::FileExplorerInterface
{
public:
	int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
	int Disconnect();

	// ����������(blocking����) ����.
	virtual std::string Echo(const char* str) override;

private:
	ServerConnector serverConnector_;
};

} // namespace client
} // namespace remoteFileExplorer