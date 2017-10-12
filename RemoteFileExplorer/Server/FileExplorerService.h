#pragma once

#include <memory>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
/* TODO: Multithread Safe �ؾ���. (Reentrant�ϸ� �� ����.) */
class FileExplorerService : public FileExplorerServiceInterface
{
public:
	FileExplorerService(std::unique_ptr<FileExplorerWatcherInterface> watcher);

	virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const override;

	virtual int GetLogicalDriveInfo(
		std::vector<common::LogicalDrive>& drives) override;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		std::uint32_t offset,
		common::Directory& dir) override;

private:
	std::shared_ptr<FileExplorerWatcherInterface> watcher_;  // TODO: ����...(shared_ptr����)
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline FileExplorerService::FileExplorerService(
	std::unique_ptr<FileExplorerWatcherInterface> watcher)
	: watcher_(std::move(watcher))
{
}

inline std::unique_ptr<FileExplorerServiceInterface> FileExplorerService::Clone() const
{
	return std::make_unique<FileExplorerService>(*this);
}

} // namespace server
} // namespace remoteFileExplorer