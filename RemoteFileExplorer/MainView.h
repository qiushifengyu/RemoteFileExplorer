
// MainView.h : CMainView Ŭ������ �������̽�
//


#pragma once


// CMainView â

class CMainView : public CWnd
{
    // �����Դϴ�.
public:
    CMainView();

    // Ư���Դϴ�.
public:

    // �۾��Դϴ�.
public:

    // �������Դϴ�.
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    // �����Դϴ�.
public:
    virtual ~CMainView();

    // ������ �޽��� �� �Լ�
protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

