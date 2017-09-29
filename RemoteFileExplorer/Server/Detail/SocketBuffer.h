#pragma once

#include <winsock2.h>

#include <cstddef>
#include <cstdint>

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
struct SocketBuffer final
{
	static const std::size_t MaxBufferSize = 1024; // TODO: �ڵ����� �������?

	static SocketBuffer* AcquireBuffer();
	static int ReleaseBuffer(SocketBuffer* socketBuffer);

	void Reset();

	OVERLAPPED overlapped;  // �ݵ�� �Ǿտ� �;���.
	std::uint8_t buffer[MaxBufferSize];
	WSABUF wsabuf;
};

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer