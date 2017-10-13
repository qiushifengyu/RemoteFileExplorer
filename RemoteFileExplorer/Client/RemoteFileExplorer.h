#pragma once

#include <memory>

#include "Client/Network/ServerConnector.h"
#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
class RPCException : public std::exception
{
public:
	const char* what() const noexcept
	{
		return "Can't communicate with a server.";
	}
};

///////////////////////////////////////////////////////////////////////////////
// Thread Unsafe.
// ����������(blocking����) RPC�� �����Ѵ�.
// RPC ���� �� ���� ��Ȳ�� ���� ���, RPCException ���ܸ� ������.
class RemoteFileExplorer final : public common::FileExplorerInterface
{
public:
	RemoteFileExplorer(std::size_t bufferSize = 64 * 1024);
	~RemoteFileExplorer();

	int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
	int Disconnect();

	virtual int GetLogicalDriveInfo(
		std::vector<common::LogicalDrive>& drives) override;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		common::file_count_t offset,
		common::Directory& dir) override;

private:
	network::ServerConnector serverConnector_;
	const std::size_t maxBufferSize_;
	std::uint8_t* buffer_;
};

///////////////////////////////////////////////////////////////////////////////
inline RemoteFileExplorer::RemoteFileExplorer(std::size_t bufferSize)
	: maxBufferSize_(bufferSize)
{
	buffer_ = new std::uint8_t[bufferSize];
}

inline RemoteFileExplorer::~RemoteFileExplorer()
{
	delete[] buffer_;
}

} // namespace client
} // namespace remoteFileExplorer