#include "Server/FileExplorerService.h"

#include <windows.h>

#include <algorithm>

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
inline time_t filetime_to_time_t(const FILETIME& ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}
const common::file_count_t MaxGotFileCount = 50;
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
int FileExplorerService::GetLogicalDriveInfo(
    std::vector<common::LogicalDrive>& drives)
{
    using common::LogicalDrive;

    drives.clear();

    DWORD driveMask = GetLogicalDrives();
    if (driveMask == 0)
        return -1;

    char letterStr[] = "A";
    wchar_t drivePath[4] = L"?:\\";
    wchar_t driveName[MAX_PATH + 1];
    for (; driveMask; driveMask >>= 1, ++letterStr[0])
    {
        if (driveMask & 1)
        {
            LogicalDrive drive;
            drive.letter = letterStr[0];

            // Drive ���ڸ� wide character�� ��ȯ�Ͽ� drivePath�� ������Ʈ.
            {
                auto wstr = utils::utf8_to_wstring(std::string(letterStr));

                if (wstr.length() != 1)
                    continue;

                drivePath[0] = wstr[0];
            }

            if (!GetVolumeInformationW(
                drivePath,
                driveName,
                ARRAYSIZE(driveName),
                nullptr, nullptr, nullptr, nullptr, 0))
            {
                // Volume ������ ���� �� ���� ���� �� ������ �����Ѵ�.
                continue;
            }

            drive.driveName = driveName;

            drives.push_back(std::move(drive));
        }
    }

    // ���� ���� ����� �����Ѵ�.
    watcher_->GetLogicalDriveInfo();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int FileExplorerService::GetDirectoryInfo(
    const std::wstring& path,
    common::file_count_t offset,
    common::Directory& dir)
{
    using common::FileInformation;
    using common::FileType;
    using common::FileAttribute;

    std::lock_guard<decltype(mutex_)> lk(mutex_);

    // ��û�� directory�� cache������ �ʰų�, ���ο� ������ �䱸�ϴ� ��쿡��,
    //   ���丮 Ž���� �����Ѵ�.
    if (path != cachedDir_.path || offset == 0)
    {
        cachedDir_.path = path;
        cachedDir_.fileInfos.clear();

        WIN32_FIND_DATA ffd;
        HANDLE hFind;

        hFind = FindFirstFileW(
            (std::wstring(path.c_str()) += L"\\*").c_str(), &ffd);

        if (hFind == INVALID_HANDLE_VALUE)
            return -1;

        bool success = true;
        do
        {
            FileInformation fileInfo;

            fileInfo.fileName = ffd.cFileName;
            fileInfo.modifiedDate =
                filetime_to_time_t(ffd.ftLastWriteTime);
            fileInfo.fileAttr = FileAttribute::NoFlag;

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            {
                fileInfo.fileAttr |= FileAttribute::System;
            }
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            {
                fileInfo.fileAttr |= FileAttribute::Hidden;
            }

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                fileInfo.fileType = FileType::Directory;
                fileInfo.fileSize = 0;
            }
            else
            {
                fileInfo.fileType = FileType::File;

                LARGE_INTEGER filesize;
                filesize.LowPart = ffd.nFileSizeLow;
                filesize.HighPart = ffd.nFileSizeHigh;

                fileInfo.fileSize = filesize.QuadPart;
            }

            cachedDir_.fileInfos.push_back(std::move(fileInfo));

        } while (success = FindNextFileW(hFind, &ffd));

        if (!success && GetLastError() != ERROR_NO_MORE_FILES)
            return -1;

        FindClose(hFind);
    }

    // ��û�� directory ������ �����Ѵ�.
    dir.path = path;
    dir.fileInfos.clear();

    auto offsetLimit = (std::min)(
        static_cast<common::file_count_t>(cachedDir_.fileInfos.size()),
        offset + MaxGotFileCount);

    for (auto i = offset; i < offsetLimit; ++i)
        dir.fileInfos.push_back(cachedDir_.fileInfos[i]);

    // ���� ���� ����� �����Ѵ�.
    watcher_->GetDirectoryInfo(path, offset);

    return 0;
}

} // namespace server
} // namespace remoteFileExplorer