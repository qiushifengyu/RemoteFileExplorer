#include "MFC/stdafx.h"

#include <afxdialogex.h>
#include <commoncontrols.h>
#include <shellapi.h>

#include <cassert>
#include <algorithm>
#include <map>

#include "MFC/Client/CClientDialog.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{
IMPLEMENT_DYNAMIC(CClientDialog, CDialogEx)

///////////////////////////////////////////////////////////////////////////////
CClientDialog::CClientDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CLIENT, pParent)
{
}

///////////////////////////////////////////////////////////////////////////////
CClientDialog::~CClientDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
BOOL CClientDialog::OnInitDialog()
{
    if (!CDialogEx::OnInitDialog())
        return FALSE;

    //
    ipAddressCtrl_.SetAddress(127, 0, 0, 1);
    portEdit_.SetWindowTextW(_T("9622"));

    //
    systemFileShow_ = false;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, systemFileShow_);
    hiddenFileShow_ = false;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, hiddenFileShow_);

    //
    viewModeComboBox_.AddString(_T("Big Icon"));
    viewModeComboBox_.AddString(_T("Icon"));
    viewModeComboBox_.AddString(_T("Simple"));
    viewModeComboBox_.AddString(_T("Report"));
    viewModeComboBox_.SetCurSel(utils::to_underlying(ViewMode::BigIcon));

    //
    SHFILEINFO sfi = { 0, };
    SHGetFileInfo(
        _T("C:\\"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    // Dir Tree View���� drive�� ������ ������.
    HICON hDriveIcon = sfi.hIcon;

    SHGetFileInfo(
        _T("dummy"),
        FILE_ATTRIBUTE_DIRECTORY,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
    // Dir Tree View���� directory�� ������ ������.
    HICON hDirIcon = sfi.hIcon;

    CImageList* dirTreeImageList = new CImageList;
    dirTreeImageList->Create(16, 16, ILC_COLOR32, 2, 0);
    dirTreeImageList->SetBkColor(RGB(255, 255, 255));
    nImageDrive_ = dirTreeImageList->Add(hDriveIcon);
    nImageDir_ = dirTreeImageList->Add(hDirIcon);
    dirTreeControl_.SetImageList(dirTreeImageList, TVSIL_NORMAL);

    DestroyIcon(hDriveIcon);
    DestroyIcon(hDirIcon);

    //
    bigFileImageList_ = new CImageList;
    bigFileImageList_->Create(64, 64, ILC_COLOR32, 2, 0);
    bigFileImageList_->SetBkColor(RGB(255, 255, 255));

    mediumFileImageList_ = new CImageList;
    mediumFileImageList_->Create(32, 32, ILC_COLOR32, 2, 0);
    mediumFileImageList_->SetBkColor(RGB(255, 255, 255));

    smallFileImageList_ = new CImageList;
    smallFileImageList_->Create(16, 16, ILC_COLOR32, 2, 0);
    smallFileImageList_->SetBkColor(RGB(255, 255, 255));

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_IPADDRESS_REMOTE_IP, ipAddressCtrl_);
    DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, portEdit_);
    DDX_Control(pDX, IDC_MFCBUTTON_CLIENT_CONTROL, controlButton_);

    DDX_Control(pDX, IDC_TREE_DIRECTORY, dirTreeControl_);
    DDX_Control(pDX, IDC_LIST_FILE, fileListControl_);

    DDX_Control(pDX, IDC_CHECK_SHOW_SYSTEM_FILES, systemFileCheckBox_);
    DDX_Control(pDX, IDC_CHECK_SHOW_HIDDEN_FILES, hiddenFileCheckBox_);
    DDX_Control(pDX, IDC_COMBO_VIEW_MODE, viewModeComboBox_);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CClientDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        return TRUE;

    return CDialogEx::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
    ON_BN_CLICKED(IDC_MFCBUTTON_CLIENT_CONTROL,
        &CClientDialog::OnBnClickedMfcbuttonClientControl)
    ON_BN_CLICKED(IDC_CHECK_SHOW_SYSTEM_FILES,
        &CClientDialog::OnBnClickedCheckShowSystemFiles)
    ON_BN_CLICKED(IDC_CHECK_SHOW_HIDDEN_FILES,
        &CClientDialog::OnBnClickedCheckShowHiddenFiles)
    ON_CBN_SELCHANGE(IDC_COMBO_VIEW_MODE,
        &CClientDialog::OnCbnSelchangeComboViewMode)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTORY,
        &CClientDialog::OnTvnSelchangedTreeDirectory)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE,
        &CClientDialog::OnNMDblclkListFile)
    ON_NOTIFY(HDN_ITEMCLICK, 0,
        &CClientDialog::OnHdnItemclickListFile)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedMfcbuttonClientControl()
{
    if (status_ == ClientStatus::Connected)
    {
        DisconnectToServer_();
    }
    else if (status_ == ClientStatus::Disconnected)
    {
        ConnectToServer_();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedCheckShowSystemFiles()
{
    systemFileShow_ = !systemFileShow_;
    CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, systemFileShow_);

    // �ٲ� ������ �����ϵ��� View���� ������Ʈ �Ѵ�.
    UpdateDirTreeViewAll_();
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnBnClickedCheckShowHiddenFiles()
{
    hiddenFileShow_ = !hiddenFileShow_;
    CheckDlgButton(IDC_CHECK_SHOW_HIDDEN_FILES, hiddenFileShow_);

    // �ٲ� ������ �����ϵ��� View���� ������Ʈ �Ѵ�.
    UpdateDirTreeViewAll_();
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnCbnSelchangeComboViewMode()
{
    // �ٲ� ������ �����ϵ��� View���� ������Ʈ �Ѵ�.
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;

    if (hItem == nullptr)
        return;

    FileTree* fileTree = (FileTree*) dirTreeControl_.GetItemData(hItem);

    assert(fileTree->hTreeItem == hItem);

    ClearFileTreeChilds_(fileTree);

    // ������ �������� ��� ���� ������ �����´�.
    // ������ ������ ���� ���, �� ���� ������ �� �����Ƿ�,
    // ���� ���� ���� �����´�.
    common::Directory dir;
    common::file_count_t offset = 0;

    do
    {
        using remoteFileExplorer::client::RPCException;

        try {
            if (remoteFileExplorer_.GetDirectoryInfo(
                fileTree->fullPath,
                offset,
                dir) != 0)
            {
                AfxMessageBox(_T("Fail to get directory information."));
                return;
            }
        }
        catch (const RPCException& e) {
            // RPC�� ������ ���, server���� ������ ���´�.
            AfxMessageBox(CString(e.what()));
            DisconnectToServer_();
            return;
        }

        // ���� ������ �������� FileTree �ڷᱸ���� �����Ѵ�.
        for (const auto& file : dir.fileInfos)
        {
            std::unique_ptr<FileTree> newFileTree = std::make_unique<FileTree>();
            newFileTree->fullPath = dir.path + L'\\' + file.fileName;
            newFileTree->f = std::move(file);
            newFileTree->parent = fileTree;

            // . Ȥ�� .. ���丮�� ������丮�� ǥ�����ش�.
            if (newFileTree->f.fileName == L".")
                newFileTree->specialBehavior = FileSpecialBehavior::CurrentDirectory;
            else if (newFileTree->f.fileName == L"..")
                newFileTree->specialBehavior = FileSpecialBehavior::ParentDirectory;
            else
                newFileTree->specialBehavior = FileSpecialBehavior::None;

            fileTree->childs.push_back(std::move(newFileTree));
        }
        offset += dir.fileInfos.size();

    } while (dir.fileInfos.size() >= 1);

    // Directory Tree View�� ������Ʈ�Ѵ�.
    UpdateDirTreeView_(fileTree);
    dirTreeControl_.Expand(hItem, TVE_EXPAND);

    // File List View�� ������Ʈ�Ѵ�.
    curDirTree_ = fileTree;
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int nItem = pNMItemActivate->iItem;
    
    // �ùٸ��� ���� ���,
    // ����) ������ �ƴ� ������� ����Ŭ���� �߻����� ��
    if (nItem < 0 || nItem >= fileListControl_.GetItemCount())
        return;

    FileTree* fileTree = (FileTree*) fileListControl_.GetItemData(nItem);
    
    assert(fileTree != nullptr);

    // ���Ͽ� ���ؼ� �ƹ��� �ൿ�� ���� �ʴ´�.
    if (fileTree->f.fileType != common::FileType::Directory)
        return;

    if (fileTree->specialBehavior == FileSpecialBehavior::None)
    {
        assert(fileTree->hTreeItem);
        dirTreeControl_.SelectItem(fileTree->hTreeItem);
    }
    else
    {
        // ".." ���丮�� �ǹ��Ѵ�.
        if (fileTree->specialBehavior == FileSpecialBehavior::ParentDirectory)
        {
            assert(fileTree->parent != nullptr);
            assert(fileTree->parent->parent != nullptr);
            dirTreeControl_.SelectItem(fileTree->parent->parent->hTreeItem);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::OnHdnItemclickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    ViewMode viewMode =
        static_cast<ViewMode>(viewModeComboBox_.GetCurSel());

    // Report ��尡 �ƴϸ� �����Ѵ�.
    if (viewMode != ViewMode::Report)
        return;

    // ���� File List View�� ����� ������ �����Ѵ�.
    if (curDirTree_ == nullptr)
        return;

    int nColumn = phdr->iItem;

    if (nColumn < 0 || nColumn > 3)
        return;

    fileListSortAscendFlags_[nColumn] = !fileListSortAscendFlags_[nColumn];

    FileInformationCompare_ compare;

    // �� �Լ��� �����. (�ϴ�, ���������̶�� �����Ͽ�)
    switch (nColumn)
    {
    case 0:  // Name
        compare = [](const auto& f1, const auto& f2) -> int
        {
            return CString(f1.fileName.c_str()).CompareNoCase(f2.fileName.c_str());
        };
        break;
    case 1:  // Modified Date
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.modifiedDate < f2.modifiedDate)
                return -1;
            else if (f1.modifiedDate > f2.modifiedDate)
                return 1;
            else
                return 0;
        };
        break;
    case 2:  // Type
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.fileType < f2.fileType)
                return -1;
            else if (f1.fileType > f2.fileType)
                return 1;
            else
                return 0;
        };
        break;
    case 3:  // Size
        compare = [](const auto& f1, const auto& f2) -> int
        {
            if (f1.fileSize < f2.fileSize)
                return -1;
            else if (f1.fileSize > f2.fileSize)
                return 1;
            else
                return 0;
        };
        break;
    default:
        assert(false);
    }

    // ������������ �����ؾ� �ϴ� ���, ������ ���� ���Լ��� '�����´�'.
    if (!fileListSortAscendFlags_[nColumn])
    {
        compare = [compare](const auto& f1, const auto& f2) -> int
        {
            return compare(f2, f1);
        };
    }

    // Item���� �����Ѵ�.
    fileListControl_.SortItems(&FileListCompareFunc_, (LPARAM) &compare);
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ int CALLBACK CClientDialog::FileListCompareFunc_(
    LPARAM lParam1,
    LPARAM lParam2,
    LPARAM lParamSort)
{
    FileInformationCompare_* compare = (FileInformationCompare_*) lParamSort;

    FileTree* fileTree1 = (FileTree*)lParam1;
    FileTree* fileTree2 = (FileTree*)lParam2;

    assert(fileTree1 != nullptr && fileTree2 != nullptr);

    return (*compare)(fileTree1->f, fileTree2->f);
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ConnectToServer_()
{
    std::uint8_t ipAddress[4];
    std::int16_t port;

    // Get IP Address
    {
        ipAddressCtrl_.GetAddress(
            ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
    }

    // Get Port Number
    {
        CString cstr;
        portEdit_.GetWindowTextW(cstr);

        int len = cstr.GetLength();
        if (len == 0 || len > 5)
        {
            AfxMessageBox(_T("Invalid Port Number!"));
            return;
        }
        int num = _ttoi(cstr.GetBuffer());
        if (num > UINT16_MAX)
        {
            AfxMessageBox(_T("Invalid Port Number!"));
            return;
        }
        port = static_cast<std::uint16_t>(num);
    }

    if (remoteFileExplorer_.Connect(ipAddress, port) != 0)
    {
        AfxMessageBox(_T("Fail to connect the server!"));
        return;
    }

    controlButton_.SetWindowTextW(_T("Disconnect"));
    status_ = ClientStatus::Connected;

    // ������ ������ �� ��, ������ �°� View�� �ʱ�ȭ�Ѵ�.
    if (InitializeView_() != 0)
        DisconnectToServer_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::DisconnectToServer_()
{
    controlButton_.SetWindowTextW(_T("Connect"));
    status_ = ClientStatus::Disconnected;
    ClearView_();
    (void) remoteFileExplorer_.Disconnect(); // �ݵ�� �� �������� �ϱ�.
}

///////////////////////////////////////////////////////////////////////////////
int CClientDialog::InitializeView_()
{
    using remoteFileExplorer::client::RPCException;

    std::vector<common::LogicalDrive> drives;

    try {
        if (remoteFileExplorer_.GetLogicalDriveInfo(drives) != 0)
        {
            AfxMessageBox(_T("Fail to get logical drive informations."));
            return -1;
        }
    }
    catch (const RPCException& e) {
        AfxMessageBox(CString(e.what()));
        return -1;
    }

    fileTreeVRoot_.childs.clear();
    dirTreeControl_.DeleteAllItems();

    auto numDrives = drives.size();
    for (auto i = 0; i < numDrives; ++i)
    {
        CString drivePath = CString(drives[i].letter) + ":";
        CString driveName(drives[i].driveName.c_str());
        
        CString text = driveName + _T('(') + drives[i].letter + _T(":)");

        HTREEITEM hItem = dirTreeControl_.InsertItem(
            text,
            nImageDrive_,
            nImageDrive_);

        std::unique_ptr<FileTree> fileTree = std::make_unique<FileTree>();
        fileTree->f.fileAttr = common::FileAttribute::NoFlag;
        fileTree->f.fileType = common::FileType::Directory;
        fileTree->f.fileName = drivePath;
        fileTree->f.modifiedDate = 0;
        fileTree->fullPath = drivePath;
        fileTree->hTreeItem = hItem;

        dirTreeControl_.SetItemData(hItem, (DWORD_PTR) fileTree.get());
        fileTreeVRoot_.childs.push_back(std::move(fileTree));
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ClearView_()
{
    ClearFileTreeChilds_(&fileTreeVRoot_);
    curDirTree_ = nullptr;
    UpdateFileListView_();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::ClearFileTreeChilds_(FileTree* parentTree)
{
    assert(parentTree != nullptr);

    for (const auto& child : parentTree->childs)
    {
        ClearFileTreeChilds_(child.get());

        if (child->hTreeItem != nullptr)
        {
            if (child->hTreeItem == dirTreeControl_.GetSelectedItem())
            {
                curDirTree_ = nullptr;
                dirTreeControl_.SelectItem(nullptr);
                UpdateFileListView_();
            }

            // item�� �����Ѵ�.
            dirTreeControl_.DeleteItem(child->hTreeItem);
        }
    }
    parentTree->childs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateDirTreeViewAll_()
{
    for (auto& drive : fileTreeVRoot_.childs)
    {
        UpdateDirTreeView_(drive.get());
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateDirTreeView_(FileTree* parentTree)
{
    assert(parentTree->hTreeItem != nullptr);

    HTREEITEM hParent = parentTree->hTreeItem;
    HTREEITEM hInsertAfter = TVI_FIRST;
    auto childCount = parentTree->childs.size();

    for (int i = 0; i < childCount; ++i)
    {
        FileTree* childTree = parentTree->childs[i].get();

        if (childTree->f.fileType != common::FileType::Directory)
            continue;

        // ���� ���丮�� tree view�� ������� �ʴ´�.
        if (childTree->specialBehavior != FileSpecialBehavior::None)
            continue;

        bool show = CheckFileShouldBeShown_(childTree);

        // ���������ϴµ� ������ �ִ� ���,
        if (show && childTree->hTreeItem == nullptr)
        {
            auto hItem = dirTreeControl_.InsertItem(
                CString(childTree->f.fileName.c_str()),
                nImageDir_,
                nImageDir_,
                hParent,
                hInsertAfter);

            assert(hItem != nullptr);
            childTree->hTreeItem = hItem;
            dirTreeControl_.SetItemData(hItem, (DWORD_PTR) childTree);
        }
        // ���������ϴµ� ������ �ִ� ���,
        else if (!show && childTree->hTreeItem != nullptr)
        {
            // �ڽĵ��� ��� �����Ѵ�.
            ClearFileTreeChilds_(childTree);

            // item�� �����Ѵ�.
            dirTreeControl_.DeleteItem(childTree->hTreeItem);
            childTree->hTreeItem = nullptr;
        }

        // �������� ���, �ڽĵ鿡 ���ؼ��� ��������� �˰����� �����ؾ� �Ѵ�.
        if (show)
        {
            // �ڽĵ鿡 ���ؼ��� ������� �˰��� ����.
            UpdateDirTreeView_(childTree);
            hInsertAfter = childTree->hTreeItem;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CClientDialog::UpdateFileListView_()
{
    // List Control�� Column�� Item���� ��� �����.
    auto headerControl = fileListControl_.GetHeaderCtrl();
    if (headerControl != nullptr)
    {
        while (headerControl->GetItemCount() > 0)
            fileListControl_.DeleteColumn(0);
    }
    fileListControl_.DeleteAllItems();

    if (curDirTree_ == nullptr)
        return;

    ViewMode viewMode =
        static_cast<ViewMode>(viewModeComboBox_.GetCurSel());
    
    // ū ������ / ������ / ������ ���,
    if (viewMode == ViewMode::BigIcon ||
        viewMode == ViewMode::Icon ||
        viewMode == ViewMode::Simple)
    {
        FileIconType fileIconType;
        int imageSize;
        UINT styleFlag;
        CImageList* imageList;
        int nImageListType;

        switch (viewMode)
        {
        case ViewMode::BigIcon:
            fileIconType = FileIconType::Big;
            imageSize = 64;
            styleFlag = LVS_ICON;
            imageList = bigFileImageList_;
            nImageListType = TVSIL_NORMAL;
            break;
        case ViewMode::Icon:
            fileIconType = FileIconType::Medium;
            imageSize = 32;
            styleFlag = LVS_ICON;
            imageList = mediumFileImageList_;
            nImageListType = TVSIL_NORMAL;
            break;
        case ViewMode::Simple:
        default:
            fileIconType = FileIconType::Small;
            imageSize = 16;
            styleFlag = LVS_LIST;
            imageList = smallFileImageList_;
            nImageListType = LVSIL_SMALL;
        }

        fileListControl_.ModifyStyle(
            LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, styleFlag);
        fileListControl_.SetImageList(imageList, nImageListType);

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            // ���������ϴ� ������ �ƴϸ� �����Ѵ�.
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            CString fileName(childTree->f.fileName.c_str());
            bool isDir = childTree->f.fileType == common::FileType::Directory;
            int nImage = GetFileIconImageIndex_(fileName, fileIconType, isDir);

            fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }
    }
    // �ڼ��� ���.
    else // ViewMode::Report
    {
        fileListControl_.ModifyStyle(
            LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, LVS_REPORT);

        fileListControl_.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(1, _T("Modified Date"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 90);
        fileListControl_.InsertColumn(3, _T("Size"), LVCFMT_LEFT, 90);
        fileListControl_.SetImageList(smallFileImageList_, LVSIL_SMALL);

        // ���� �̸��� ó���� �̹� ������������ ���ĵ��ִ�.
        fileListSortAscendFlags_[0] = true;
        fileListSortAscendFlags_[1] = fileListSortAscendFlags_[2] =
            fileListSortAscendFlags_[3] = false;

        int nItem = 0;
        for (const auto& childTree : curDirTree_->childs)
        {
            // ���������ϴ� ������ �ƴϸ� �����Ѵ�.
            if (!CheckFileShouldBeShown_(childTree.get()))
                continue;

            CString fileName(childTree->f.fileName.c_str());
            bool isDir = childTree->f.fileType == common::FileType::Directory;
            int nImage = GetFileIconImageIndex_(
                fileName,
                FileIconType::Small,
                isDir);

            tm* tm = std::localtime(&childTree->f.modifiedDate);

            char dateStringBuffer[30];
            snprintf(dateStringBuffer, 30, "%d-%02d-%02d %02d:%02d:%02d",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec);

            auto dateString = CString(dateStringBuffer);
            auto typeString = isDir ? _T("Directory") : _T("File");
            auto sizeString = isDir ? CString(_T("")) :
                CString((std::to_string(childTree->f.fileSize) + " B").c_str());

            int nIndex = fileListControl_.InsertItem(nItem, fileName, nImage);
            fileListControl_.SetItemText(nIndex, 1, dateString);
            fileListControl_.SetItemText(nIndex, 2, typeString);
            fileListControl_.SetItemText(nIndex, 3, sizeString);
            fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
        }

        // Column Width ����.
        assert(fileListControl_.GetHeaderCtrl()->GetItemCount() == 4);
        const double rate[4] = { 0.40, 0.25, 0.15, 0.2 };
        CRect rect;
        fileListControl_.GetClientRect(rect);
        for (int i = 0; i < 4; ++i)
            fileListControl_.SetColumnWidth(i, rect.Width() * rate[i]);

        fileListControl_.SetRedraw(true);
        fileListControl_.UpdateWindow();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool CClientDialog::CheckFileShouldBeShown_(FileTree* fileTree)
{
    bool show = true;

    if (!systemFileShow_ &&
        static_cast<bool>(fileTree->f.fileAttr & common::FileAttribute::System))
        show = false;

    if (!hiddenFileShow_ &&
        static_cast<bool>(fileTree->f.fileAttr & common::FileAttribute::Hidden))
        show = false;

    return show;
}

///////////////////////////////////////////////////////////////////////////////
int CClientDialog::GetFileIconImageIndex_(
    const CString& fileName,
    FileIconType fileIconType,
    bool isDir)
{
    // CString -> file extension.
    // FileIconType -> file icon type.
    // bool -> is directory?
    using MapKey = std::tuple<CString, FileIconType, bool>;

    // ������ GDI Object ������ ImageList.Add()�� ���� ����,
    //   icon image index pool(cache)�� ����� ����.
    // MapKey -> icon image index ���� mapping.
    static std::map<MapKey, int> fileImageMap;
    // MapKeyType -> icon handle ���� mapping.
    //static std::map<MapKey, HICON> fileIconMap;

    // icon�� ��� �� ����, ��ü���ϸ��� �ƴ϶� Ȯ���ڸ� ������ �ȴ�.
    CString extension = _T("dummy");

    if (!isDir)
    {
        // Parse extension from fileName
        int pos = fileName.ReverseFind('.');
        if (pos != -1)
            extension = fileName.Right(fileName.GetLength() - pos).MakeLower();
    }

    // �ʿ��� icon image�� cache�� �̹� �����ϴ����� Ȯ���Ѵ�.
    MapKey mapKey = std::make_tuple(extension, fileIconType, isDir);
    auto it = fileImageMap.find(mapKey);
    if (it != std::end(fileImageMap))
        return it->second;

    HICON hIcon = nullptr;
    SHFILEINFO sfi = { 0, };
    UINT flag = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

    switch (fileIconType)
    {
    case FileIconType::Big:
    case FileIconType::Medium:
        flag |= SHGFI_LARGEICON;
        break;
    case FileIconType::Small:
        flag |= SHGFI_SMALLICON;
        break;
    default:
        assert(false);
    }

    CoInitialize(nullptr);

    // medium icon Ȥ�� small icon�� ���Ѵ�.
    HRESULT hResult = SHGetFileInfo(
        extension,
        (isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
        &sfi,
        sizeof(sfi),
        flag);

    if (FAILED(hResult))
        return -1;

    hIcon = sfi.hIcon;

    // Big icon�� ��û�� ���,
    if (fileIconType == FileIconType::Big)
    {
        // Medium icon�� �̹� ������ ���´�,
        //   �̰� image list�� cache�� �����Ѵ�.
        {
            MapKey mediumMapKey =
                std::make_tuple(extension, FileIconType::Medium, isDir);
            int nImage = mediumFileImageList_->Add(hIcon);
            assert(nImage >= 0);
            fileImageMap[mediumMapKey] = nImage;
        }

        // System image list�� ��´�.
        HIMAGELIST* systemImageList;
        hResult = SHGetImageList(
            SHIL_JUMBO,
            IID_IImageList,
            (void**) &systemImageList);

        if (SUCCEEDED(hResult))
        {
            // System image list���� ����, icon�� ���´�.
            hResult = ((IImageList*) systemImageList)->GetIcon(
                sfi.iIcon,
                ILD_TRANSPARENT,
                &hIcon);

            // Big icon�� ���ϴ� �� ���������Ƿ�,
            //   ������ ���� medium icon�� �����Ѵ�.
            if (SUCCEEDED(hResult))
                DestroyIcon(sfi.hIcon);
        }
    }

    // ���� icon�� image list�� cache�� �����Ѵ�.
    int nImage;
    switch (fileIconType)
    {
    case FileIconType::Big:
        nImage = bigFileImageList_->Add(hIcon);
        break;
    case FileIconType::Medium:
        nImage = mediumFileImageList_->Add(hIcon);
        break;
    case FileIconType::Small:
        nImage = smallFileImageList_->Add(hIcon);
        break;
    default:
        assert(false);
    }
    fileImageMap[mapKey] = nImage;
    DestroyIcon(hIcon);

    return nImage;
}

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer
