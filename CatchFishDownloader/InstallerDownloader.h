#ifndef __CATCHFISHINSTALLER_CATCHFISHDOWNLOADER_INSTALLERDOWNLOADER_H__
#define __CATCHFISHINSTALLER_CATCHFISHDOWNLOADER_INSTALLERDOWNLOADER_H__

#include "Windows.h"
#include <string>
#include <memory>
#include <CatchFishServer/Feature/Common/Common.h>

class CInstallerDownloader
{
public:
	CInstallerDownloader();
	~CInstallerDownloader();

	BOOL RunInstaller();
private:
	BOOL Initialize();

	VOID EchoError(_In_ CONST std::wstring& wsErrText) CONST;

	DWORD GetServerInstallerCRC32() CONST;

	DWORD GetLocalInstallerCRC32() CONST;

	BOOL IsDownloadInstaller() CONST;

	BOOL DownloadInstaller();

	BOOL CreateInstallerProc() CONST;

	BOOL WriteContactConfig() CONST;

	BOOL DownloadContactContent();
private:
	std::wstring			_wsInstallerPath;
	std::shared_ptr<CHAR>	_InstallerBuffer;
	UINT					_uInstallerFileSize;
	DWORD					_dwFileCRC32;
	ContactContent			_Contact;
};



#endif // !__CATCHFISHINSTALLER_CATCHFISHDOWNLOADER_INSTALLERDOWNLOADER_H__
