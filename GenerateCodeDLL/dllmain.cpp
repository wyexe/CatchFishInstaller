#include <Windows.h>
#include <set>
#include <filesystem>
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <FileLib/File.h>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <AlgroithmLib/Encrypt/RC4.h>
#include "Feature/LocalAction.h"

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG


BOOL WINAPI DllMain(_In_ HINSTANCE , _In_ DWORD dwResaon, _In_ LPVOID )
{
	if (dwResaon == DLL_PROCESS_DETACH)
	{
		CServerAction::GetInstance().Stop();
	}
	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPIV Initialize(_In_ LPCWSTR pwszText)
{
	libTools::CLog::GetInstance().SetClientName(L"GenerateCodeDLL", libTools::CCharacter::MakeCurrentPath(L"\\"));
	return CLocalAction::GetInstance().Refresh(pwszText);
}

extern "C" __declspec(dllexport) void WINAPIV GetVecFishInfo(_Out_ LPWSTR pwszText)
{
	// Format: 
	// FishType,FishName1,0,FishName2,1,FishName3,1
	std::set<std::wstring> VecUniqueFishType;
	auto& Vec = CLocalAction::GetInstance().GetVecIdentify();
	for (auto& itm : Vec)
		VecUniqueFishType.insert(itm.Idenfier.wsGameType);


	std::wstring wsResult;
	for (auto& FishTypeName : VecUniqueFishType)
	{
		std::wstring wsText;
		for (auto& itm : Vec)
		{
			if (itm.Idenfier.wsGameType == FishTypeName)
			{
				if (!wsText.empty())
					wsText += L",";


				wsText += itm.Idenfier.wsGameName;
				wsText += L",";
				wsText += itm.ExistRandCode ? L"1" : L"0";
				wsText += L",";
				wsText += libTools::CCharacter::MakeFormatText(L"%04X", itm.dwIdentifierSerialNumber);
			}
		}


		if (!wsResult.empty())
			wsResult += L"\r\n";


		wsResult += FishTypeName;
		wsResult += L",";
		wsResult += wsText;
	}

	libTools::CCharacter::strcpy_my(pwszText, wsResult.c_str(), wsResult.length());
}

extern "C" __declspec(dllexport) void WINAPIV GetVecCodeType(_Out_ LPWSTR pwszText)
{
	std::wstring wsResult;
	auto& Vec = CLocalAction::GetInstance().GetVecCodeType();
	for (auto& itm : Vec)
	{
		if (!wsResult.empty())
			wsResult += L"\r\n";


		wsResult += itm;
	}

	libTools::CCharacter::strcpy_my(pwszText, wsResult.c_str(), wsResult.length());
}



static GenerateCodeContent g_GenerateCodeContent;
extern "C" __declspec(dllexport) void WINAPIV SetGenerateCodeParam(_In_ LPCWSTR pwszKeyName, _In_ LPCWSTR pwszKeyValue)
{
	std::wstring wsKeyName = pwszKeyName;
	if (wsKeyName == L"FishType")
		g_GenerateCodeContent.Identifier.wsGameType = pwszKeyValue;
	else if (wsKeyName == L"FishName")
		g_GenerateCodeContent.Identifier.wsGameName = pwszKeyValue;
	else if (wsKeyName == L"CodeType")
		g_GenerateCodeContent.wsCodeType = pwszKeyValue;
	else if (wsKeyName == L"OriginDiskSerialNumber")
		g_GenerateCodeContent.wsOriginDiskSerialNumber = pwszKeyValue;
	else if (wsKeyName == L"EncryptDiskSerialNumber")
		g_GenerateCodeContent.wsEncryptDiskSerialNumber = pwszKeyValue;
	else if (wsKeyName == L"WinScore")
		g_GenerateCodeContent.wsWinScore = pwszKeyValue;
	else if (wsKeyName == L"LoseScore")
		g_GenerateCodeContent.wsLoseScore = pwszKeyValue;
	else if (wsKeyName == L"CodeIndex")
		g_GenerateCodeContent.dwCodeIndex = _wtoi(pwszKeyValue);
	else if (wsKeyName == L"RandCode")
		g_GenerateCodeContent.dwRandCode = _wtoi(pwszKeyValue);
	else if (wsKeyName == L"IsHide")
		g_GenerateCodeContent.IsHideCode = _wtoi(pwszKeyValue);
	else
		::MessageBox(NULL, libTools::CCharacter::MakeFormatText(L"无效Key[%s]", pwszKeyName).c_str(), L"", NULL);

}

extern "C" __declspec(dllexport) int WINAPIV GenerateCode(_Out_ LPWSTR pwszText)
{
	std::wstring wsQueryCode, wsResult;
	if (!CServerAction::GetInstance().GenerateStandardCode(g_GenerateCodeContent, wsQueryCode, wsResult))
		return FALSE;

	wsprintfW(pwszText, L"%s\r\n%s", wsQueryCode.c_str(), wsResult.c_str());
	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPIV GenerateTestCode(_In_ LPCWSTR pwszGameType, _In_ LPCWSTR pwszGameName, _In_ LPCWSTR pwszOriginDiskSerailNumber, _Out_ LPWSTR pwszEncryptDiskSerailNumber, _Out_ LPWSTR pwszCode)
{
	std::wstring wsTestCode, wsCountCode, wsSerialNumber;
	GameIdentity Identifier;
	

	Identifier.wsGameName = pwszGameName;
	Identifier.wsGameType = pwszGameType;
	wsSerialNumber = pwszOriginDiskSerailNumber;
	if (!CServerAction::GetInstance().GetGenerateCode(Identifier, wsSerialNumber, wsCountCode, wsTestCode))
		return FALSE;


	wsprintfW(pwszCode, L"%s,%s",wsCountCode.c_str(), wsTestCode.c_str());
	libTools::CCharacter::strcpy_my(pwszEncryptDiskSerailNumber, wsSerialNumber.c_str());
	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPIV CreateChildGenerateCode(_In_ LPCWSTR EncryDiskSerialNumber, _In_ LPCWSTR FishTypeText, _In_ LPCWSTR FishNameText, _Out_ LPWSTR sbText)
{
	std::wstring	wsIdentityText;
	BOOL			bExistRandCode;



	SpecifyCustomerContent Content;
	Content.EncryptDiskSerialNumber = EncryDiskSerialNumber;
	Content.Identifier.wsGameType = FishTypeText;
	Content.Identifier.wsGameName = FishNameText;
	if (!CServerAction::GetInstance().CreateNewChildGenerateCode(Content, wsIdentityText, bExistRandCode))
		return FALSE;


	wsprintfW(sbText, L"%s\r\n%d", wsIdentityText.c_str(), bExistRandCode);
	return TRUE;
}


CatchFishStructContent g_CatchFishStructContent;
extern "C" __declspec(dllexport) void WINAPIV SetCreateNewFishGameParam(_In_ LPCWSTR pwszKeyName, _In_ LPCWSTR pwszKeyValue)
{
	std::wstring wsKeyName = pwszKeyName;
	if (wsKeyName == L"Series")
	{
		g_CatchFishStructContent.Release();
		g_CatchFishStructContent.Identifier.wsGameType = pwszKeyValue;
	}
	else if (wsKeyName == L"Name")
		g_CatchFishStructContent.Identifier.wsGameName = pwszKeyValue;
	else if (wsKeyName == L"SerialNumber")
		g_CatchFishStructContent.dwSeriesSerialNumber = std::stoi(std::wstring(pwszKeyValue), 0, 16);
	else if (wsKeyName == L"GameFullPath")
		g_CatchFishStructContent.wsFullPath = pwszKeyValue;
	else if (wsKeyName == L"GameCRC32")
		g_CatchFishStructContent.dwFileCRC32 = static_cast<DWORD>(std::stoll(std::wstring(pwszKeyValue), 0, 16));
	else if (wsKeyName == L"ReleaseDirectory")
		g_CatchFishStructContent.wsCopyDestPath = pwszKeyValue;
	else if (wsKeyName == L"IsExistRandCode")
		g_CatchFishStructContent.IsExistRestore = pwszKeyValue[0] == L'1' ? TRUE : FALSE;
	else if (wsKeyName == L"SpecifyInjectorPath")
		g_CatchFishStructContent.wsInjectorPath = pwszKeyValue;
	else if (wsKeyName == L"EnumValue")
		g_CatchFishStructContent.emVersion = static_cast<enum版本>(std::stoi(std::wstring(pwszKeyValue)));
	else if (wsKeyName == L"IdentifierDirectory")
		g_CatchFishStructContent.VecSupplementPath.push_back(pwszKeyValue);
	else if (wsKeyName == L"BinaryCode")
	{
		std::vector<std::wstring> Vec;
		libTools::CCharacter::Split(std::wstring(pwszKeyValue), L"-", Vec, libTools::CCharacter::em_Split_Option_KeepOnly_RemoveEmptyEntries);
		if (Vec.size() == 2)
		{
			g_CatchFishStructContent.VecCode.push_back(CatchFishBinaryCode{ static_cast<DWORD>(std::stoll(Vec.at(0),0,16)), static_cast<DWORD>(std::stoll(Vec.at(1),0,16)) });
		}
	}
}

extern "C" __declspec(dllexport) int WINAPIV CreateNewFishGame(_In_ LPCWSTR pwszFish0Path, _In_ LPCWSTR pwszFishHPath, _In_ int EnumValue)
{
	libTools::CSocketBuffer SocketBuffer(em_Sock_Msg_CreateNew_CatchFishFileContent);
	g_CatchFishStructContent.emVersion = static_cast<enum版本>(EnumValue);
	SocketBuffer.AddStruct(g_CatchFishStructContent);
	SocketBuffer << 2;


	LPVOID lpFish0FileContent = nullptr, lpFishHFileContent = nullptr;
	SIZE_T uFish0FileSize = 0, uFishHFileSize = 0;
	if (!libTools::CFile::ReadFileContent(pwszFish0Path, lpFish0FileContent, uFish0FileSize))
	{
		::MessageBoxW(NULL, L"读取Fish0.dll失败!", L"", NULL);
		return 0;
	}
	SetResDeleter(lpFish0FileContent, [](LPVOID& lpFileContent) { ::VirtualFree(lpFileContent, 0, MEM_RELEASE); });
	if (!libTools::CFile::ReadFileContent(pwszFishHPath, lpFishHFileContent, uFishHFileSize))
	{
		::MessageBoxW(NULL, L"读取Fish0.dll失败!", L"", NULL);
		return 0;
	}
	SetResDeleter(lpFishHFileContent, [](LPVOID& lpFileContent) { ::VirtualFree(lpFileContent, 0, MEM_RELEASE); });


	SocketBuffer << std::experimental::filesystem::v1::path(pwszFish0Path).filename().wstring() << uFish0FileSize << libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFish0FileContent), uFish0FileSize);
	SocketBuffer.SetDataPtr(reinterpret_cast<CONST CHAR*>(lpFish0FileContent), uFish0FileSize);


	SocketBuffer << std::experimental::filesystem::v1::path(pwszFishHPath).filename().wstring() << uFishHFileSize << libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFishHFileContent), uFishHFileSize);
	SocketBuffer.SetDataPtr(reinterpret_cast<CONST CHAR*>(lpFishHFileContent), uFishHFileSize);


	return CServerAction::GetInstance().CreateNewFishGame(SocketBuffer);
}

