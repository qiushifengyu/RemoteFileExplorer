#pragma once

#include "resource.h"

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
