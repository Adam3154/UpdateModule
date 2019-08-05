
#include "FTPAPI.h"
#include "FileTools.h"
#include "duilib.h"
#include "FrameWnd.h"



sUpdateDlg::sUpdateDlg(CFrameWnd* dlg)
{
	pDlg = dlg;
}


sUpdateDlg::~sUpdateDlg()
{
}

CDuiString sUpdateDlg::GetSkinFile()
{
	return _T("sUpdateDlg.xml");
}

LPCTSTR sUpdateDlg::GetWindowClassName(void) const
{
	return _T("UIsUpdateDlg");
}

void sUpdateDlg::InitWindow()
{
	needRestart = false;
	setdwMustVer = new std::set<DWORD>;
	setdwNetVer = new std::set<DWORD>;
	m_pLabel= static_cast<CLabelUI*>(m_pm.FindControl(_T("caption")));
	m_pRich_text = static_cast<CRichEditUI*>(m_pm.FindControl(_T("msg_text")));
	m_pButUpdate = static_cast<CButtonUI*>(m_pm.FindControl(_T("updatebtn")));
	m_pButCancel = static_cast<CButtonUI*>(m_pm.FindControl(_T("cancelbtn")));
	m_pProg = static_cast<CProgressUI*>(m_pm.FindControl(_T("msg_pg")));
	m_pMyText = static_cast<CTextUI*>(m_pm.FindControl(_T("myText")));
	
	m_pButUpdate->SetEnabled(false);
	m_pMyText->SetText(_T("�������У����Ժ򡣡�"));

	//HANDLE  hcheck = (HANDLE)_beginthreadex(NULL, 0, checkSUpdateThread, this, 0, NULL);
	HANDLE hThread;
	DWORD dwThreadId;
	hThread = CreateThread(NULL	// Ĭ�ϰ�ȫ����
		, NULL		// Ĭ�϶�ջ��С
		, checkSUpdateThread // �߳���ڵ�ַ
		, this	//���ݸ��̺߳����Ĳ���
		, 0		// ָ���߳���������
		, &dwThreadId	//�߳�ID��
	);
	
}

LRESULT sUpdateDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = TRUE;
	switch (uMsg) {
	case WM_TIMER:		    lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	
	default:
		bHandled = FALSE;
	}
	return 0;
}


void sUpdateDlg::Notify(TNotifyUI & msg)
{
	if (msg.sType == L"click")
	{
		if (msg.pSender->GetName() == L"cancelbtn")
		{
			Close(IDCANCEL);
			return;
		}
		else if (msg.pSender->GetName() == L"updatebtn")
		{
			m_pButCancel->SetEnabled(false);
			SetTimer(m_hWnd, ID_TIMER_DLCANCELENABLE, 60000, NULL);
			m_pButUpdate->SetEnabled(false);
			//HANDLE  hcheck = (HANDLE)_beginthreadex(NULL, 0, SUpdateThread, this, 0, NULL);
			HANDLE hThread;
			DWORD dwThreadId;

			hThread = CreateThread(NULL	// Ĭ�ϰ�ȫ����
				, NULL		// Ĭ�϶�ջ��С
				, SUpdateThread // �߳���ڵ�ַ
				, this	//���ݸ��̺߳����Ĳ���
				, 0		// ָ���߳���������
				, &dwThreadId	//�߳�ID��
			);

		}
	}
}

