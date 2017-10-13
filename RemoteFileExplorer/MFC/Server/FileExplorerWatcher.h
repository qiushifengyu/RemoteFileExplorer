#pragma once

#include <windows.h>

#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// File Explorer ���񽺰� �������� ��, �� ���� ��ϵ� �����쿡,
//   Window Message�� Post�Ѵ�.
class FileExplorerWatcher
    : public remoteFileExplorer::server::FileExplorerWatcherInterface
{
public:
    // Window Message ��ȣ�� ����, base�� ������ offset�� �ǹ�.
    enum
    {
        WmOffsetGetLogicalDriveInfo = 0,
        WmOffsetGetDirectoryInfo,

        WmUpperOffset
    };

    FileExplorerWatcher(HWND hWindow, UINT wmBase)
        : hWindow_(hWindow), wmBase_(wmBase) {}

    virtual void GetLogicalDriveInfo() override;
    virtual void GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset) override;

private:
    HWND hWindow_;
    UINT wmBase_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer