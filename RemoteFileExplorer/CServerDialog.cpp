// CServerDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CServerDialog.h"
#include "afxdialogex.h"


// CServerDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CServerDialog, CDialogEx)

CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER, pParent)
{

}

CServerDialog::~CServerDialog()
{
}

void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialogEx)
END_MESSAGE_MAP()


// CServerDialog �޽��� ó�����Դϴ�.
