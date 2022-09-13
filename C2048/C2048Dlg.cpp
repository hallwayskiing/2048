
// C2048Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "C2048.h"
#include "C2048Dlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CC2048Dlg 对话框



CC2048Dlg::CC2048Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_C2048_DIALOG, pParent)
	, gameMap(4, std::vector<int>(4))
	, iScore(0)
	, sScore("")
	, hTable()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CC2048Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, gameMap[0][0]);
	DDX_Text(pDX, IDC_EDIT10, gameMap[2][1]);
	DDX_Text(pDX, IDC_EDIT11, gameMap[2][2]);
	DDX_Text(pDX, IDC_EDIT12, gameMap[2][3]);
	DDX_Text(pDX, IDC_EDIT13, gameMap[3][0]);
	DDX_Text(pDX, IDC_EDIT14, gameMap[3][1]);
	DDX_Text(pDX, IDC_EDIT15, gameMap[3][2]);
	DDX_Text(pDX, IDC_EDIT16, gameMap[3][3]);
	DDX_Text(pDX, IDC_EDIT2, gameMap[0][1]);
	DDX_Text(pDX, IDC_EDIT3, gameMap[0][2]);
	DDX_Text(pDX, IDC_EDIT4, gameMap[0][3]);
	DDX_Text(pDX, IDC_EDIT5, gameMap[1][0]);
	DDX_Text(pDX, IDC_EDIT6, gameMap[1][1]);
	DDX_Text(pDX, IDC_EDIT7, gameMap[1][2]);
	DDX_Text(pDX, IDC_EDIT8, gameMap[1][3]);
	DDX_Text(pDX, IDC_EDIT9, gameMap[2][0]);
}

BEGIN_MESSAGE_MAP(CC2048Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SAVE, &CC2048Dlg::OnSave)
	ON_COMMAND(ID_LOAD, &CC2048Dlg::OnLoad)
	ON_COMMAND(ID_RESTART, &CC2048Dlg::OnRestart)
	ON_COMMAND(ID_AUTO, &CC2048Dlg::OnAuto)
END_MESSAGE_MAP()


// CC2048Dlg 消息处理程序