//����1��ʾ���°汾������0��ʾ�ޣ�����-1����
int sUpdateDlg::checkSoftUpdate()
{
	//��ȡ����汾��
	CString cuVer = GetAppVersion();
	CString strAppver = cuVer;
	CString tmp;
	tmp = strAppver.Left(strAppver.Find('.'));
	strAppver = strAppver.Right(3);
	tmp += strAppver;
	pDlg->m_dwCurVer = (DWORD)_wtoi(tmp.GetBuffer(0));  //---------------------- b.���浱ǰ�汾��Ϣ,��ʽ:[2133]
	tmp.ReleaseBuffer();

	char* host = "39.108.72.130";
	int port = 21;
	char *user = "zdeps158";
	char *pwd = "JN105bQt";

#ifdef _DEBUG
	char *path = "UPA/UPDATE_D";
#else
	char *path = "UPA/UPDATE";
#endif

	char * list = NULL;
	int data_len;
	int ret;
	SOCKET c_sock = ftp_connect(host, port, user, pwd);
	if ((int)c_sock < 0) { 
		pDlg->MessageBoxW(_T("socket����ʧ��")); 
		return -1; 
	}
	ret = ftp_cwd(c_sock, path);
	if (ret != 0) { 
		ftp_quit(c_sock); 
		pDlg->MessageBoxW(_T("���ù���Ŀ¼ʧ��")); 
		return -1;
	}

	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString localDirectory(exeFullPath);
	localDirectory = localDirectory.Left(localDirectory.ReverseFind(_T('\\')) + 1);
	CStringA dlVerPath = localDirectory + _T("download\\version.ini");
	CString dlVerPathw = dlVerPath;
	//��������ļ�������ɾ��,�����Ƕϵ����̫�鷳
	if (GetFileAttributes(dlVerPathw) != -1) {
		//��ֹ�û��ϴ�ֱ��ȡ�����޷������رո��ļ���������ļ���ռ��
		//if (pDlg->m_file.m_hFile != CFile::hFileNull)pDlg->m_file.Close();
		if (DeleteFile(dlVerPathw) != TRUE) {
			//pDlg->SetTipWindowText(_T("ɾ�������ļ�ʧ��!"));
			ftp_quit(c_sock);
			pDlg->MessageBoxW(_T("ɾ�����������ļ�ʧ��"));
			return -1;
		}
	}
	int dlVerSize;
	ret = ftp_server2local(host, c_sock, "version.ini", dlVerPath.GetBuffer(), &dlVerSize);
	dlVerPath.ReleaseBuffer();
	if (ret != 0) { 
		CreateDirectory(localDirectory+_T("download\\"), NULL);
		ret = ftp_server2local(host, c_sock, "version.ini", dlVerPath.GetBuffer(), &dlVerSize);
		dlVerPath.ReleaseBuffer();
		if(ret!=0){
			pDlg->MessageBoxW(_T("���������ļ�ʧ��"));
			ftp_quit(c_sock);
			return -1;
		}
	}

	TCHAR strBuff[256];
	CString upVer;
	CString myHardSn = pDlg->m_pUPSlbhardsn->GetText();
	if (myHardSn.Find('Y') != -1) {//���԰�
		::GetPrivateProfileString(_T("version"), _T("beta version"), _T(""), strBuff, 256, dlVerPathw.GetBuffer());
		dlVerPathw.ReleaseBuffer();
	}
	else {
		::GetPrivateProfileString(_T("version"), _T("latest version"), _T(""), strBuff, 256, dlVerPathw.GetBuffer());
		dlVerPathw.ReleaseBuffer();
	}
	CString latVer = strBuff;
	tmp.Format(_T("%s"), strBuff);
	upVer = tmp.Left(tmp.Find('.'));
	tmp = tmp.Right(3);
	upVer += tmp;
	DWORD dwUpVer = (DWORD)_wtoi(upVer.GetBuffer());
	upVer.ReleaseBuffer();

	//m_dwCurVer = 2177;

	if (pDlg->m_dwCurVer == dwUpVer) {
		ftp_quit(c_sock);
		Close();
		pDlg->MessageBoxW(_T("�������°汾"));
		return 0;
	}

	setdwMustVer->clear();
	setdwMustVer->insert(dwUpVer);
	for (int i = 0; ; i++) {
		CString tmp;
		tmp.Format(_T("%d"), i);
		ret = ::GetPrivateProfileString(_T("mustVersion"), tmp, _T(""), strBuff, 256, dlVerPathw.GetBuffer());
		dlVerPathw.ReleaseBuffer();
		if (ret <= 0) {
			break;
		}
		tmp.Format(_T("%s"), strBuff);
		upVer = tmp.Left(tmp.Find('.'));
		tmp = tmp.Right(3);
		upVer += tmp;
		DWORD tmpDW = _ttoi(upVer);
		if (tmpDW > pDlg->m_dwCurVer && tmpDW < dwUpVer)
		{
			setdwMustVer->insert(tmpDW);
		}
	}
	setMyText(_T("���ڻ�ȡ��������,���Ժ򡣡�"));
	/*ftp_quit(c_sock);
	c_sock = ftp_connect(host, port, user, pwd);*/
#ifdef _DEBUG
	char *path1 = "../TXT_D";
#else
	char *path1 = "../TXT";
#endif
	ret = ftp_cwd(c_sock, path1);
	if (ret != 0) {
		ftp_quit(c_sock);
		pDlg->MessageBoxW(_T("���ù���Ŀ¼ʧ��"));
		return -1;
	}
	ret = ftp_list(host, c_sock, "", &list, &data_len);
	if (ret != 0) {
		ftp_quit(c_sock);
		pDlg->MessageBoxW(_T("��ȡ�ļ��б�ʧ��"));
		return -1;
	}

	string str(list);
	int pos;
	setdwNetVer->clear();
	while ((pos = str.find(".txt")) != string::npos)
	{
		string verstr = str.substr(pos - 4, 4);
		DWORD netVer = atoi(verstr.c_str());
		if (netVer > pDlg->m_dwCurVer && netVer <= dwUpVer)
		{
			setdwNetVer->insert(netVer);
		}
		str = str.substr(pos + 1);
	}
	m_pProg->SetMaxValue(setdwNetVer->size());
	int prog = 0;
	for (std::set<DWORD>::iterator it = setdwNetVer->begin(); it != setdwNetVer->end(); it++) {
		char szNewFile[MAX_PATH];
		itoa(*it, szNewFile, 10);
		CString filename = szNewFile;
		filename += _T(".txt");
		CString strLocalFile = localDirectory + _T("download\\");
		strLocalFile += filename;
		//5.��������ļ�������ɾ��,�����Ƕϵ����̫�鷳
		if (GetFileAttributes(strLocalFile) != -1) {
			//��ֹ�û��ϴ�ֱ��ȡ�����޷������رո��ļ���������ļ���ռ��
			//if (pDlg->m_file.m_hFile != CFile::hFileNull)pDlg->m_file.Close();

			if (DeleteFile(strLocalFile) != TRUE) {
				//pDlg->SetTipWindowText(_T("ɾ�������ļ�ʧ��!"));
				ftp_quit(c_sock);
				pDlg->MessageBoxW(_T("ɾ�����ظ�����־�ļ�ʧ��"));
				return -1;
			}
		}
		//����
		int filesize;
		char s[MAX_PATH], d[MAX_PATH];
		strncpy(s, (CStringA)filename, sizeof(s));
		strncpy(d, (CStringA)strLocalFile, sizeof(d));
		int ret = ftp_server2local(host, c_sock, s, d, &filesize);
		if (ret) {
			//printf("����ʧ��\n");
			CreateDirectory(localDirectory + _T("download\\"), NULL);
			ret = ftp_server2local(host, c_sock, s, d, &filesize);
			if (ret) {
				ftp_quit(c_sock);
				pDlg->MessageBoxW(_T("���ظ��¹���ʧ��"));
				return -1;
			}
		}
		FILE *fp = fopen((CStringA)strLocalFile, "rb");
		if (fp == NULL)//�򿪲������ɹ�
		{
			ftp_quit(c_sock);
			pDlg->MessageBoxW(_T("���ļ�ʧ��"));
			return -1;
		}
		char cchar[1024];
		/*wchar_t m_wchar[1024];
		size_t converted = 0;*/
		while (fgets(cchar, 1024, fp) != NULL) {
			wchar_t *m_wchar;
			int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
			m_wchar = new wchar_t[len + 1];
			MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
			m_wchar[len] = '\0';

			//m_psUpdateDlg->m_pRich_text->
			m_pRich_text->AppendText(m_wchar);
			Sleep(10);

			delete[] m_wchar;
		}
		fclose(fp);
		m_pRich_text->AppendText(_T("\n------------------------\n"));
		Sleep(10);
		m_pProg->SetValue(++prog);
	}
	CString tmpText = _T("��ǰ�汾��") + cuVer + _T("�����°汾��") + latVer + ("������������ť");
	//m_psUpdateDlg->setMyText(_T("��⵽�°汾������������ť"));
	setMyText(tmpText);
	m_pButUpdate->SetEnabled(true);
	ftp_quit(c_sock);
	
	//m_psUpdateDlg->Close();
	return 1;
}


