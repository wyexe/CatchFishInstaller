#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_ECHO_ECHOPACKET_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_ECHO_ECHOPACKET_H__

#include <Windows.h>
#include <functional>
#include <SocketCommon/SocketBuffer.h>
#include <Feature/Res/ResText.h>
#include <SocketCommon/SocketRemoteClient.h>
#include <ProcessLib/Lock/Lock.h>
#include <queue>


class CEchoPacket
{
public:
	CEchoPacket() = default;
	~CEchoPacket() = default;
private:
	template<typename T>
	static T ExtractPacket(_In_ std::function<T()> Ptr)
	{
		return std::move(Ptr());
	}

	struct MailFormContent
	{
		HWND	hWnd;
		DWORD	dwPid;
	};

public:
	static BOOL KeepALive(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL DownloadFile(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL InstallRecord(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL SetInstallerVersion(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GenerateTestCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GenerateStandardCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL Generate_Specify_StandCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GetFishList(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GetCodeTypeList(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL DownloadIdentifier(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL ExcuteClientSQL(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL ReleaseNewFishGame(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL ReleaseNewChildSpecifyStandCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GetContactContent(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL UploadGameFileHead(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL UploadGameFileContent(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL IsExistGameFile(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL GetVecIdentifier(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static BOOL QueryCountCode(_In_ libTools::CSocketRemoteClient* pRemoteClient, _In_ libTools::CSocketBuffer* pSocketBuffer);

	static VOID Initialize();
private:
	static std::wstring GenerateEncodeSerailNumber(_In_ CONST std::wstring& wsSerialNumber, _In_ DWORD dwSeriesSerialNumber);
private:
	static BOOL CALLBACK EnumSetWinName(HWND hWnd, LPARAM lpParam);

	static DWORD WINAPI _MailWorker(LPVOID);

	static VOID AddTextToVecMail(_In_ CONST std::wstring& wsText);
private:
	static std::queue<std::wstring> _VecMail;
	static libTools::CThreadLock	 _LockVecMail;
};




#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_SOCKET_ECHO_ECHOPACKET_H__
