// CClientDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CClientDialog.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
END_MESSAGE_MAP()


// CClientDialog �޽��� ó�����Դϴ�.