BOOL CC2048Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//添加状态栏，显示分数
	m_status.Create(this);
	UINT nID[] = { 10000 };
	m_status.SetIndicators(nID, 1);
	m_status.SetPaneInfo(0, 10000, SBPS_NORMAL, 100);
	RECT rect = { 0 };
	GetClientRect(&rect);
	//获取屏幕分辨率，适配状态栏
	int nScreenWidth, nScreenHeight;
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int statusBottom, statusWidth;
	switch (nScreenWidth)
	{
	
	case 2880:
		statusBottom = 80;
		statusWidth = 40;
		break;
	case 2560:
		statusBottom = 80;
		statusWidth = 40;
		break;		
	case 1920:
		statusBottom = 40;
		statusWidth = 20;
		break;
	default:
		statusBottom = 40;
		statusWidth = 20;
		break;
	}
	m_status.MoveWindow(0, rect.bottom-statusBottom, rect.right, statusWidth);
	m_status.SetPaneText(0, L"Score:0");

	//添加菜单栏
	CMenu menu;
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);

	//添加快捷键菜单
	hTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	
	//所有方格初始化为不可见
	for (int i = 0; i < 16; i++)
		GetDlgItem(1000 + i)->ShowWindow(SW_HIDE);

	//随机使两个方块变为可见并获得2和4的值
	while (1)
	{
		int init1, init2, x, y;
		srand(clock());
		init1 = rand() % 16 + 1000;
		srand(clock() + rand());
		init2 = rand() % 16 + 1000;
		if (init1 == init2)continue;//循环直到获得两个不同的随机值

		GetDlgItem(init1)->ShowWindow(SW_SHOW);
		x = (init1 - 1000) / 4, y = (init1 - 1000) % 4;
		gameMap[x][y] = 2;
		GetDlgItem(init2)->ShowWindow(SW_SHOW);
		x = (init2 - 1000) / 4, y = (init2 - 1000) % 4;
		gameMap[x][y] = 4;
		break;
	}
	UpdateData(false);

	return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CC2048Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CC2048Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CC2048Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CC2048Dlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::vector<std::vector<int> > preMap = gameMap;
	switch (nChar)
	{
	case 'W':
	case VK_UP:
	{
		bool bFlagOfRedo = false;
		do
		{
			bFlagOfRedo = false;
			//先移动，不合并
			for (int b = 0; b < 4; b++)
			{
				int pos = 0;
				for (int a = 0; a < 4; a++)
				{
					if (gameMap[a][b] != 0)
					{
						int temp = gameMap[a][b];
						gameMap[a][b] = 0;
						gameMap[pos][b] = temp;
						pos++;
					}
				}
			}

			//合并同类项
			for (int b = 0; b < 4; b++)
				for (int a = 0; a < 3; a++)
				{
					if (gameMap[a][b] != 0 && gameMap[a][b] == gameMap[a + 1][b])
					{
						gameMap[a][b] *= 2;
						iScore += gameMap[a][b];
						gameMap[a + 1][b] = 0;
						a = -1;
						bFlagOfRedo = true;
					}
				}
		} while (bFlagOfRedo);//合并过后要重新移动

		break;
	}
	case'S':
	case VK_DOWN:
	{
		bool bFlagOfRedo = false;
		do
		{
			bFlagOfRedo = false;
			for (int b = 0; b < 4; b++)
			{
				int pos = 3;
				for (int a = 3; a >= 0; a--)
				{
					if (gameMap[a][b] != 0)
					{
						int temp = gameMap[a][b];
						gameMap[a][b] = 0;
						gameMap[pos][b] = temp;
						pos--;
					}
				}
			}

			for (int b = 0; b < 4; b++)
				for (int a = 3; a > 0; a--)
				{
					if (gameMap[a][b] != 0 && gameMap[a][b] == gameMap[a - 1][b])
					{
						gameMap[a][b] *= 2;
						iScore += gameMap[a][b];
						gameMap[a - 1][b] = 0;
						a = 4;
						bFlagOfRedo = true;
					}
				}
		} while (bFlagOfRedo);
		break;
	}
	case 'D':
	case VK_RIGHT:
	{
		bool bFlagOfRedo = false;
		do
		{
			bFlagOfRedo = false;
			for (int b = 0; b < 4; b++)
			{
				int pos = 3;
				for (int a = 3; a >= 0; a--)
				{
					if (gameMap[b][a] != 0)
					{
						int temp = gameMap[b][a];
						gameMap[b][a] = 0;
						gameMap[b][pos] = temp;
						pos--;
					}
				}
			}

			for (int b = 0; b < 4; b++)
				for (int a = 3; a > 0; a--)
				{
					if (gameMap[b][a] != 0 && gameMap[b][a] == gameMap[b][a - 1])
					{
						gameMap[b][a] *= 2;
						iScore += gameMap[b][a];
						gameMap[b][a - 1] = 0;
						a = 4;
						bFlagOfRedo = true;
					}
				}
		} while (bFlagOfRedo);

		break;
	}
	case 'A':
	case VK_LEFT:
	{
		bool bFlagOfRedo = false;
		do
		{
			bFlagOfRedo = false;
			for (int b = 0; b < 4; b++)
			{
				int pos = 0;
				for (int a = 0; a < 4; a++)
				{
					if (gameMap[b][a] != 0)
					{
						int temp = gameMap[b][a];
						gameMap[b][a] = 0;
						gameMap[b][pos] = temp;
						pos++;
					}
				}
			}

			for (int b = 0; b < 4; b++)
				for (int a = 0; a < 3; a++)
				{
					if (gameMap[b][a] != 0 && gameMap[b][a] == gameMap[b][a + 1])
					{
						gameMap[b][a] *= 2;
						iScore += gameMap[b][a];
						gameMap[b][a + 1] = 0;
						a = -1;
						bFlagOfRedo = true;
					}
				}
		} while (bFlagOfRedo);

		break;
	}
	}

	if (preMap != gameMap)//如果没有发生变化则不生成随机方块
	{
		//随机生成2或4方块
		while (1)
		{
			int x = 0, y = 0;//随机生成位置
			srand((unsigned)time(0) + rand());
			x = rand() % 4;
			y = rand() % 4;
			if (gameMap[x][y] == 0)
			{
				gameMap[x][y] = x + y > 4 ? 4 : 2;
				break;
			}

			bool isEmpty = false;//判断是否继续生成
			for (int a = 0; a < 4; a++)
				for (int b = 0; b < 4; b++)
					if (gameMap[a][b] == 0)isEmpty = true;
			if (!isEmpty)break;//全满则不再生成
		}
	}

	//值为0的方格设置为不可见
	for (int a = 0; a < 4; a++)
		for (int b = 0; b < 4; b++)
			if (gameMap[a][b] == 0)GetDlgItem(a * 4 + b + 1000)->ShowWindow(SW_HIDE);
			else GetDlgItem(a * 4 + b + 1000)->ShowWindow(SW_SHOW);

	//更新状态栏
	sScore.Format(L"%d", iScore);
	m_status.SetPaneText(0, L"Score:"+sScore);

	UpdateData(false);
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CC2048Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (TranslateAccelerator(GetSafeHwnd(), hTable, pMsg))
	{
		return true;
	}

	return SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}


