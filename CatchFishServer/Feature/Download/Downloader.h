#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_DOWNLOAD_DOWNLOADER_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_DOWNLOAD_DOWNLOADER_H__

#include <Windows.h>
#include <map>
#include <vector>
#include <set>
#include <Feature/Common/Common.h>
#include <ProcessLib/Lock/Lock.h>

class CDownloader : public libTools::CThreadLock
{
public:
	CDownloader();
	~CDownloader();

	static CDownloader& GetInstance();

	BOOL Refresh();

	BOOL GetDownLoadFile(_In_ CONST GameIdentity& Identifier, _Out_ std::vector<CatchFishFileContent>& Vec);

	BOOL IsExistUploadFileCRC(_In_ DWORD dwCRC32);

	VOID AddUploadFileCRC(_In_ DWORD dwCRC32);
private:
	BOOL RefreshDownFile();

	BOOL RefreshUploadFile();
private:
	// Format: L"3AÏµÁÐ-ÓÄÁéÇ±Í§" ... ...
	std::map<std::wstring, CatchFishFileStruct> _VecFileContent;

	//
	std::set<DWORD> _VecUploadFileCRC;

	//
	libTools::CThreadLock _UploadVecLock;
};



#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_DOWNLOAD_DOWNLOADER_H__
