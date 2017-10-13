#pragma once

#include <afxwin.h>

#include "MFC/Server/CServerDialog.h"
#include "MFC/Client/CClientDialog.h"

namespace remoteFileExplorer
{
namespace mfc
{
///////////////////////////////////////////////////////////////////////////////
enum class ProgramMode
{
	MainMode,
	ServerMode,
	ClientMode
};

///////////////////////////////////////////////////////////////////////////////
class CMainView : public CFormView
{
	DECLARE_DYNCREATE(CMainView)

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINVIEW };
#endif

protected:
	CMainView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMainView();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonServer();
	afx_msg void OnBnClickedButtonClient();
	DECLARE_MESSAGE_MAP()

private:
	CButton serverButton_;
	CButton clientButton_;
	server::CServerDialog serverDialog_;
	client::CClientDialog clientDialog_;
	ProgramMode programMode_{ ProgramMode::MainMode };
};

} // namespace mfc
} // namespace remoteFileExplorer