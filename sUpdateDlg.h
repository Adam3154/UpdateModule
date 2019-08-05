#pragma once
#include <set>

class sUpdateDlg :
	public WindowImplBase
{
public:
	sUpdateDlg(CFrameWnd*);
	~sUpdateDlg();

	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();
	virtual void Notify(TNotifyUI &msg);

	static DWORD WINAPI checkSUpdateThread(LPVOID lpParam);
	static DWORD WINAPI SUpdateThread(LPVOID lpParam);

	int checkSoftUpdate();
	void setMyText(LPCTSTR);
	int SoftUpdate();
	CString GetAppVersion();
	int ThreadUnzip_OneVersion(CString& localDirectory, CString& strLocalFile);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CLabelUI* m_pLabel;
	CRichEditUI* m_pRich_text;
	CProgressUI* m_pProg;
	CButtonUI* m_pButUpdate;
	std::set<DWORD>* setdwMustVer;
	std::set<DWORD>* setdwNetVer;
	bool needRestart;
	CFrameWnd* pDlg;
private:
	//CDuiString m_strXMLPath;	
	
	CButtonUI* m_pButCancel;	
	CTextUI* m_pMyText;
};

