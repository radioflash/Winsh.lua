// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "resource.h"
#include "WriteFrm.h"
#include "JHCNotifyIcon.h"
#include "JHCDropFiles.h"
#include "JHCCMenu.h"
#include "JHCEdit.h"
#include "JHCMultiPaneStatusBar.h"
#include "LuaLibIF.h"
#include "JHCMsgTrap.h"
#include "CLuaCommandEnv.h"

CRect PutWindow(int w = 400, int h = 400);

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CMsgTrap, public CIdleHandler,
		public CNotifyIconImpl<CMainFrame>, public CDropFiles<CMainFrame>,
		public CCMenu, public ILuaLibWinsh	
{
protected:

	CHorSplitterWindow m_wndSplitter;	//Child Windows and controls of this main window.
	JHCEdit m_wndTopEdit;
	JHCEdit m_wndBotEdit;
	JHCMultiPaneStatusBarCtrl m_status;
	CWriteFrameProxy* m_paras;
	CLua<ILuaLibWinsh>* m_lua;
	CLuaCommandEnv* m_cmd;

	int m_refMM;						//Index of table in registry, Lua Functions by index
	UINT m_repmode;						//The current message reporting mode
	UINT m_errmode;						//The current error reporting mode
	CString m_prompt;					//The interactive mode prompt.
	int uicount;						//The UI lifetime counter (IncUI/DecUI)
	BOOL m_ovr;							//TRUE for overwrite mode, FALSE for insert mode;
	CString m_file;						//Name of the current file.
	BOOL m_clr;							//TRUE to clear file prior to write.
	LONGLONG m_filepos;					//Position in file for read iterator.

	BOOL m_Lmenu;
	BOOL m_Rmenu;

	CString AppName;					//Application name shown in window title bars (Winsh.lua Scripting System)
	CString InitName;					//Name of the initialisation script executed on Lua reset (init)
	CString ResType;					//Type of Script Resources (GNT)
	CString LuaExt;						//Extension for script files (.GNT)
	CString ExePath;					//Full path to this exe file.
	CString ExeName;					//Name of this exe file (winsh).
	CString LibPath;					//Full path to library files (or empty).
	CString LastError;					//The last error message that was reported (or suppressed).

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CMainFrame() {uicount = 0;}

	~CMainFrame() {CloseLua();}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (m_wndTopEdit.m_hWnd == GetFocus())
		{
			// Capture the ENTER key for special processing in the top edit control:
			if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) return ExecuteImmediate(TRUE);
			// Typing character in overstrike mode - delete next character first:
			if (m_ovr && (pMsg->message == WM_CHAR) && (pMsg->wParam > 31)) m_wndTopEdit.DeleteChar();
		}
		return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		m_ovr = (GetKeyState(VK_INSERT) & 1) != 0;
		if (m_ovr)
			m_status.SetPaneText(IDR_STATUS_PANE3, CString("OVR"));
		else
			m_status.SetPaneText(IDR_STATUS_PANE3, CString("INS"));
		if ((GetKeyState(VK_CAPITAL) & 1) || (GetKeyState(VK_SHIFT) < 0))
			m_status.SetPaneText(IDR_STATUS_PANE4, CString("CAP"));
		else
			m_status.SetPaneText(IDR_STATUS_PANE4, CString(""));
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SCRIPT_EXIM, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_CUT, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_UNDO, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_DELETE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_SELECTALL, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EVALUATE, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_TRAP
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnMenuPopup)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_COPYDATA, OnRemoteCommand)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColor)
		MESSAGE_RANGE_HANDLER(0xC000, 0xFFFF, OnRegisteredMessage)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_DELETE, OnEditDelete)
		COMMAND_ID_HANDLER(ID_EDIT_SELECTALL, OnEditSelectAll)
		COMMAND_ID_HANDLER(ID_EDIT_CLEARALL, OnEditClearAll)
		COMMAND_ID_HANDLER(ID_EDIT_CLEARCUR, OnEditClearCurrent)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_SCRIPT_EXIM, OnScriptExIm)
		COMMAND_ID_HANDLER(ID_SCRIPT_ADDPROMPT, OnScriptAddPrompt)
		COMMAND_ID_HANDLER(ID_EVALUATE, OnScriptEvaluate)
		COMMAND_ID_HANDLER(ID_SCRIPT_RESET, OnScriptReset)
		COMMAND_ID_HANDLER(ID_CLOSE_REPORT, OnCloseReport)
		COMMAND_ID_HANDLER(ID_NOTIFY_EXIT, OnNotifyExit)
		COMMAND_ID_HANDLER(ID_HELP_LIST, OnHelpList)
		COMMAND_ID_HANDLER(ID_HELP, OnHelp)
		COMMAND_ID_HANDLER(ID_HELP_COMMANDLINE, OnHelpCommandLine)
		COMMAND_ID_HANDLER(ID_HELP_APPLICATIONNOTES, OnHelpAppNotes)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_RANGE_HANDLER(1, 32000, OnLuaCommand)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		CHAIN_MSG_MAP(CNotifyIconImpl<CMainFrame>)
		CHAIN_MSG_MAP(CDropFiles<CMainFrame>)
		CHAIN_MSG_MAP(CCMenu)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// Create and initialise the Report Window on its own thread:
		m_paras = new CWriteFrameProxy(this, true);
		m_paras->rect = PutWindow(600, 400);
		m_paras->visible = false;
		m_paras->Create();

		CreateSimpleStatusBar();

		m_status.SubclassWindow(m_hWndStatusBar);
		int arrPanes[] = {ID_DEFAULT_PANE, IDR_STATUS_PANE1, IDR_STATUS_PANE2, IDR_STATUS_PANE3, IDR_STATUS_PANE4};
		int arrWidths[] = {0, 50, 120, 30, 30};
		m_status.SetPanes(arrPanes, sizeof(arrPanes) / sizeof(int), false);
		// SetPaneWidths is a personal addition in atlctrlx.h (WTL):
		m_status.SetPaneWidths(arrWidths, sizeof(arrWidths) / sizeof(int));

		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		// create the splitter window
		m_wndSplitter.Create(*this, rcDefault, NULL, 0, WS_EX_CLIENTEDGE);

		// create the upper edit control
		DWORD dwEditStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL |WS_HSCROLL
			| ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE;
		m_wndTopEdit.Create(m_wndSplitter, rcDefault, NULL, dwEditStyle);
		m_wndTopEdit.SetFont((HFONT)::GetStockObject(SYSTEM_FIXED_FONT));
		m_wndTopEdit.SetMargins(10, 10);
		m_wndTopEdit.SetTabStops(16);
		m_wndTopEdit.SetLimitText(-1);

		// create the lower edit control
		dwEditStyle |= ES_READONLY;
		m_wndBotEdit.Create(m_wndSplitter, rcDefault, NULL, dwEditStyle);
		m_wndBotEdit.SetFont((HFONT)::GetStockObject(SYSTEM_FIXED_FONT));
		m_wndBotEdit.SetMargins(10, 10);
		m_wndBotEdit.SetTabStops(16);
		m_wndBotEdit.SetLimitText(-1);

		// set the splitter panes
		m_wndSplitter.SetSplitterPanes(m_wndTopEdit, m_wndBotEdit);

		// set the splitter as the client area window
		m_hWndClient = m_wndSplitter;
		UpdateLayout();
		m_wndSplitter.SetSplitterPos(100);

		// register to accept dropped files:
		AcceptDropFiles(true);

		OnIdle();

		// set information
		CString x;
		x.LoadString(IDS_FILE_EXT);
		x.MakeUpper(); x.TrimLeft(TEXT(" .")); x.TrimRight(TEXT(" "));
		LuaExt = CString(TEXT(".")) + CString(x);
		x.LoadString(IDS_SCRIPT_DEF);
		x.MakeUpper(); x.TrimLeft(TEXT(" ")); x.TrimRight(TEXT(" "));
		InitName = CString(x);
		x.LoadString(IDS_SCRIPT_RES);
		x.MakeUpper(); x.TrimLeft(TEXT(" ")); x.TrimRight(TEXT(" "));
		ResType = CString(x);


		// Get the full filepath of this exe file:
		CString c1("");
		GetModuleFileName(NULL, c1.GetBuffer(MAX_PATH), MAX_PATH);
		c1.ReleaseBuffer();
		c1.TrimLeft(_T(" \"")); c1.TrimRight(_T(" \""));

		// This block is a trick: If the exe is marked as a console app but started from Windows
		// it will be started with a console and the title of that console will be the module
		// filepath - we do not want this console unless we do writes in conio mode in which case
		// we will recreate it. If the exe is started from a pre-existing console the title will
		// be different and we do want to retain it.
		CString ct("");
		CString cp = CString(c1);
		cp.MakeUpper();
		DWORD cc = GetConsoleTitle(ct.GetBuffer(MAX_PATH), MAX_PATH);
		ct.ReleaseBuffer();
		if (cc > 0)
		{
			ct.MakeUpper(); ct.TrimLeft(_T(" \"")); ct.TrimRight(_T(" \""));
			if (cp == ct) FreeConsole();
		}

		// Derive the path to the exe file and the basic name (without the .exe extension):
		ExePath = c1;
		PathRemoveFileSpec(ExePath.LockBuffer()); ExePath.ReleaseBuffer();
		PathAddBackslash(ExePath.LockBuffer()); ExePath.ReleaseBuffer();
		ExePath.MakeUpper();
		ExeName = CString(PathFindFileName(c1.LockBuffer())); c1.ReleaseBuffer();
		PathRemoveExtension(ExeName.LockBuffer()); ExeName.ReleaseBuffer();

		// If Windows 7, set the UserModelID so task bar buttons are separate for different applications:
		FARPROC proc = GetProcAddress(GetModuleHandle(TEXT("Shell32.dll")), "SetCurrentProcessExplicitAppUserModelID");
		typedef HRESULT WINAPI scpeaumid(PCWSTR);
		if (proc != NULL)
		{
			x.LoadString(IDR_MAINFRAME);
			x += CString(".") += ExeName;
			x.MakeUpper();
			((scpeaumid*)proc)(x);
		}

		// initialise Lua scripting
		m_lua = new CLua<ILuaLibWinsh>(this, luaX_loads, luaX_preloads, luaX_postload);
		IncUI();
		OpenLua();
		DecUI();
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// Destroy the report window
		m_paras->Destroy();

		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// Close only hides this window - termination occurs when ALL UI elements
		// have closed and all scripts have completed.
		if (IsWindowVisible())
		{
			DecUI();
			ShowWindow(SW_HIDE);
		}
		return 0;
	}

	LRESULT OnColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if ((HWND)lParam == m_wndBotEdit.m_hWnd)
		{
			bHandled = TRUE;
			return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// The menu Exit option actually shuts this app down.
		DestroyWindow();
		return 0;
	}

	LRESULT OnCloseReport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DecUI();
		return 0;
	}

	LRESULT OnNotifyExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// Exit option on the Notify icon menu closes that icon:
		SetTaskIcon((HICON)INVALID_HANDLE_VALUE, false, false);
		return 0;
	}

	LRESULT OnMenuPopup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (m_notifymenu) return 0;
		int s; int e;
		SetCCMenuGutter(true);
		UIEnable(ID_EDIT_UNDO, m_wndTopEdit.CanUndo());
		if (m_wndSplitter.GetActivePane() == 0)
		{
			m_wndTopEdit.GetSel(s, e);
			UIEnable(ID_EDIT_PASTE, (::IsClipboardFormatAvailable(CF_TEXT) || ::IsClipboardFormatAvailable(CF_UNICODETEXT)));
			if (e > s)
			{
				UIEnable(ID_EDIT_CUT, TRUE);
				UIEnable(ID_EDIT_COPY, TRUE);
				UIEnable(ID_EDIT_DELETE, TRUE);
				UIEnable(ID_EVALUATE, TRUE);
			}
			else
			{
				UIEnable(ID_EDIT_CUT, FALSE);
				UIEnable(ID_EDIT_COPY, FALSE);
				UIEnable(ID_EDIT_DELETE, FALSE);
				UIEnable(ID_EVALUATE, FALSE);
			}
		}
		else
		{
			UIEnable(ID_EDIT_PASTE, FALSE);
			UIEnable(ID_EDIT_CUT, FALSE);
			UIEnable(ID_EDIT_DELETE, FALSE);
			m_wndBotEdit.GetSel(s, e);
			if (e > s)
			{
				UIEnable(ID_EDIT_COPY, TRUE);
				UIEnable(ID_EVALUATE, TRUE);
			}
			else
			{
				UIEnable(ID_EDIT_COPY, FALSE);
				UIEnable(ID_EVALUATE, FALSE);
			}
		}
		return 0;
	}

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CMenuHandle menu((HMENU)wParam);
		int ss = LOWORD(lParam);
		int se = HIWORD(lParam);
		menu.AppendMenu(MF_SEPARATOR);
		if (m_wndSplitter.GetActivePane() == 0)
			menu.AppendMenu(MF_ENABLED, ID_SCRIPT_EXIM, _T("Execute"));
		menu.AppendMenu((ss == se)? MF_GRAYED : MF_ENABLED, ID_EVALUATE, _T("Evaluate Selected"));
		menu.AppendMenu(MF_ENABLED, ID_EDIT_CLEARCUR, _T("Clear"));
		return 0;
	}

	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_wndSplitter.GetActivePane() == 0)
			m_wndTopEdit.Copy();
		else
			m_wndBotEdit.Copy();
		return 0;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_wndTopEdit.Cut();
		return 0;
	}

	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_wndTopEdit.Paste();
		return 0;
	}

	LRESULT OnEditDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_wndTopEdit.ReplaceSel(TEXT(""), TRUE);
		return 0;
	}

	LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_wndSplitter.GetActivePane() == 0)
			m_wndTopEdit.SetSelAll();
		else
			m_wndBotEdit.SetSelAll();
		return 0;
	}

	LRESULT OnEditClearAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearTop();
		ClearBot();
		return 0;
	}

	LRESULT OnEditClearCurrent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_wndSplitter.GetActivePane() == 0)
			ClearTop();
		else
			ClearBot();
		return 0;
	}

	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_wndTopEdit.Undo();
		return 0;
	}

	LRESULT OnScriptExIm(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ExecuteImmediate(FALSE);
		return 0;
	}

	LRESULT OnScriptEvaluate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString x; CString c;
		if (m_wndSplitter.GetActivePane() == 0)
			m_wndTopEdit.GetString(x);
		else
			m_wndBotEdit.GetString(x);
		AddCommand(CString("=") + x);
		c = m_cmd->GetCmdVar(CString("_EQUAL"), CString("return (!!)"));
		c.Replace(CString("!!"), x);
		if (c.GetLength() > 0) ExecLuaString(c, TRUE);
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_COMMAND, MAKEWPARAM(wParam, WM_TIMER), 0);
		return 0;
	}

	LRESULT OnScriptAddPrompt(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		AddPrompt();
		return 0;
	}

	LRESULT OnScriptReset(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearBot();
		m_status.SetPaneText(IDR_STATUS_PANE1, CString(""));
		CloseLua();
		OpenLua();
		return 0;
	}

	LRESULT OnLuaCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		lua_State* L = *m_lua;
		lua_checkstack(L, 5);
		int top = lua_gettop(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX ,m_refMM);
		lua_pushinteger(L, wID);
		lua_gettable(L, -2);
		if (!lua_isnumber(L, -1))
		{
			lua_pushinteger(L, wID);
			lua_pushinteger(L, wNotifyCode);
			lua_pushinteger(L, (int)hWndCtl);
			ExecChunk(3, CString("MessageHandler"));
		}
		lua_settop(L, top);
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearBot();
		WriteMessage(CString("    Winsh.lua Summary Help"));
		WriteMessage(CString("    ======================"));
		CString nm("onhelp");
		if (m_cmd->CheckCmdField(nm, LUA_TFUNCTION))
			ExecLuaString(nm + CString("()"), TRUE);
		else
			WriteMessage(CString("Help Topic not available."));
		m_wndBotEdit.SetSel(0, 0, FALSE);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearBot();
		WriteMessage(CString("    About Winsh.lua"));
		WriteMessage(CString("    ==============="));
		CString nm("onabout");
		if (m_cmd->CheckCmdField(nm, LUA_TFUNCTION))
			ExecLuaString(nm + CString("()"), TRUE);
		else
			WriteMessage(CString("Help Topic not available."));
		m_wndBotEdit.SetSel(0, 0, FALSE);
		return 0;
	}

	LRESULT OnHelpList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearBot();
		WriteMessage(CString("    Resources available in this Winsh.lua Build"));
		WriteMessage(CString("    ==========================================="));
		CString nm("oninventory");
		if (m_cmd->CheckCmdField(nm, LUA_TFUNCTION))
			ExecLuaString(nm + CString("()"), TRUE);
		else
			WriteMessage(CString("Help Topic not available."));
		m_wndBotEdit.SetSel(0, 0, FALSE);
		return 0;
	}

	LRESULT OnHelpCommandLine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ClearBot();
		WriteMessage(CString("    Winsh.lua Command-line Help"));
		WriteMessage(CString("    ==========================="));
		CString nm("oncommandline");
		if (m_cmd->CheckCmdField(nm, LUA_TFUNCTION))
			ExecLuaString(nm + CString("()"), TRUE);
		else
			WriteMessage(CString("Help Topic not available."));
		m_wndBotEdit.SetSel(0, 0, FALSE);
		return 0;
	}

	LRESULT OnHelpAppNotes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		lua_State* L = *m_lua;
		ClearBot();
		WriteMessage(CString("    Application Notes"));
		WriteMessage(CString("    ================="));
		lua_getglobal(L, "_APPNOTES");
		if (lua_isstring(L, -1))
			WriteMessage(CString(lua_tostring(L, -1)));
		else
			WriteMessage(CString("This message may be customised by setting the global string\r\n'_APPNOTES' in the application script."));
		m_wndBotEdit.SetSel(0, 0, FALSE);
		return 0;
	}

	void LoadFile(LPCTSTR name)
	{
		HANDLE h; BYTE buf; DWORD n;

		m_wndTopEdit.SetWindowText(TEXT(""));
		h = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE) return;
		while (::ReadFile(h, &buf, 1, &n, NULL) && (n == 1))
		{
			m_wndTopEdit.AppendText(CString(buf));
		}
		if (!(h == INVALID_HANDLE_VALUE)) CloseHandle(h);
	}

	bool OnFileDrop(UINT n)
	{
		LoadFile(GetDropFilePath(0));
		return true;
	}

	void ClearTop(void);
	void ClearBot(void);
	void ReportMem(void);
	void AddPrompt(void);
	void WriteOK(void) {m_status.SetPaneText(IDR_STATUS_PANE1, TEXT("OK"));}
	void SetRunning(void) {m_status.SetPaneText(IDR_STATUS_PANE1, TEXT("RUNNING"));}
	BOOL IsEmpty(void){return !(m_wndBotEdit.GetModify());}

	void CloseLua(void);
	void OpenLua(void);

	void IncUI(void);
	void DecUI(void);
	void OnNotifyRightclick();
	void OnNotifyLeftclick();
	void PrepareNotifyMenu(CMenuHandle m, int k);
	HICON GetMenuIcon(UINT uCmd);
	BOOL ExecuteImmediate(BOOL enter);
	void AddCommand(CString &s);
	void ExecLuaString(CString &s, BOOL cmd = FALSE);
	void SetLibraryPaths();
	void ExecCommandLine(CString& cmd, BOOL nodef = FALSE, BOOL nocmd = FALSE, BOOL nofile = FALSE);

	void WriteConsole(DWORD chan, LPCTSTR string);
	void WriteFile(CString& msg);
	static BOOL CALLBACK searcher(HWND hWnd, LPARAM lParam);
	CString ExclusionName(LPCTSTR GUID, UINT kind);
	LRESULT OnRegisteredMessage(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnRemoteCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);

