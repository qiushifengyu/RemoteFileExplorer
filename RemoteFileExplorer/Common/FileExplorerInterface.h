// TODO: #ifndef ������ �ٲٱ� (�ٸ�����)
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