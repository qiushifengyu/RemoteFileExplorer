#pragma once

#include "resource.h"

// CClientDialog ��ȭ �����Դϴ�.

class CClientDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CClientDialog)

public:
	CClientDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CClientDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
