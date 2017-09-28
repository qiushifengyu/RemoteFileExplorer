
// RemoteFileExplorer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "MFC/stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFC/CRemoteFileExplorerApp.h"
#include "MFC/CMainFrame.h"

namespace remoteFileExplorer
{
namespace mfc
{

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteFileExplorerApp

BEGIN_MESSAGE_MAP(CRemoteFileExplorerApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CRemoteFileExplorerApp::OnAppAbout)
END_MESSAGE_MAP()


// CRemoteFileExplorerApp ����

CRemoteFileExplorerApp::CRemoteFileExplorerApp()
{
    // �ٽ� ���� ������ ����
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
    // ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
    //     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
    //     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    // TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
    // ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
    SetAppID(_T("RemoteFileExplorer.AppID.NoVersion"));

    // TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
    // InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CRemoteFileExplorerApp ��ü�Դϴ�.

CRemoteFileExplorerApp theApp;


// CRemoteFileExplorerApp �ʱ�ȭ

BOOL CRemoteFileExplorerApp::InitInstance()
{
    // ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
    // ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
    // InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
    // �� �׸��� �����Ͻʽÿ�.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();


    EnableTaskbarInteraction(FALSE);

    // RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
    // AfxInitRichEdit2();

    // ǥ�� �ʱ�ȭ
    // �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
    // �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
    // ��ƾ�� �����ؾ� �մϴ�.
    // �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
    // TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
    // ������ �������� �����ؾ� �մϴ�.
    SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));


    // �� â�� ����� ���� �� �ڵ忡���� �� ������ â ��ü��
    // ���� ���� �̸� ���� ���α׷��� �� â ��ü�� �����մϴ�.
    CMainFrame* pFrame = new CMainFrame;
    if (!pFrame)
        return FALSE;
    m_pMainWnd = pFrame;
    // �������� ����� ���ҽ��� �Բ� �ε��մϴ�.
    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);





    // â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();
    return TRUE;
}

int CRemoteFileExplorerApp::ExitInstance()
{
    //TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
    return CWinApp::ExitInstance();
}

// CRemoteFileExplorerApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

                                                        // �����Դϴ�.
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CRemoteFileExplorerApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CRemoteFileExplorerApp �޽��� ó����

} // namespace mfc
} // namespace remoteFileExplorer