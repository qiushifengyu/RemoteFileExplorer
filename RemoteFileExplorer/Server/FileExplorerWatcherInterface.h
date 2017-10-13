#pragma once

#include <string>

#include "Common/CommonType.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// ���� Ž���� ���񽺰� ���� �ɶ�, ��������� �����ϱ� ���� �������̽��̴�.
// ���� Ž���� ���񽺴� ��û�� �����ϸ鼭, �� ���� �����
//   �� �������̽� ��ü�� ���ؼ� �����Ѵ�.
class FileExplorerWatcherInterface
{
public:
    virtual ~FileExplorerWatcherInterface() = default;

    virtual void GetLogicalDriveInfo() = 0;
    virtual void GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset) = 0;
};

} // namespace server
} // namespace remoteFileExplorer