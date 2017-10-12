
// MainFrame.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "MFC/CMainView.h"

namespace remoteFileExplorer
{
namespace mfc
{

class CMainFrame : public CFrameWnd
{

public:
    CMainFrame();
protected:
    DECLARE_DYNAMIC(CMainFrame)

    // Ư���Դϴ�.
public:

    // �۾��Դϴ�.
public:

    // �������Դϴ�.
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    // �����Դϴ�.
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
    CStatusBar statusBar_;
    CMainView* mainView_;

    // ������ �޽��� �� �Լ�
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()

};

} // namespace mfc
} // namespace remoteFileExplorer