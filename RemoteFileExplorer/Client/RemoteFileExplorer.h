#pragma once

#include <memory>

#include "Client/Detail/ServerConnector.h"
#include "Common/FileExplorerInterface.h"
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
	detail::ServerConnector serverConnector_; // TODO: ������Ͽ��� detail ������ �Ⱦ��.
};

} // namespace client
} // namespace remoteFileExplorer