#include "BaseServerAction.h"
#include <LogLib/Log.h>
#include <ExceptionLib/Exception.h>
#include <ProcessLib/Process/Process.h>

#define _SELF L"BaseServerAction.cpp"

CBaseServerAction::~CBaseServerAction()
{
	Stop();
}

BOOL CBaseServerAction::Run(_In_ CONST std::wstring& wsText)
{
	_OfficialReleaseIocpClient.SetDisConnectPtr([]
	{
		LOG_MSG_CF(L"与服务器断开连接!");
		::TerminateProcess(::GetCurrentProcess(), 0);
	});
	_OtherReleaseIocpClient.SetDisConnectPtr([]
	{
		LOG_MSG_CF(L"与服务器断开连接!");
		::TerminateProcess(::GetCurrentProcess(), 0);
	});
	libTools::g_EchoExceptionMsgPtr = [](CONST std::wstring& wsText) { LOG_MSG_CF(L"InvokeException:%s", wsText.c_str()); };


	BOOL bRetCode = FALSE;
	if (wsText == L"L")
		bRetCode = _OfficialReleaseIocpClient.Run(老板_SERVER_IP, SERVER_PORT, 5 * 1000);
	else if(wsText == L"H")
		bRetCode = _OfficialReleaseIocpClient.Run(SERVER_IP, SERVER_PORT, 5 * 1000);


	if (!bRetCode)
	{
		LOG_MSG_CF(L"Connect to Server Faild .");
		return FALSE;
	}
	/*if (!_OtherReleaseIocpClient.Run(OTHERSERVER_IP, OTHERSERVER_PORT, 3 * 1000))
	{
		LOG_MSG_CF(L"Connect to Server Faild ..");
		return FALSE;
	}*/


	_Run = TRUE;
	_hThread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)_KeepSocketLiveThread, this, NULL, NULL);
	return TRUE;
}

VOID CBaseServerAction::Stop()
{
	_Run = FALSE;
	if (_hThread != NULL)
	{
		::WaitForSingleObject(_hThread, INFINITE);
		::CloseHandle(_hThread);
		_hThread = NULL;


		_OfficialReleaseIocpClient.Stop();
		_OtherReleaseIocpClient.Stop();
	}
}

DWORD WINAPI CBaseServerAction::_KeepSocketLiveThread(LPVOID lpParam)
{
	CBaseServerAction* pServerAction = reinterpret_cast<CBaseServerAction *>(lpParam);
	while (pServerAction->_Run)
	{
		libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_KeepLive);
		pServerAction->_OfficialReleaseIocpClient.SyncSend(SocketBuffer, [](libTools::CSocketBuffer&) {});

		libTools::CSocketBuffer OtherSocketBuffer(em_Sock_Msg_OtherServer_KeepAlive);
		//pServerAction->_OtherReleaseIocpClient.SyncSend(OtherSocketBuffer, [](libTools::CSocketBuffer&) {});
		for (int i = 0; i < 30 && pServerAction->_Run; ++i)
			::Sleep(100);


	}
	return 0;
}
