
// RemoteFileExplorer.h : RemoteFileExplorer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CRemoteFileExplorerApp:
// �� Ŭ������ ������ ���ؼ��� RemoteFileExplorer.cpp�� �����Ͻʽÿ�.
//

class CRemoteFileExplorerApp : public CWinApp
{
public:
    CRemoteFileExplorerApp();


    // �������Դϴ�.
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    // �����Դϴ�.

public:
    UINT  m_nAppLook;
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CRemoteFileExplorerApp theApp;
