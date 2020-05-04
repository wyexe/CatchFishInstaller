#include "ServerAction.h"
#include <LogLib/Log.h>
#include <ExceptionLib/Exception.h>


#define _SELF L"ServerAction.cpp"
CServerAction::~CServerAction()
{
	
}

CServerAction& CServerAction::GetInstance()
{
	static CServerAction Instance;
	return Instance;
}

BOOL CServerAction::GenerateStandardCode(_In_ CONST GenerateCodeContent& Content, _Out_ std::wstring& wsQueryCode, _Out_ std::wstring& wsCheatCode)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Generate_StandardCode);
	SocketBuffer.AddStruct(Content);


	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [&](libTools::CSocketBuffer& Buffer)
	{
		Buffer >> wsCheatCode >> wsQueryCode;
	});
}

BOOL CServerAction::GetFishList(_Out_ std::vector<GameIdentityContent>& VecIdentifyer)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Get_FishList);
	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [&VecIdentifyer](libTools::CSocketBuffer& Buffer)
	{
		UINT uSize = 0;
		Buffer >> uSize;
		for (UINT i = 0; i < uSize; ++i)
		{
			GameIdentityContent Identifier;
			Buffer >> Identifier.Idenfier.wsGameType >> Identifier.Idenfier.wsGameName >> reinterpret_cast<DWORD&>(Identifier.ExistRandCode) >> Identifier.dwIdentifierSerialNumber;
			VecIdentifyer.push_back(Identifier);
		}
	});
}


BOOL CServerAction::GetVecCodeType(_Out_ std::vector<std::wstring>& VecCodeType)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_Get_CodeTypeList);
	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [&VecCodeType](libTools::CSocketBuffer& Buffer)
	{
		UINT uSize = 0;
		Buffer >> uSize;
		for (UINT i = 0; i < uSize; ++i)
		{
			std::wstring wsText;
			Buffer >> wsText;
			VecCodeType.push_back(wsText);
		}
	});
}

BOOL CServerAction::GetGenerateCode(_In_ CONST GameIdentity& Identifier, _In_ std::wstring& wsDiskSerialNumber, _Out_ std::wstring& wsCountCode, _Out_ std::wstring& wsTestCode)
{
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Generate_TestCode);
	SendSocketBuffer << wsDiskSerialNumber << Identifier.wsGameType << Identifier.wsGameName;
	return _OfficialReleaseIocpClient.SyncSend(SendSocketBuffer, [&](libTools::CSocketBuffer& SocketBuffer)
	{
		SocketBuffer >> wsDiskSerialNumber >> wsTestCode >> wsCountCode;
	});
}

BOOL CServerAction::CreateNewChildGenerateCode(_In_ CONST SpecifyCustomerContent& Content, _Out_ std::wstring& IdentityText, _Out_ BOOL& bExistRandCode)
{
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_CreateNew_ChildSpecifyStandCode);
	SendSocketBuffer.AddStruct(Content);
	return _OfficialReleaseIocpClient.SyncSend(SendSocketBuffer, [&IdentityText, &bExistRandCode](libTools::CSocketBuffer& SocketBuffer)
	{
		SocketBuffer >> IdentityText >> reinterpret_cast<DWORD&>(bExistRandCode);
	});
}

BOOL CServerAction::CreateNewFishGame(_In_ libTools::CSocketBuffer& SocketBuffer)
{
	return _OfficialReleaseIocpClient.SyncSend(SocketBuffer, [](libTools::CSocketBuffer&) {});
}
