#include "StatisticsRecorder.h"
#include <LogLib/Log.h>
#include <CharacterLib/Character.h>
#include <Feature/Res/ResText.h>
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"DbManagerLib_Debug.lib")
#else
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"ProcessLib.lib")
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"DbManagerLib.lib")
#endif // _DEBUG

#define _SELF L"StatisticsRecorder.cpp"
CStatisticsRecorder::CStatisticsRecorder()
{
	
}

CStatisticsRecorder::~CStatisticsRecorder()
{
	this->StopAsyncThread();
}

CStatisticsRecorder& CStatisticsRecorder::GetInstance()
{
	static CStatisticsRecorder Instance;
	return Instance;
}

VOID CStatisticsRecorder::Initialize()
{
	_wsDns = L"CatchFishServer";
	_wsDbUser = L"GameUser";
	_wsDbPass = L"0x5F58A41C";

	
	std::wcout << L"Initialize Database [" << _wsDns.c_str() << L"]" << std::endl;
	this->RunAsyncThread();
}


DWORD CStatisticsRecorder::AddInstallRecord(_In_ CONST InstallDiskRecord_Tranfer& Content)
{
	std::wstring wsSQL = libTools::CCharacter::MakeFormatText(L"exec [AddInstallRecord] N'%s',N'%s',N'%s',N'%s',N'%s'",
		Content.Identifier.wsGameType.c_str(),
		Content.Identifier.wsGameName.c_str(),
		Content.wsClientIp.c_str(),
		Content.wsOriginDiskSerialNumber.c_str(),
		Content.wsEncryptDiskSerialNumber.c_str());



	std::wstring wsInstallID;
	return this->ExcuteSQL_SingleResult(wsSQL, wsInstallID) ? std::stoi(wsInstallID) : 0;
}

VOID CStatisticsRecorder::AddInstallDiskRecord(_In_ DWORD dwInstallId, _In_ CONST InstallDiskRecord_Tranfer& Content)
{
	for (auto& itm : Content.Vec)
	{
		std::wstring wsSQL = libTools::CCharacter::MakeFormatText(L"exec [InsertInstallDiskRecord] %d, %d, N'%s', %d, N'%s', N'%s', N'%s', N'%s', N'%s', N'%s'",
			dwInstallId, 
			itm.dwPhysicsDiskIndex,
			itm.wsDiskSerialNumber.c_str(), 
			itm.dwDiskSize, 
			itm.wsTestCode.c_str(), 
			itm.wsCountCode.c_str(), 
			itm.wsEncodeSerialNumber.c_str(),
			L"", 
			itm.wsLaoBanEncodeSerialNumber.c_str(),
			itm.wsOriginText.c_str());


		this->AsyncExcuteSQL(wsSQL);
	}
}

VOID CStatisticsRecorder::AddGenerateCodeRecord(_In_ CONST GenerateCodeContent& Content, _In_ CONST std::wstring& wsUser)
{
	// Add Record
	std::wstring wsSQL = libTools::CCharacter::MakeFormatText(L"exec [AddCodeRecord] N'%s',N'%s',N'%s',N'%s',%d,N'%s',N'%s',N'%s'",
		Content.wsEncryptDiskSerialNumber.c_str(),
		Content.wsOriginDiskSerialNumber.c_str(),
		Content.Identifier.wsGameType.c_str(),
		Content.Identifier.wsGameName.c_str(),
		Content.dwCodeIndex,
		Content.wsWinScore.c_str(),
		Content.wsLoseScore.c_str(),
		wsUser.c_str());


	this->AsyncExcuteSQL(wsSQL);
}

BOOL CStatisticsRecorder::ExcuteClientSQL(_In_ CONST std::wstring& wsSQL, _Out_ std::vector<Table>& VecTable)
{
	return ExcuteSQL(wsSQL, VecTable);
}