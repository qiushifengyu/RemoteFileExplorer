#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxbutton.h>

#include "MFC/resource.h"
#include "MFC/Server/FileExplorerWatcher.h"
#include "Server/Server.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{

enum class ServerStatus
{
	STARTED,
	STOPPED
};

// CServerDialog ��ȭ �����Դϴ�.

class CServerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CServerDialog)

public:
	CServerDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CServerDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER };
#endif

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMfcbuttonServerControl();
	afx_msg LRESULT OnGetLogicalDriveInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDirectoryInfo(WPARAM wParam, LPARAM lParam);

private:
	static const UINT FeWmBase{ WM_APP }; // file explorer window message base

	// TODO: �����Ҵ� �ؾ����� ������...?
	std::unique_ptr<remoteFileExplorer::server::Server> server_;
	ServerStatus status_{ ServerStatus::STOPPED };
	CMFCButton controlButton_;
	CEdit portEdit_;
	CListCtrl logListCtrl_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer