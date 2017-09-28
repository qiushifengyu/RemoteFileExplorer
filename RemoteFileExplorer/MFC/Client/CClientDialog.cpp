// CClientDialog.cpp : ���� �����Դϴ�.
//

#include "MFC/stdafx.h"
#include "MFC/Client/CClientDialog.h"
#include <afxdialogex.h>

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

void CClientDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_REMOTE_IP, ipAddressCtrl_);
	DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, portEdit_);
	DDX_Control(pDX, IDC_MFCBUTTON_CLIENT_CONTROL, controlButton_);
	DDX_Control(pDX, IDC_EDIT_ECHO_STRING, echoStringEdit_);
	DDX_Control(pDX, IDC_MFCBUTTON_CLIENT_ECHO, echoButton_);
	DDX_Control(pDX, IDC_LIST_ECHO_RESULT, echoResultListBox_);
}

BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON_CLIENT_CONTROL,
		&CClientDialog::OnBnClickedMfcbuttonClientControl)
	ON_BN_CLICKED(IDC_MFCBUTTON_CLIENT_ECHO,
		&CClientDialog::OnBnClickedMfcbuttonClientEcho)
END_MESSAGE_MAP()

void CClientDialog::OnBnClickedMfcbuttonClientControl()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (status_ == ClientStatus::CONNECTED)
	{
		remoteFileExplorer_.Disconnect();
		controlButton_.SetWindowTextW(_T("Connect"));

		status_ = ClientStatus::DISCONNECTED;
	}
	else if (status_ == ClientStatus::DISCONNECTED)
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

		controlButton_.SetWindowTextW(_T("Disconnect"));

		status_ = ClientStatus::CONNECTED;
	}
}

void CClientDialog::OnBnClickedMfcbuttonClientEcho()
{
	if (status_ != ClientStatus::CONNECTED)
	{
		AfxMessageBox(_T("Not connected to a server :("));
		return;
	}

	CString cstr;
	echoStringEdit_.GetWindowTextW(cstr);

	std::string result =
		remoteFileExplorer_.Echo(static_cast<CStringA>(cstr).GetString());

	echoResultListBox_.AddString(CString(result.c_str()));
}

// CClientDialog �޽��� ó�����Դϴ�.

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer