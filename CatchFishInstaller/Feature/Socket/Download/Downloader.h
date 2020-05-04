#ifndef __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_DOWNLOAD_DOWNLOADER_H__
#define __CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_DOWNLOAD_DOWNLOADER_H__

#include <Windows.h>
#include <vector>
#include <CatchFishServer/Feature/Common/Common.h>

class CDownloader
{
public:
	CDownloader() = default;
	~CDownloader() = default;

	static CDownloader& GetInstance();

	BOOL Download_CheatFile(_In_ CONST GameIdentity& Identifier, _Out_ std::vector<CatchFishFileContent>& Vec);

	VOID Upload_InstallRecord(_In_ CONST InstallDiskRecord_Tranfer& Content);

	DWORD GetGenerateCodeCRC32();

	BOOL GetGenerateCode(_In_ CONST GameIdentity& Identifier, _In_ std::wstring& wsDiskSerialNumber, _Out_ std::wstring& wsCountCode, _Out_ std::wstring& wsTestCode);

	BOOL Download_GameIdentifier(std::vector<CatchFishStructContent>& Vec);

	BOOL Download_DumpTool(std::vector<CatchFishFileContent>& Vec);
private:

};



#endif // !__CATCHFISHINSTALL_CATCHFISHINSTALL_FEATURE_SOCKET_DOWNLOAD_DOWNLOADER_H__
