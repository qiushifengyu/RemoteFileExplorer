#pragma once

#ifndef __AFXWIN_H__
#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "MFC/resource.h"       // �� ��ȣ�Դϴ�.

namespace remoteFileExplorer
{
namespace mfc
{
///////////////////////////////////////////////////////////////////////////////
class CRemoteFileExplorerApp : public CWinApp
{
public:
    CRemoteFileExplorerApp();

    virtual BOOL InitInstance() override;
    virtual int ExitInstance() override;

    UINT  m_nAppLook;

protected:
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CRemoteFileExplorerApp theApp;

} // namespace mfc
} // namespace remoteFileExplorer