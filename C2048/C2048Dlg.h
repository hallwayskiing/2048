
// C2048Dlg.h: 头文件
//

#pragma once
#include<vector>

// CC2048Dlg 对话框
class CC2048Dlg : public CDialogEx
{
// 构造
public:
	CC2048Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_C2048_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	std::vector<std::vector<int> >gameMap;
	CStatusBar m_status;
	HACCEL hTable;
	int iScore;
	CString sScore;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL CC2048Dlg::PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnRestart();
	afx_msg void OnAuto();
};