DWORD __stdcall sUpdateDlg::checkSUpdateThread(LPVOID lpParam)
{
	sUpdateDlg* dlg = (sUpdateDlg*)lpParam;
	int ret = dlg->checkSoftUpdate();
	if (ret == -1) {
		dlg->Close();
		dlg->pDlg->MessageBoxW(_T("����ʧ�ܣ����Ժ�����"));
	}
	
	return 0;
}

void sUpdateDlg::setMyText(LPCTSTR newText)
{
	m_pMyText->SetText(newText);
	return;
}


DWORD __stdcall sUpdateDlg::SUpdateThread(LPVOID lpParam)
{
	sUpdateDlg *dlg = (sUpdateDlg*)lpParam;
	int ret = dlg->SoftUpdate();
	KillTimer(dlg->m_hWnd, ID_TIMER_DLPROCS);
	dlg->Close();
	if (ret == -1) {
		dlg->pDlg->MessageBoxW(_T("����ʧ�ܣ����Ժ�����"));
		//MessageBox(NULL, _T("����ʧ�ܣ����Ժ�����"), NULL, 0);
	}
	else {
		if (dlg->needRestart) {
			dlg->pDlg->MessageBoxW(_T("�����ɹ�������������"));
			//MessageBox(NULL, _T("�����ɹ�������������"), NULL, 0);
		}
		else {
			dlg->pDlg->MessageBoxW(_T("�����ɹ�"));
			//MessageBox(NULL, _T("�����ɹ�"), NULL, 0);
		}
	}
	return 0;
}

CString sUpdateDlg::GetAppVersion()
{
	CString strAppVer;
	CString str;
	TCHAR strBuff[256];
	CString path = (CString)CPaintManagerUI::GetInstancePath() + _T("config\\version.ini");
	//����汾
	::GetPrivateProfileString(_T("version"), _T("1"), _T(""), strBuff, 256, path); //�豸����
	strAppVer.Format(_T("%s"), strBuff);
	return strAppVer;
}

//����0��ʾ��-1��ʾ����ʧ��
int sUpdateDlg::SoftUpdate()
{
	////��ȡ����汾��
	//CString strAppver = GetAppVersion();
	//CString tmp;
	//tmp = strAppver.Left(strAppver.Find('.'));
	//strAppver = strAppver.Right(3);
	//tmp += strAppver;
	//m_dwCurVer = (DWORD)_wtoi(tmp.GetBuffer(0));  //---------------------- b.���浱ǰ�汾��Ϣ,��ʽ:[2133]
	//tmp.ReleaseBuffer();

	m_pMyText->SetText(_T("�������������������Ժ򡣡�"));
	SetTimer(m_hWnd, ID_TIMER_DLPROCS, 100,NULL);
	m_pProg->SetMaxValue(100);
	m_pProg->SetValue(0);

	char* host = "39.108.72.130";
	int port = 21;
	char *user = "zdeps158";
	char *pwd = "JN105bQt";

#ifdef _DEBUG
	char *path = "UPA/UPDATE_D";
#else
	char *path = "UPA/UPDATE";
#endif

	char * list = NULL;
	int data_len;
	int ret;
	SOCKET c_sock = ftp_connect(host, port, user, pwd);
	if ((int)c_sock < 0) { return -1; }
	ret = ftp_cwd(c_sock, path);
	if (ret != 0) { ftp_quit(c_sock); return -1; }

	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString localDirectory(exeFullPath);
	localDirectory = localDirectory.Left(localDirectory.ReverseFind(_T('\\')) + 1);
	//CStringA dlVerPath = localDirectory + _T("download\\version.ini");
	//CString dlVerPathw = dlVerPath;
	////��������ļ�������ɾ��,�����Ƕϵ����̫�鷳
	//if (GetFileAttributes(dlVerPathw) != -1) {
	//	//��ֹ�û��ϴ�ֱ��ȡ�����޷������رո��ļ���������ļ���ռ��
	//	//if (pDlg->m_file.m_hFile != CFile::hFileNull)pDlg->m_file.Close();
	//	if (DeleteFile(dlVerPathw) != TRUE) {
	//		//pDlg->SetTipWindowText(_T("ɾ�������ļ�ʧ��!"));
	//		ftp_quit(c_sock); return -1;
	//	}
	//}
	//int dlVerSize;
	//ret = ftp_server2local(host, c_sock, "version.ini", dlVerPath.GetBuffer(), &dlVerSize);
	//dlVerPath.ReleaseBuffer();
	//if (ret != 0) { ftp_quit(c_sock); return -1; }

	//TCHAR strBuff[256];
	//CString upVer;
	//CString myHardSn = GetLocalHardSn();
	//if (myHardSn.Find('Y') != -1) {//���԰�
	//	::GetPrivateProfileString(_T("version"), _T("beta version"), _T(""), strBuff, 256, dlVerPathw.GetBuffer());
	//	dlVerPathw.ReleaseBuffer();
	//}
	//else {
	//	::GetPrivateProfileString(_T("version"), _T("latest version"), _T(""), strBuff, 256, dlVerPathw.GetBuffer());
	//	dlVerPathw.ReleaseBuffer();
	//}
	//tmp.Format(_T("%s"), strBuff);
	//upVer = tmp.Left(tmp.Find('.'));
	//tmp = tmp.Right(3);
	//upVer += tmp;
	//DWORD dwUpVer = (DWORD)_wtoi(upVer.GetBuffer());
	//upVer.ReleaseBuffer();

	//if (m_dwCurVer == dwUpVer) {
	//	ftp_quit(c_sock);
	//	return 0;
	//}
	
	//int i = 0;
	for (std::set<DWORD>::iterator it = setdwMustVer->begin(); it != setdwMustVer->end(); it++) {
		CString upVer;
		upVer.Format(_T("%d"), *it);
		upVer += _T(".7z");
		CString strLocalFile = localDirectory + _T("download\\");
		strLocalFile += upVer;
		//5.��������ļ�������ɾ��,�����Ƕϵ����̫�鷳
		if (GetFileAttributes(strLocalFile) != -1) {
			//��ֹ�û��ϴ�ֱ��ȡ�����޷������رո��ļ���������ļ���ռ��
			//if (pDlg->m_file.m_hFile != CFile::hFileNull)pDlg->m_file.Close();
			if (DeleteFile(strLocalFile) != TRUE) {
				//pDlg->SetTipWindowText(_T("ɾ�������ļ�ʧ��!"));
				
				ftp_quit(c_sock); return -1;
			}
		}
		//����
		int filesize;
		char s[MAX_PATH], d[MAX_PATH];
		strncpy(s, (CStringA)upVer, sizeof(s));
		strncpy(d, (CStringA)strLocalFile, sizeof(d));
		ret = ftp_server2local(host, c_sock, s, d, &filesize);
		if (ret!=0) {
			pDlg->MessageBoxW(_T("����ʧ��"));
			CreateDirectory(localDirectory + _T("download\\"), NULL);
			ret = ftp_server2local(host, c_sock, s, d, &filesize);
			if (ret!=0) { pDlg->MessageBoxW(_T("����ʧ��")); ftp_quit(c_sock); return -1; }
		}
		//m_pProg->SetValue(++i);

#ifdef myDebug
		CString debugtext;
		_itow(filesize, debugtext.GetBuffer(), 10);
		debugtext.ReleaseBuffer();
		//debugtext.Format(_T("�ļ���СΪ%d\n", filesize));
		m_pRich_text->AppendText(debugtext);
		Sleep(10);
#endif // myDebug

		m_pMyText->SetText(_T("��װ�������С���"));
		int restartFlag;
		if ((restartFlag = ThreadUnzip_OneVersion(localDirectory, strLocalFile)) == -1) {//xxxx.7z
			if ((restartFlag = ThreadUnzip_OneVersion(localDirectory, strLocalFile)) == -1) {
				if ((restartFlag = ThreadUnzip_OneVersion(localDirectory, strLocalFile)) == -1) {
					pDlg->MessageBoxW(_T("��ѹʧ��"));
					return -1;
				}
			}
		}
		m_pMyText->SetText(_T("�ð汾�������"));
		if (restartFlag == 1) {
			needRestart = true;
		}
	}

	DeleteDir(localDirectory + _T("download\\"));
	Sleep(10);
	CreateDirectory(localDirectory + _T("download\\"), NULL);
	ftp_quit(c_sock);
	return 0;


	//ret = ftp_list(host,c_sock, "", &list, &data_len);
	//if(ret!=0){return -1;}
	//
	//string str(list);
	//int pos;
	//setdwNetVer->clear();
	//while((pos=str.find(".7z"))!=string::npos)
	//{
	//	string verstr = str.substr(pos-4,4);
	//	DWORD netVer = atoi(verstr.c_str());
	//	if(netVer>m_dwCurVer)
	//	{
	//		setdwNetVer->insert(netVer);
	//	}
	//	str = str.substr(pos+1);
	//}

	//if(setdwNetVer->size()==0){
	//	
	//	return 0;
	//}
	//
	//int restartFlag;
	//for(std::set<DWORD>::iterator it = setdwNetVer->begin(); it!=setdwNetVer->end(); it++){
	//	char szNewFile[MAX_PATH];
	//	itoa(*it,szNewFile,10);
	//	CString filename = szNewFile;
	//	filename += _T(".7z");
	//	CString strLocalFile = localDirectory + _T("download\\");
	//	strLocalFile += filename;
	//	//5.��������ļ�������ɾ��,�����Ƕϵ����̫�鷳
	//	if (GetFileAttributes(strLocalFile) != -1){
	//		//��ֹ�û��ϴ�ֱ��ȡ�����޷������رո��ļ���������ļ���ռ��
	//		//if (pDlg->m_file.m_hFile != CFile::hFileNull)pDlg->m_file.Close();

	//		if (DeleteFile(strLocalFile) != TRUE){
	//			//pDlg->SetTipWindowText(_T("ɾ�������ļ�ʧ��!"));
	//			return -1;
	//		}
	//	}
	//	//����
	//	int filesize;
	//	char s[MAX_PATH],d[MAX_PATH];
	//	strncpy(s,(CStringA)filename,sizeof(s));
	//	strncpy(d,(CStringA)strLocalFile,sizeof(d));
	//	int ret = ftp_server2local(host,c_sock, s, d, &filesize);
	//	if(ret){
	//		printf("����ʧ��\n");
	//		CreateDirectory(localDirectory + _T("download\\"), NULL);
	//		ret = ftp_server2local(host,c_sock, s, d, &filesize);
	//		if(ret){return -1;}
	//	}
	//	
	//	if ( (restartFlag=ThreadUnzip_OneVersion(localDirectory,strLocalFile)) == -1){
	//		if ((restartFlag=ThreadUnzip_OneVersion(localDirectory,strLocalFile)) == -1){
	//			if ((restartFlag=ThreadUnzip_OneVersion(localDirectory,strLocalFile)) == -1){
	//				return -1;
	//			}
	//		}
	//	}
	//}
	//DeleteDir(localDirectory + _T("download\\"));
	//Sleep(10);
	//CreateDirectory(localDirectory + _T("download\\"), NULL);
	//
	//return restartFlag;
}