extern "C" __declspec(dllexport) int WINAPIV DecodeLogFile(_In_ LPCWSTR pwszPath)
{
	std::experimental::filesystem::v1::path TarPath(pwszPath);
	std::wstring TarDirectory = TarPath.parent_path().append("DecodeFile.txt").wstring();


	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;
	if (!libTools::CFile::ReadFileContent(std::wstring(pwszPath), lpFileContent, uFileSize))
	{
		::MessageBoxW(NULL, L"读取文化失败!", L"", NULL);
		return 0;
	}
	SetResDeleter(lpFileContent, [](LPVOID& p) { ::VirtualFree(p, 0, MEM_RELEASE); });


	std::string Rc4Key;
	for (int i = 0; i < 16; ++i)
		Rc4Key.push_back(reinterpret_cast<CONST CHAR*>(lpFileContent)[i]);

	
	std::shared_ptr<CHAR> KeyStream = libTools::CRC4(Rc4Key.c_str(), 16).GetKeyStream(uFileSize - 16);
	std::shared_ptr<CHAR> PlainText = libTools::CRC4_Decryption::DecryptText(reinterpret_cast<CONST CHAR*>(lpFileContent) + 16, uFileSize - 16, KeyStream.get());

	return libTools::CFile::WriteFile(TarDirectory, reinterpret_cast<CONST BYTE*>(PlainText.get()), uFileSize - 16);
}