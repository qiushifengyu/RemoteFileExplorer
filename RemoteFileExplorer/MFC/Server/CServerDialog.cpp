// CServerDialog.cpp : ���� �����Դϴ�.
//

#include "MFC/stdafx.h"
#include "MFC/Server/CServerDialog.h"
#include "Server/FileExplorerService.h"
#include "afxdialogex.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{

// �̰� ������ Ÿ���� �ʹ� ����;;
using namespace remoteFileExplorer::server;

// CServerDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CServerDialog, CDialogEx)

CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER, pParent)
{
}

CServerDialog::~CServerDialog()
{
}

BOOL CServerDialog::OnInitDialog()
{
	if (CDialogEx::OnInitDialog() == FALSE) {
		return FALSE;
	}

	server_ = std::make_unique<Server>(
		std::make_unique<FileExplorerService>(
			std::make_unique<FileExplorerWatcher>(GetSafeHwnd(), FeWmBase)));

	logListCtrl_.InsertColumn(0, _T("Category"), LVCFMT_CENTER, 90);
	logListCtrl_.InsertColumn(1, _T("Log"), LVCFMT_LEFT, 90);
	logListCtrl_.InsertColumn(2, _T("Etc"), LVCFMT_LEFT, 90);

	return TRUE;
}

void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCBUTTON_SERVER_CONTROL, controlButton_);
	DDX_Control(pDX, IDC_EDIT_LISTEN_PORT, portEdit_);
	DDX_Control(pDX, IDC_LIST_SERVER_LOG, logListCtrl_);
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON_SERVER_CONTROL,
		&CServerDialog::OnBnClickedMfcbuttonServerControl)
	ON_MESSAGE(CServerDialog::FeWmBase + FileExplorerWatcher::WmOffsetEcho,
		&CServerDialog::OnFeEcho)
END_MESSAGE_MAP()


// CServerDialog �޽��� ó�����Դϴ�.
void CServerDialog::OnBnClickedMfcbuttonServerControl()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (status_ == ServerStatus::STARTED)
	{
		server_->Stop();
		controlButton_.SetWindowTextW(_T("Run Server"));

		status_ = ServerStatus::STOPPED;
	}
	else if (status_ == ServerStatus::STOPPED)
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
		std::uint16_t port = static_cast<std::uint16_t>(num);

		if (server_->Start(port) != 0)
		{
			AfxMessageBox(_T("Fail to start server!"));
			return;
		}

		controlButton_.SetWindowTextW(_T("Stop Server"));

		status_ = ServerStatus::STARTED;
	}
}

LRESULT CServerDialog::OnFeEcho(WPARAM wParam, LPARAM lParam)
{
	//TODO: �Ʒ����� �ұ� ����?
	/*
	if (status_ != ServerStatus::STARTED)
	{
		AfxMessageBox(_T("FUCK!"));
		return 0;
	}
	*/
	auto str = std::unique_ptr<std::string>(
		reinterpret_cast<std::string*>(wParam));

	int nIndex = logListCtrl_.InsertItem(0, _T("Echo"));
	logListCtrl_.SetItemText(nIndex, 1, CString(str->c_str()));
	logListCtrl_.SetItemText(nIndex, 2, _T("."));

	return 0;
}

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer