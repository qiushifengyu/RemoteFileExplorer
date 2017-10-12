// CClientDialog.cpp : ���� �����Դϴ�.
//

#include "MFC/stdafx.h"

#include <afxdialogex.h>
#include <commoncontrols.h>
#include <shellapi.h>

#include <map>

#include "MFC/Client/CClientDialog.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{

// CClientDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CClientDialog, CDialogEx)

CClientDialog::CClientDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENT, pParent)
{

}

CClientDialog::~CClientDialog()
{
}

BOOL CClientDialog::OnInitDialog()
{
	if (!CDialogEx::OnInitDialog())
		return FALSE;

	//
	SHFILEINFO sfi = { 0, };
	SHGetFileInfo(
		_T("C:\\"),
		FILE_ATTRIBUTE_DIRECTORY,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
	hDriveIcon_ = sfi.hIcon;

	SHGetFileInfo(
		_T("dummy"),
		FILE_ATTRIBUTE_DIRECTORY,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
	hFolderIcon_ = sfi.hIcon;

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

	return TRUE;
}

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

BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON_CLIENT_CONTROL,
		&CClientDialog::OnBnClickedMfcbuttonClientControl)
	ON_BN_CLICKED(IDC_CHECK_SHOW_SYSTEM_FILES,
		&CClientDialog::OnBnClickedCheckShowSystemFiles)
	ON_BN_CLICKED(IDC_CHECK_SHOW_HIDDEN_FILES,
		&CClientDialog::OnBnClickedCheckShowHiddenFiles)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIRECTORY,
		&CClientDialog::OnTvnSelchangedTreeDirectory)
	ON_CBN_SELCHANGE(IDC_COMBO_VIEW_MODE,
		&CClientDialog::OnCbnSelchangeComboViewMode)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE,
		&CClientDialog::OnNMDblclkListFile)
END_MESSAGE_MAP()

void CClientDialog::OnBnClickedMfcbuttonClientControl()
{
	if (status_ == ClientStatus::Connected)
	{
		controlButton_.SetWindowTextW(_T("Connect"));
		status_ = ClientStatus::Disconnected;
		ClearView_();
		(void) remoteFileExplorer_.Disconnect(); // �ݵ�� �� �������� �ϱ�.
	}
	else if (status_ == ClientStatus::Disconnected)
	{
		std::uint8_t ipAddress[4];
		std::int16_t port;

		// Get IP Address
		{
			// TODO: IP Address Check
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

		if (InitializeView_() != 0)
		{
			AfxMessageBox(_T("Fail to initialize view!"));
			remoteFileExplorer_.Disconnect();
			ClearView_();
			return;
		}

		controlButton_.SetWindowTextW(_T("Disconnect"));

		status_ = ClientStatus::Connected;
	}
}

void CClientDialog::OnBnClickedCheckShowSystemFiles()
{
	systemFileShow_ = !systemFileShow_;
	CheckDlgButton(IDC_CHECK_SHOW_SYSTEM_FILES, systemFileShow_);
	UpdateDirTreeViewAll();
	UpdateFileListView();
}

void CClientDialog::OnBnClickedCheckShowHiddenFiles()
{
	hiddenFileShow_ = !hiddenFileShow_;
	CheckDlgButton(IDC_CHECK_SHOW_HIDDEN_FILES, hiddenFileShow_);
	UpdateDirTreeViewAll();
	UpdateFileListView();
}

// TODO: ����.
HICON GetFileIcon(const CString& fileName, ViewMode viewMode, bool isDir);

void CClientDialog::ClearFileTreeChilds(FileTree* parentTree)
{
	assert(parentTree != nullptr);

	// TODO: ���߿� �����.
	parentTree->beingDeleted = true;

	for (const auto& child : parentTree->childs)
	{
		ClearFileTreeChilds(child.get());

		if (child->hTreeItem != nullptr)
		{
			if (child->hTreeItem == dirTreeControl_.GetSelectedItem())
			{
				curDirTree_ = nullptr;
				dirTreeControl_.SelectItem(nullptr);
				UpdateFileListView();
			}
			dirTreeControl_.DeleteItem(child->hTreeItem);
		}
	}
	parentTree->childs.clear();

	parentTree->beingDeleted = false;
}

void CClientDialog::OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	if (hItem == nullptr)
		return;

	FileTree* fileTree = (FileTree*) dirTreeControl_.GetItemData(hItem);

	assert(fileTree->hTreeItem == hItem);

	if (fileTree->beingDeleted)
		assert(false);

	ClearFileTreeChilds(fileTree);

	// ������ �������� ��� ���� ������ �����´�.
	// ������ ������ ���� ���, �� ���� ������ �� �����Ƿ�,
	// ���� ���� ���� �����´�.
	common::Directory dir;
	std::uint32_t offset = 0;

	do
	{
		if (remoteFileExplorer_.GetDirectoryInfo(
			fileTree->fullPath,
			offset,
			dir) != 0)
		{
			AfxMessageBox(_T("Fail to get directory information."));
			return;
		}

		for (const auto& file : dir.fileInfos)
		{
			std::unique_ptr<FileTree> newFileTree = std::make_unique<FileTree>();
			newFileTree->fullPath = dir.path + L'\\' + file.fileName;
			newFileTree->f = std::move(file);
			newFileTree->parent = fileTree;

			// . Ȥ�� .. ���丮�� ������丮�� ǥ�����ش�.
			if (newFileTree->f.fileName == L".")
				newFileTree->isVirtual = 1;
			else if (newFileTree->f.fileName == L"..")
				newFileTree->isVirtual = 2;
			else
				newFileTree->isVirtual = 0;

			fileTree->childs.push_back(std::move(newFileTree));
		}
		offset += dir.fileInfos.size();

	} while (dir.fileInfos.size() >= 1);

	// Directory Tree View�� ������Ʈ�Ѵ�.
	UpdateDirTreeView(fileTree);

	curDirTree_ = fileTree;

	dirTreeControl_.Expand(hItem, TVE_EXPAND);
	UpdateFileListView();
}

void CClientDialog::OnCbnSelchangeComboViewMode()
{
	UpdateFileListView();
}


void CClientDialog::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = pNMItemActivate->iItem;
	FileTree* fileTree = (FileTree*)fileListControl_.GetItemData(nItem);
	
	assert(fileTree != nullptr);

	if (fileTree->f.fileType != common::FileType::Directory)
		return;

	if (fileTree->isVirtual)
	{
		// ".." ���丮�� �ǹ��Ѵ�.
		if (fileTree->isVirtual == 2)
		{
			assert(fileTree->parent != nullptr);
			assert(fileTree->parent->parent != nullptr);
			dirTreeControl_.SelectItem(fileTree->parent->parent->hTreeItem);
		}
	}
	else
	{
		assert(fileTree->hTreeItem);
		dirTreeControl_.SelectItem(fileTree->hTreeItem);
	}
}

void CClientDialog::UpdateDirTreeViewAll()
{
	for (auto& drive : fileTreeVRoot_.childs)
	{
		UpdateDirTreeView(drive.get());
	}
}

void CClientDialog::UpdateDirTreeView(FileTree* parentTree)
{
	assert(parentTree->hTreeItem != nullptr);

	CImageList* imageList = dirTreeControl_.GetImageList(TVSIL_NORMAL);

	HTREEITEM hParent = parentTree->hTreeItem;
	HTREEITEM hInsertAfter = TVI_LAST;
	auto childCount = parentTree->childs.size();

	// �������� iterate �Ѵ�. (hInsertAfter ������)
	for (int i = childCount - 1; i >= 0; --i)
	{
		FileTree* childTree = parentTree->childs[i].get();

		if (childTree->f.fileType != common::FileType::Directory)
			continue;

		// ���� ���丮�� tree view�� ������� �ʴ´�.
		if (childTree->isVirtual)
			continue;

		bool show = true;

		if (!systemFileShow_ &&
			static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::System))
			show = false;

		if (!hiddenFileShow_ &&
			static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::Hidden))
			show = false;

		// ���������ϴµ� �������� �ִ� ���,
		if (show && childTree->hTreeItem == nullptr)
		{
			auto nImage = imageList->Add(hFolderIcon_);
			auto hItem =
				dirTreeControl_.InsertItem(
					CString(childTree->f.fileName.c_str()),
					nImage, nImage, hParent, hInsertAfter);
			assert(hItem != nullptr);
			childTree->hTreeItem = hItem;
			dirTreeControl_.SetItemData(hItem, (DWORD_PTR) childTree);
		}
		// ���������ϴµ� �������� �ִ� ���,
		else if (!show && childTree->hTreeItem != nullptr)
		{
			// �ڽĵ��� ��� �����Ѵ�.
			ClearFileTreeChilds(childTree);

			// TODO: imageList ����.
			dirTreeControl_.DeleteItem(childTree->hTreeItem);
			childTree->hTreeItem = nullptr;
		}

		// �������� ���, �ڽĵ鿡 ���ؼ��� ��������� �˰����� �����ؾ� �Ѵ�.
		if (show)
		{
			// �ڽĵ鿡 ���ؼ��� ������� �˰��� ����.
			UpdateDirTreeView(childTree);
			hInsertAfter = childTree->hTreeItem;
		}
	}
}

