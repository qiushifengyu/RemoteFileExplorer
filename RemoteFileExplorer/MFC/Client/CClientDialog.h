#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxbutton.h>

#include "MFC/resource.h"
#include "Client/RemoteFileExplorer.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
enum class ClientStatus
{
    Connected,
    Disconnected
};

///////////////////////////////////////////////////////////////////////////////
enum class ViewMode
{
    BigIcon = 0,
    Icon,
    Simple,
    Report
};

///////////////////////////////////////////////////////////////////////////////
enum class FileSpecialBehavior
{
    None,
    CurrentDirectory,  // "." Directory
    ParentDirectory    // ".." Directory
};

///////////////////////////////////////////////////////////////////////////////
enum class FileIconType
{
    Big,
    Medium,
    Small,
};

///////////////////////////////////////////////////////////////////////////////
// Ʈ�� ������ ���丮/���� �����͸������ϱ� ���� �ڷᱸ���̴�.
// Directory Tree Control (=Directory Tree View) ��
//   File List Box Control (=File List View) �� ����
//   backbone �ڷᱸ�� ������ �Ѵ�.
// Dir Tree View�� File List View�� ��� �� �ڷᱸ���� �������� �����ȴ�.
struct FileTree
{
    std::wstring fullPath;
    common::FileInformation f;

    FileTree* parent;
    std::vector<std::unique_ptr<FileTree>> childs; // Only for directory.

    // Directory Tree Control������ item handle�� ��Ÿ����.
    // ���� ���� nullptr�̶��, tree view�� �������� ���� ������ �ǹ��Ѵ�.
    // ������ �׻� �� ���� nullptr�̴�. (tree view�� �������� �����Ƿ�.)
    HTREEITEM hTreeItem{ nullptr }; // Only for directory.
    FileSpecialBehavior specialBehavior;
};

///////////////////////////////////////////////////////////////////////////////
// Client�� UI�� ����Ѵ�.
class CClientDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CClientDialog)

public:
    CClientDialog(CWnd* pParent = nullptr);
    virtual ~CClientDialog() override;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CLIENT };
#endif

protected:
    virtual BOOL OnInitDialog() override;
    virtual void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV �����Դϴ�.

    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedMfcbuttonClientControl();
    afx_msg void OnBnClickedCheckShowSystemFiles();
    afx_msg void OnBnClickedCheckShowHiddenFiles();
    afx_msg void OnCbnSelchangeComboViewMode();
    // Dir Tree View���� ���õ� item�� �ٲ���� �� ȣ��Ǵ� �Լ�.
    afx_msg void OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
    // File List View���� ����Ŭ���� �߻����� �� ȣ��Ǵ� �Լ�.
    afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);

private:
    ///////////////////////////////////////////////////////////////////////////////
    void ConnectToServer_();
    void DisconnectToServer_();

    // ��� View�� �ʱ�ȭ�ϴ� �Լ�.
    int InitializeView_();
    // ��� View�� ���� �Լ�.
    void ClearView_();

    // parentTree�� �ڽ�Ʈ������ ���� �����ϴ� �Լ�.
    // (�ڱ��ڽ��� �������� �ʴ´�.)
    void ClearFileTreeChilds_(FileTree* parentTree);

    // Dir Tree View�� root���� ���� ������Ʈ�ϴ� �Լ�.
    void UpdateDirTreeViewAll_();
    // Dir Tree View���� parentTree�� �ش��ϴ� item������ ������Ʈ�ϴ� �Լ�.
    void UpdateDirTreeView_(FileTree* parentTree);
    // File List View�� ������Ʈ�ϴ� �Լ�.
    void UpdateFileListView_();

    // ������ View�鿡�� ���������ϴ� �� �ƴ����� ��ȯ�ϴ� �Լ�.
    bool CheckFileShouldBeShown_(FileTree* fileTree);

    // File List Control�� ImageList���� file icon image�� index�� ��� �Լ�.
    int GetFileIconImageIndex_(
        const CString& fileName,
        FileIconType fileIconSizeType,
        bool isDir);

    ///////////////////////////////////////////////////////////////////////////////
    remoteFileExplorer::client::RemoteFileExplorer remoteFileExplorer_;
    ClientStatus status_{ ClientStatus::Disconnected };

    CIPAddressCtrl ipAddressCtrl_;
    CEdit portEdit_;
    CMFCButton controlButton_;

    CTreeCtrl dirTreeControl_;
    // ������ root. �� root �ؿ��� drive���� ��ġ�Ѵ�.
    FileTree fileTreeVRoot_;
    // Dir Tree Control�� ImageList���� drive icon image�� index.
    int nImageDrive_;
    // Dir Tree Control�� ImageList���� directory icon image�� index.
    int nImageDir_;

    CListCtrl fileListControl_;
    // File List Control���� ���Ǵ� ImageList��.
    // ���� ū ������, ���� ������, ���� �������� ���� ImageList�̴�.
    CImageList* bigFileImageList_;
    CImageList* mediumFileImageList_;
    CImageList* smallFileImageList_;

    // ���� File List View�� ��Ÿ���� �ִ� directory�� �ǹ��Ѵ�.
    FileTree* curDirTree_{ nullptr };

    CComboBox viewModeComboBox_;

    CButton systemFileCheckBox_;
    bool systemFileShow_;
    CButton hiddenFileCheckBox_;
    bool hiddenFileShow_;
};

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer