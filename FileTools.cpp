// FileTools.cpp
#pragma once
//#include "stdafx.h"
 
#include "FileTools.h"
#include <atlconv.h>
//#include <corecrt_io.h>

#include "duilib.h"

#include <tchar.h>
#include <string>

int tmpNo = 0;

bool IsFileExist(LPCTSTR filePath)
{
	DWORD ftyp = GetFileAttributes(filePath);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
	{
		// �ļ�/�ļ��в�����
		return false;
	}
 
	return true;
}
 
bool IsDir(LPCTSTR filePath)
{
	DWORD ftyp = GetFileAttributes(filePath);
 
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
	{
		// ����һ���ļ���
		return true;
	}
 
	return false;
}
 
void GetFiles(LPCTSTR path, std::vector<fstring>& files)
{
	HANDLE hFile = 0;
	WIN32_FIND_DATA fileInfo;
	memset(&fileInfo, 0, sizeof(LPWIN32_FIND_DATA));
 
	fstring wsTemp(path);
	if (_T("\\") != wsTemp.substr(wsTemp.length() - 2))
	{
		wsTemp.append(_T("\\"));
	}
	wsTemp.append(_T("*"));
 
	hFile = FindFirstFile(wsTemp.c_str(), &fileInfo);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
 
	do
	{
		//����ǵ�ǰĿ¼�������ϼ�Ŀ¼����ֱ�ӽ�����һ��ѭ��  
		if ('.' == fileInfo.cFileName[0])
		{
			continue;
		}
		
		files.push_back(fileInfo.cFileName);
 
	} while (FindNextFile(hFile, &fileInfo));
 
	FindClose(hFile);
}
#include <atlstr.h>
//int retVal=0;
//����0��������������1������������-1ʧ��
int DeleteDir(LPCTSTR pSrc)
{
	int retVal=0;
	//RemoveDirectory
	if (NULL == pSrc)
	{
		return -1;
	}
 
	if (!IsFileExist(pSrc))
	{
		// Դ�ļ�������
		return -1;
	}
 
	if (!IsDir(pSrc))
	{
		int ret = DeleteFile(pSrc);
		if(!ret)
		{
			CString srcStr(pSrc);
			srcStr = srcStr.Left(srcStr.ReverseFind(_T('\\')) + 1);
			srcStr += _T("tmp");
			CString tmp;
			_itow(tmpNo++,tmp.GetBuffer(),10);
			tmp.ReleaseBuffer();
			srcStr += tmp;
			ret = MoveFile(pSrc,srcStr.GetBuffer(0));
			srcStr.ReleaseBuffer();
			if(!ret){
				DWORD errNo = GetLastError();
				return -1;
			}
			retVal = 1;
		}
	}
 
	std::vector<fstring> files;
	GetFiles(pSrc, files);
 
	for (std::vector<fstring>::iterator i = files.begin();
		i != files.end();
		++i)
	{
		// Դλ��
		fstring sTemp(pSrc);
		sTemp.append(_T("\\")).append(*i);
		LPCTSTR srcName = sTemp.c_str();
 
		if (IsDir(srcName))
		{
			DeleteDir(srcName);
		}
		else
		{
			int ret = DeleteFile(srcName);
			if(!ret)
			{
				CString srcStr(srcName);
				srcStr = srcStr.Left(srcStr.ReverseFind(_T('\\')) + 1);
				srcStr += _T("tmp");
				CString tmp;
				_itow(tmpNo++,tmp.GetBuffer(),10);
				tmp.ReleaseBuffer();
				srcStr += tmp;
				ret = MoveFile(srcName,srcStr.GetBuffer(0));
				srcStr.ReleaseBuffer();
				if(!ret){
					DWORD errNo = GetLastError();
					return -1;
				}
				retVal = 1;
			}
		}
	}
 
	RemoveDirectory(pSrc);
	return retVal;
}

//����0��������������1������������-1ʧ��
int CopyDir(LPCTSTR pSrc, LPCTSTR pDes, bool bOverLoad/* = true*/)
{
	int flag = 0;
	if (NULL == pSrc || NULL == pDes)
	{
		return -1;
	}

//#ifdef myDebug
//	CString debugtext;
//	debugtext.Format(_T("��ʼ�����ļ�1"));
//	m_pRich_text->AppendText(debugtext);
//	Sleep(10);
//#endif // myDebug

	if (!IsFileExist(pSrc))
	{
		// Դ�ļ�������
		return -1;
	}



	if (!IsDir(pSrc))
	{
		if (IsFileExist(pDes))
		{
			if (bOverLoad)
			{
				// Ŀ���ļ�����, ɾ���ļ�
				int ret=DeleteDir(pDes);
				if(ret==-1)
				{
					return -1;
				}
				else if(ret==1)
				{
					flag = 1;
				}
				ret=CopyFile(pSrc, pDes, false);
				if(ret==-1)
				{
					return -1;
				}
			}
		}
		else
		{
			int ret = CopyFile(pSrc, pDes, false);
			if(ret==-1)
			{
				return -1;
			}
		}
 
		// �����ļ�����
		//SetFileAttributes(pDes, FILE_ATTRIBUTE_HIDDEN);
		return flag;
	}
 
	if (IsFileExist(pDes))
	{
		//if (bOverLoad)
		//{
		//	// Ŀ���ļ�����, ɾ���ļ���
		//	DeleteDir(pDes);
		//	CreateDirectory(pDes, NULL);
		//}
	}
	else
	{
		int ret = CreateDirectory(pDes, NULL);
		if(ret==-1)
		{
			return -1;
		}
	}
 
	std::vector<fstring> files;
	GetFiles(pSrc, files);
 
	for (std::vector<fstring>::iterator i = files.begin();
		i != files.end();
		++i)
	{
		
		// Դλ��
		fstring sTemp(pSrc);
		sTemp.append(_T("\\")).append(*i);
		LPCTSTR srcName = sTemp.c_str();
 
		// Ŀ��λ��
		fstring sTemp1(pDes);
		sTemp1.append(_T("\\")).append(*i);
		LPCTSTR dstName = sTemp1.c_str();
		if (IsDir(srcName))
		{
			int ret=CopyDir(srcName, dstName);
			if(ret==-1)
			{
				return -1;
			}
			else if(ret==1)
			{
				flag = 1;
			}
		}
		else
		{
			if (IsFileExist(dstName))
			{
				int ret=DeleteDir(dstName);
				if(ret==-1)
				{
					return -1;
				}
				else if(ret==1)
				{
					flag = 1;
				}
			}
 
			CopyFile(srcName, dstName, false);
		}
 
		// �����ļ�����
		//SetFileAttributes(dstName, FILE_ATTRIBUTE_HIDDEN);
	}
 
	return flag;
}
