// FileTools.h
#pragma once
#include <windows.h>
#include <string.h>
#include <vector>
 
#ifdef UNICODE
#define fstring std::wstring
#else
#define fstring std::string
#endif // !UNICODE
 
/*********************************************************************
���ܣ��ļ��Ƿ���ڣ��ļ�/�ļ��У�
������filePath ·��
*********************************************************************/
bool IsFileExist(LPCTSTR filePath);
 
/*********************************************************************
���ܣ��Ƿ����ļ���
������filePath ·��
*********************************************************************/
bool IsDir(LPCTSTR filePath);
 
/*********************************************************************
���ܣ����path·�����ļ��У��µ������ļ����ƣ�������·����������׺��
������path ·��
*********************************************************************/
void GetFiles(LPCTSTR path, std::vector<fstring>& files);
 
/*********************************************************************
���ܣ�ɾ��Ŀ¼(��Ҫ��֤�ļ����ڵ��ļ�û�б�ռ��)
������pSrc ·��
*********************************************************************/
int DeleteDir(LPCTSTR pSrc);
 
/*********************************************************************
���ܣ�����Ŀ¼
������pSrc��ԭĿ¼��
pDes��Ŀ��Ŀ¼��
bOverLoad, ���Ŀ������Ƿ��滻(���Ϊtrue����ɾ��ԭ��Ŀ¼��С��ʹ��)
���أ�<0��ʧ��>0���ɹ�
*********************************************************************/
int CopyDir(LPCTSTR pSrc, LPCTSTR pDes, bool bOverLoad = true);