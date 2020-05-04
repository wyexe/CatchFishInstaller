#ifndef __CATCHFISHINSTALLER_GENERATECODEDLL_FEATURE_BASESERVERACTION_H__
#define __CATCHFISHINSTALLER_GENERATECODEDLL_FEATURE_BASESERVERACTION_H__

#include <CatchFishServer/Feature/Common/Common.h>
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>


class CBaseServerAction
{
public:
	CBaseServerAction() = default;
	~CBaseServerAction();

public:
	BOOL Run(_In_ CONST std::wstring& wsText);

	VOID Stop();
private:
	static DWORD WINAPI _KeepSocketLiveThread(LPVOID lpParam);
protected:
	HANDLE _hThread = NULL;
	BOOL   _Run = FALSE;
	CIocpClient		_OfficialReleaseIocpClient;
	CIocpClient		_OtherReleaseIocpClient;
};



#endif // !__CATCHFISHINSTALLER_GENERATECODEDLL_FEATURE_BASESERVERACTION_H__
