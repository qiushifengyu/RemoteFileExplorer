
// MainView.cpp : CMainView Ŭ������ ����
//

#include "stdafx.h"
#include "RemoteFileExplorer.h"
#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainView

CMainView::CMainView()
{
}

CMainView::~CMainView()
{
}


BEGIN_MESSAGE_MAP(CMainView, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CMainView �޽��� ó����

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

    return TRUE;
}

void CMainView::OnPaint()
{
    CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

                       // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

                       // �׸��� �޽����� ���ؼ��� CWnd::OnPaint()�� ȣ������ ���ʽÿ�.
}