void CClientDialog::UpdateFileListView()
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
		UINT styleFlag;
		int imageSize;
		int nImageList;

		switch (viewMode)
		{
		case ViewMode::BigIcon:
			imageSize = 64;
			styleFlag = LVS_ICON;
			nImageList = TVSIL_NORMAL;
			break;
		case ViewMode::Icon:
			imageSize = 32;
			styleFlag = LVS_ICON;
			nImageList = TVSIL_NORMAL;
			break;
		case ViewMode::Simple:
		default:
			imageSize = 16;
			styleFlag = LVS_LIST;
			nImageList = LVSIL_SMALL;
		}

		fileListControl_.ModifyStyle(
			LVS_ICON | LVS_REPORT | LVS_SMALLICON | LVS_LIST, styleFlag);

		CImageList* imageList = new CImageList;
		bool success = imageList->Create(imageSize, imageSize, ILC_COLOR32, 0, 111);
		assert(success);
		imageList->SetBkColor(RGB(255, 255, 255));

		int nItem = 0;
		for (const auto& childTree : curDirTree_->childs)
		{
			bool show = true;

			if (!systemFileShow_ &&
				static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::System))
				show = false;

			if (!hiddenFileShow_ &&
				static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::Hidden))
				show = false;

			if (!show)
				continue;

			CString fileName(childTree->f.fileName.c_str());
			HICON hIcon = GetFileIcon(
				fileName,
				viewMode,
				childTree->f.fileType == common::FileType::Directory);

			int nImage = imageList->Add(hIcon);

			fileListControl_.InsertItem(nItem, fileName, nImage);
			fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
		}
		fileListControl_.SetImageList(imageList, nImageList);
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

		CImageList* imageList = new CImageList;
		imageList->Create(16, 16, ILC_COLOR32, 0, 111);
		imageList->SetBkColor(RGB(255, 255, 255));

		int nItem = 0;
		for (const auto& childTree : curDirTree_->childs)
		{
			bool show = true;

			if (!systemFileShow_ &&
				static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::System))
				show = false;

			if (!hiddenFileShow_ &&
				static_cast<bool>(childTree->f.fileAttr & common::FileAttribute::Hidden))
				show = false;

			if (!show)
				continue;

			bool isDir = childTree->f.fileType == common::FileType::Directory;
			CString fileName(childTree->f.fileName.c_str());
			HICON hIcon = GetFileIcon(fileName, viewMode, isDir);
			int nImage = imageList->Add(hIcon);

			auto dateString = CString(std::strtok(std::asctime(std::localtime(
				&childTree->f.modifiedDate)), "\n"));
			auto typeString = isDir ? _T("Directory") : _T("File");
			auto sizeString = isDir ? CString(_T("")) :
				CString((std::to_string(childTree->f.fileSize) + " Bytes").c_str());

			int nIndex = fileListControl_.InsertItem(nItem, fileName, nImage);
			fileListControl_.SetItemText(nIndex, 1, dateString);
			fileListControl_.SetItemText(nIndex, 2, typeString);
			fileListControl_.SetItemText(nIndex, 3, sizeString);
			fileListControl_.SetItemData(nItem++, (DWORD_PTR) childTree.get());
		}
		fileListControl_.SetImageList(imageList, LVSIL_SMALL);
	}
}

// TODO: ����.
HICON GetFileIcon(const CString& fileName, ViewMode viewMode, bool isDir)
{
	// (extension, view mode, isDir) -> icon handle ���� mapping.
	static std::map<std::pair<CString, ViewMode>, HICON> fileIconMap;
	static std::map<ViewMode, HICON> dirIconMap;

	CString extension = _T("dummy");

	if (isDir)
	{
		auto it = dirIconMap.find(viewMode);
		if (it != std::end(dirIconMap))
			return it->second;
	}
	else
	{
		// Parse extension from fileName
		int pos = fileName.ReverseFind('.');
		if (pos != -1)
			extension = fileName.Right(pos);

		auto it = fileIconMap.find(std::make_pair(extension, viewMode));
		if (it != std::end(fileIconMap))
			return it->second;
	}

	HICON hIcon = nullptr;
	SHFILEINFO sfi = { 0, };
	UINT flag = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

	const UINT IconSizeFlagMax = 4;
	const UINT IconSizeFlag[IconSizeFlagMax] = {
		SHGFI_SYSICONINDEX, SHGFI_SYSICONINDEX, SHGFI_LARGEICON, SHGFI_SMALLICON
	};
	UINT idx;

	switch (viewMode)
	{
	case ViewMode::BigIcon:
		idx = 1;
		break;
	case ViewMode::Icon:
		idx = 2;
		break;
	case ViewMode::Simple:
	case ViewMode::Report:
	default:
		idx = 3;
	}

	CoInitialize(nullptr);

	for (; idx <= IconSizeFlagMax; ++idx)
	{
		HRESULT hResult = SHGetFileInfo(
			extension,
			(isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL),
			&sfi,
			sizeof(sfi),
			flag);

		if (FAILED(hResult))
			continue;

		if (IconSizeFlag[idx] == SHGFI_SYSICONINDEX) {
			// Retrieve the system image list.
			HIMAGELIST* imageList;
			hResult = SHGetImageList(
				(idx == 0 ? SHIL_JUMBO : SHIL_EXTRALARGE),
				IID_IImageList,
				(void**)&imageList);

			if (FAILED(hResult))
				continue;

			// Get the icon we need from the list. Note that the HIMAGELIST we retrieved
			// earlier needs to be casted to the IImageList interface before use.
			hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
		}
		else {
			hIcon = sfi.hIcon;
		}
	}

	if (isDir)
		dirIconMap[viewMode] = hIcon;
	else
		fileIconMap[std::make_pair(extension, viewMode)] = hIcon;

	return hIcon;
}

int CClientDialog::InitializeView_()
{
	std::vector<common::LogicalDrive> drives;

	if (remoteFileExplorer_.GetLogicalDriveInfo(drives) != 0)
	{
		AfxMessageBox(_T("Fail to get logical drive informations."));
		return -1;
	}

	fileTreeVRoot_.childs.clear();
	dirTreeControl_.DeleteAllItems();

	CImageList* imageListTree = new CImageList;
	imageListTree->Create(16, 16, ILC_COLOR32, 0, 0/*TODO*/);
	imageListTree->SetBkColor(RGB(255, 255, 255));

	auto numDrives = drives.size();
	for (auto i = 0; i < numDrives; ++i)
	{
		CString drivePath = CString(drives[i].letter) + ":";
		CString driveName(drives[i].driveName.c_str());
		
		CString text = driveName + _T('(') + drives[i].letter + _T(":)");

		imageListTree->Add(hDriveIcon_);
		HTREEITEM hItem = dirTreeControl_.InsertItem(text, i, i);

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

	dirTreeControl_.SetImageList(imageListTree, TVSIL_NORMAL);

	return 0;
}

void CClientDialog::ClearView_()
{
	ClearFileTreeChilds(&fileTreeVRoot_);
	curDirTree_ = nullptr;
	UpdateFileListView();
}

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer