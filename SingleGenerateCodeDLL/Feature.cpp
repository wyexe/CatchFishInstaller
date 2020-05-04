#include "Feature.h"
#include <LogLib/Log.h>

#define _SELF L"Feature.cpp"
CFeature& CFeature::GetInstance()
{
	static CFeature Instance;
	return Instance;
}

BOOL CFeature::GenerateStandardCode(_In_ CONST Tranfer_SpecifyCustomerContent& Content, _Out_ std::wstring& wsQueryCode, _Out_ std::wstring& wsCheatCode)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Generate_Specify_StandCode);
	SocketBuffer.AddStruct(Content);


	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [&](libTools::CSocketBuffer& Buffer)
	{
		Buffer >> wsCheatCode >> wsQueryCode;
	});
}

BOOL CFeature::QueryCountCode(_In_ CONST std::wstring& wsCountCode, _Out_ std::wstring& wsText)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Query_CountCode);
	SocketBuffer << wsCountCode;

	LOG_C_D(L"wsCountCode=%s", wsCountCode.c_str());
	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [&wsText](libTools::CSocketBuffer& Buffer) 
	{
		Buffer >> wsText;
	});
}