#pragma region ILuaLib Library Interface

public:
	virtual CString& GetInitName() {return InitName;}
	virtual CString& GetResType()  {return ResType;}
	virtual CString& GetLuaExt()   {return LuaExt;}
	virtual CString& GetExePath()  {return ExePath;}
	virtual CString& GetExeName()  {return ExeName;}
	virtual CString& GetLastError(){return LastError;}
	virtual CString& GetAppName()  {return AppName;}
	virtual HWND GetHWND()         {return m_hWnd;}
	virtual void SetAppName(LPCTSTR s);
	virtual BOOL SetAppIcon(LPCTSTR icon);
	virtual BOOL Require(LPCTSTR name);
	virtual int GetRegistryTable(int key = LUA_NOREF, int ix = 0, const char* w = NULL);
	virtual UINT AllocLuaMessages(UINT num);
	virtual void SetTimerMessage(UINT code, UINT time = 0);
	virtual UINT CaptureMessage(UINT msg, msgtraphandler fMsg, WPARAM wfltr = 0) {return TrapMsg(msg, fMsg, wfltr, 0);}
	virtual void SetLuaMessageHandler(UINT code);
	virtual UINT FindFreeLuaMessage(UINT code, UINT num = 1);
	virtual void SetReportMode(UINT r, UINT e);
	virtual void CMainFrame::SetOverlay(LPCTSTR icon);
	virtual BOOL SetFile(LPCTSTR fn, BOOL clr = FALSE);
	virtual BOOL SetTaskIcon(HICON icon, bool lmenu, bool rmenu);
	virtual void Balloon(LPCTSTR mt, LPCTSTR ms, UINT to, DWORD ic);
	virtual void WriteMessage(CString& s, BOOL nl = TRUE);
	virtual void WriteError(CString& s);
	virtual void SetReportWindow(int act, int px, int py);
	virtual TCHAR ReadConsole();
	virtual LPCTSTR GetLine(BOOL start = FALSE);
	virtual BOOL Mutex(LPCTSTR key, UINT scope, LPCTSTR cmd);
	virtual int LoadScriptFile(LPCTSTR fn);
	virtual int LoadScriptResource(LPCTSTR fn);
	virtual HICON LoadIcon(LPCTSTR nm, int cx, int cy);
	virtual CCMenu* Menu(int from = 0);
	virtual int ExecChunk(int parms = 0, LPCTSTR context = _T(""));
	virtual int GetInventory(int type);

#pragma endregion

#pragma region CMsgTrap Interface

	virtual LRESULT OnMsgTrap(UINT id, UINT msg, LPARAM lp, void* vp)
	{
		BOOL b;
		return OnLuaCommand(msg, id, (HWND)lp, b);
	};

#pragma endregion

};