//����0��ʾ��������������1��ʾ��Ҫ����������-1��ʾ��ѹʧ��
int sUpdateDlg::ThreadUnzip_OneVersion(CString& localDirectory, CString& strLocalFile)//xxxx.7z
{

#ifdef myDebug
	CString debugtext;
	debugtext.Format(_T("��ѹ�С���\n"));
	m_pRich_text->AppendText(debugtext);
	Sleep(10);
#endif // myDebug

	
	int retNo = 0;
	CString strCmdFile, strZipFile, strToFolder;
	CString str, tmp;
	/*tmp.Format("V%d.%03d������,���Ժ�...",dwVersion/1000,dwVersion%1000);
	SetTipWindowText(tmp);*/

	/*tmp.Format("download/%d.7z",dwVersion);*/
	strCmdFile = localDirectory + _T("unzip\\7z.exe");
	strZipFile = strLocalFile;
	strToFolder = localDirectory + _T("download\\tmp\\");;
	if (GetFileAttributes(strCmdFile) == -1) { return -1; }
	if (GetFileAttributes(strZipFile) == -1) { return -1; }
	if (GetFileAttributes(strToFolder) == -1) { 
		CreateDirectory(strToFolder, NULL); 
		//m_pMyText->SetText(_T("������ʱ�ļ���"));

#ifdef myDebug
		CString debugtext;
		debugtext.Format(_T("������ʱ�ļ���\n"));
		m_pRich_text->AppendText(debugtext);
		Sleep(10);
#endif // myDebug

	}

	TCHAR s1[200], s2[200], s3[200];
	//GetShortPathName(strCmdFile, s1, sizeof(s1));
	//GetShortPathName(strZipFile, s2, sizeof(s2));
	//GetShortPathName(strToFolder, s3, sizeof(s3));

	s1[0] = s2[0] = s3[0] = '"';
	wcscpy(s1 + 1, strCmdFile.GetBuffer());
	wcscpy(s2 + 1, strZipFile.GetBuffer());
	wcscpy(s3 + 1, strToFolder.GetBuffer());
	strCmdFile.ReleaseBuffer();
	strZipFile.ReleaseBuffer();
	strToFolder.ReleaseBuffer();
	size_t len;
	len = wcslen(s1);
	s1[len] = '"';
	s1[len + 1] = '\0';
	len = wcslen(s2);
	s2[len] = '"';
	s2[len + 1] = '\0';
	len = wcslen(s3);
	s3[len] = '"';
	s3[len + 1] = '\0';

	TCHAR szCmd[1000];
	_stprintf(szCmd, _T("%s x %s -y -o%s"), s1, s2, s3);

	bool bExecuted = false;
	PROCESS_INFORMATION  ProcessInfo;
	STARTUPINFO  StartupInfo;  //This  is  an  [in]  parameter  
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.cb = sizeof  StartupInfo;  //Only  compulsory  field 
	if (CreateProcess(NULL, szCmd,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		//m_pMyText->SetText(_T("��ѹ�ɹ�"));

#ifdef myDebug
		CString debugtext;
		debugtext.Format(_T("��ѹ�ɹ�\n"));
		m_pRich_text->AppendText(debugtext);
		Sleep(5000);
#endif // myDebug

		WaitForSingleObject(ProcessInfo.hProcess, 5 * 60 * 1000);  //INFINITE
		DWORD exitcode = 0;
		if (GetExitCodeProcess(ProcessInfo.hProcess, &exitcode)) {
			if (exitcode != 0) {
				return -1;
			}
#ifdef myDebug			
			CString exitCstr;
			_itot(exitcode, exitCstr.GetBuffer(), 10);
			exitCstr.ReleaseBuffer();
			pDlg->MessageBoxW(exitCstr);
#endif // myDebug
		}
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
		bExecuted = true;
	}
	if (!bExecuted) { return -1; }

	Sleep(10);
	CString strSource = localDirectory + _T("download\\tmp\\");
	CString strTarget = localDirectory;

	//CString strSourceMainExe = strSource + _T("Release\\UPA.exe");
	//CString strTargetMainExeTmp;
	//CString strTargetMainExe = strTarget + _T("Release\\");
	////if ((GetFileAttributes(strSourceMainExe) != -1) && //1.�����ʾ������Ҫ����(ע����ʾ������������),�����,���´�����ʱɾ��
	////	(!g_bMark_MainExeRenamed))                       //2.�������Ϊ�ٲŸ���,����ֱ�Ӹ���(���ñ������崦˵��)
	//{
	//	strTargetMainExeTmp = strTargetMainExe + _T("tmp.exe");
	//	strTargetMainExe += _T("UPA.exe");
	//	//CFile::Rename(strTargetMainExe,strTargetMainExeTmp);
	//	//g_bMark_MainExeRenamed = true;  //���λ,ֻΪ�˸������������ֽ�һ��,�ڶ���Ϊ��������
	//	//m_bNeedRestart = true;          //���λ,��Ҫ���������Ч
	//}

	retNo = CopyDir(strSource, strTarget);
	if (retNo == -1)
	{
		//if (GetFileAttributes(strSourceMainExe) != -1)
		//{
		//	CFile::Rename(strTargetMainExeTmp,strTargetMainExe); //�����������Ҫ����,���Ҹ���ʧ��Ҫ������������ָĻ���
		//}
		return -1;
	}
	
	//����(����)һ���汾��ɺ�,���������ļ�Ϊ��ǰ�汾,

	//tmp = strLocalFile.Right(strLocalFile.ReverseFind('\\'));����
	tmp = strLocalFile.Mid(strLocalFile.ReverseFind('\\') + 1);
	//tmp = strLocalFile.Right(7);//xxxx.7z
	tmp = tmp.Left(tmp.Find('.'));

	DWORD dwVersion = (DWORD)_ttoi(tmp.GetBuffer(0));  //---------------------- b.���浱ǰ�汾��Ϣ,��ʽ:[2133]
	tmp.ReleaseBuffer();
	str.Format(_T("%d.%03d"), dwVersion / 1000, dwVersion % 1000);
	//m_StaticCurVerTip.SetWindowText(str);
	tmp = localDirectory + _T("config\\version.ini");
	WritePrivateProfileString(_T("version"), _T("1"), str, tmp);

	DeleteDir(strSource);
	Sleep(10);
	CreateDirectory(strSource, NULL);

	return retNo;
}

LRESULT sUpdateDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == ID_TIMER_DLPROCS)
	{
		if (m_pProg->GetValue() == 100) {
			m_pProg->SetValue(0);
		}
		m_pProg->SetValue(m_pProg->GetValue() + 5);
	}
	else if (wParam == ID_TIMER_DLCANCELENABLE) {
		m_pButCancel->SetEnabled(true);
		KillTimer(m_hWnd, ID_TIMER_DLCANCELENABLE);
	}
	bHandled = TRUE;
	return 0;
}