void CC2048Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnOK();
	CDialogEx::OnDestroy();
}


void CC2048Dlg::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	using namespace std;
	ofstream out("C:\\Windows\\Temp\\save_2048.dat",ios::trunc);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			out << gameMap[i][j]<<endl;
	out << iScore;
	MessageBox(L"Success!");
}


void CC2048Dlg::OnLoad()
{
	// TODO: 在此添加命令处理程序代码
	using namespace std;
	ifstream in("C:\\Windows\\Temp\\save_2048.dat", ios::in);
	if (!in.is_open())MessageBox(L"Not Saved Yet!");
	else
	{
		string readLine;

		//恢复map
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				getline(in, readLine);
				gameMap[i][j] = atoi(readLine.c_str());
			}

		//恢复分数
		getline(in, readLine);
		iScore = atoi(readLine.c_str());
		sScore = readLine.c_str();
		m_status.SetPaneText(0, L"Score:" + sScore);

		//恢复可见性
		for (int a = 0; a < 4; a++)
			for (int b = 0; b < 4; b++)
				if (gameMap[a][b] == 0)GetDlgItem(a * 4 + b + 1000)->ShowWindow(SW_HIDE);
				else GetDlgItem(a * 4 + b + 1000)->ShowWindow(SW_SHOW);

		UpdateData(false);
	}
}


void CC2048Dlg::OnRestart()
{
	// TODO: 在此添加命令处理程序代码
	iScore = 0;
	m_status.SetPaneText(0, L"Score:0");

	for (int i = 0; i < 16; i++)
		GetDlgItem(1000 + i)->ShowWindow(SW_HIDE);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			gameMap[i][j] = 0;
	
	while (1)
	{
		int init1, init2, x, y;
		srand(clock());
		init1 = rand() % 16 + 1000;
		srand(clock() + rand());
		init2 = rand() % 16 + 1000;
		if (init1 == init2)continue;//循环直到获得两个不同的随机值

		GetDlgItem(init1)->ShowWindow(SW_SHOW);
		x = (init1 - 1000) / 4, y = (init1 - 1000) % 4;
		gameMap[x][y] = 2;
		GetDlgItem(init2)->ShowWindow(SW_SHOW);
		x = (init2 - 1000) / 4, y = (init2 - 1000) % 4;
		gameMap[x][y] = 4;
		break;
	}
	UpdateData(false);
}


void CC2048Dlg::OnAuto()
{
	// TODO: 在此添加命令处理程序代码
	for (int i = 0; i < 100; i++)
	{
		srand(clock() + rand());
		int x = rand() % 4;
		switch (x)
		{
		case 0:
			SendMessage(WM_KEYDOWN, VK_UP);
			break;
		case 1:
			SendMessage(WM_KEYDOWN, VK_DOWN);
			break;
		case 2:
			SendMessage(WM_KEYDOWN, VK_LEFT);
			break;
		case 3:
			SendMessage(WM_KEYDOWN, VK_RIGHT);
			break;
		}
	}
}
