#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "Common/CommonType.h"
#include "Common/FileSystem.h"

namespace remoteFileExplorer
{
namespace common
{
///////////////////////////////////////////////////////////////////////////////
// RPC (Remote Procedure Call)�� ���� �ٽ� �������̽�.
// RPC Client�� Server �� ��� �� �������̽��� ��ӹ޾� �����ؾ��Ѵ�.
// RPC Client ��������, Server�� ���ν��� ȣ���� ��û�ϰ� �� ����� ��ȯ�ؾ��Ѵ�.
// RPC Server ��������, ���ÿ��� ���ν����� �����ϰ� �� ����� ��ȯ�Ѵ�.
// RPC ���� �������̽��� ���� ���������μ�, RPC ���� �ϰ�����
//   C++ Ÿ�� �ý������� ������ �� �ִ�. (�������� ��, �ϰ����� �˻�ȴٴ� ��.)
class FileExplorerInterface
{
public:
	virtual ~FileExplorerInterface() = default;

	virtual int GetLogicalDriveInfo(std::vector<LogicalDrive>& drives) = 0;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		file_count_t offset,
		Directory& dir) = 0;
};

} // namespace common
} // namespace remoteFileExplorer