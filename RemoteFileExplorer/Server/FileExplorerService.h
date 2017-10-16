#pragma once

#include <memory>
#include <mutex>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// ���� Ž���� ���񽺸� �����Ѵ�.
// ������ ���ÿ��� ���� Ž���⿡ ���õ� ��ɵ��� �����ϴ� ������ �Ѵ�.
// Thread Safe
class FileExplorerService : public FileExplorerServiceInterface
{
public:
    FileExplorerService(std::unique_ptr<FileExplorerWatcherInterface> watcher);
    FileExplorerService(const FileExplorerService& other);

    virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const override;

    virtual int GetLogicalDriveInfo(
        std::vector<common::LogicalDrive>& drives) override;
    virtual int GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset,
        common::Directory& dir) override;

private:
    mutable std::mutex mutex_;  // cachedDir_�� ���� ����ȭ�� ���� ���ȴ�.
    common::Directory cachedDir_{};
    std::shared_ptr<FileExplorerWatcherInterface> watcher_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline FileExplorerService::FileExplorerService(
    std::unique_ptr<FileExplorerWatcherInterface> watcher)
    : watcher_(std::move(watcher))
{
}

inline FileExplorerService::FileExplorerService(const FileExplorerService& other)
{
    std::lock_guard<decltype(other.mutex_)> lk(other.mutex_);
    cachedDir_ = other.cachedDir_;
    watcher_ = other.watcher_;
}

inline std::unique_ptr<FileExplorerServiceInterface> FileExplorerService::Clone() const
{
    return std::make_unique<FileExplorerService>(*this);
}

} // namespace server
} // namespace remoteFileExplorer