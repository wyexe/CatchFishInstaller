#include <Windows.h>
#include <CatchFishServer/Feature/Common/Common.h>
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>
#include "Feature.h"

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#endif // _DEBUG

#define _SELF L"dllmain.cpp"
BOOL WINAPI DllMain(_In_ HINSTANCE , _In_ DWORD fdwReason, _In_ LPVOID )
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{

	}
	return TRUE;
}


extern "C" __declspec(dllexport) int WINAPIV Initialize(_In_ LPCWSTR pwszText)
{
	libTools::CLog::GetInstance().SetClientName(L"SingleGenerateCodeDLL", libTools::CCharacter::MakeCurrentPath(L"\\"));
	return CFeature::GetInstance().Run(pwszText);
}
/*
static Tranfer_SpecifyCustomerContent g_Tranfer_SpecifyCustomerContent;
extern "C" __declspec(dllexport) void WINAPIV SetGenerateCodeParam(_In_ LPCWSTR pwszKeyName, _In_ LPCWSTR pwszKeyValue)
{
	std::wstring wsKeyName = pwszKeyName;
	if (wsKeyName == L"IdentifyText")
		g_Tranfer_SpecifyCustomerContent.IdentifyText = pwszKeyValue;
	else if (wsKeyName == L"WinScore")
		g_Tranfer_SpecifyCustomerContent.wsWinScore = pwszKeyValue;
	else if (wsKeyName == L"LoseScore")
		g_Tranfer_SpecifyCustomerContent.wsLoseScore = pwszKeyValue;
	else if (wsKeyName == L"CodeIndex")
		g_Tranfer_SpecifyCustomerContent.dwCodeIndex = _wtoi(pwszKeyValue);
	else if (wsKeyName == L"RandCode")
		g_Tranfer_SpecifyCustomerContent.dwRandCode = _wtoi(pwszKeyValue);
	else
		::MessageBox(NULL, libTools::CCharacter::MakeFormatText(L"无效Key[%s]", pwszKeyName).c_str(), L"", NULL);

}

extern "C" __declspec(dllexport) int WINAPIV GenerateCode(_Out_ LPWSTR pwszText)
{
	std::wstring wsQueryCode, wsResult;
	if (!CFeature::GetInstance().GenerateStandardCode(g_Tranfer_SpecifyCustomerContent, wsQueryCode, wsResult))
		return FALSE;


	wsprintfW(pwszText, L"%s\r\n%s", wsQueryCode.c_str(), wsResult.c_str());
	return TRUE;
}*/


extern "C" __declspec(dllexport) int WINAPIV QueryCountCode(_In_ LPCWSTR pwszCountCode, _Out_ LPWSTR pwszText)
{
	std::wstring wsText;
	if (!CFeature::GetInstance().QueryCountCode(pwszCountCode, wsText))
		return FALSE;

	LOG_C_D(L"wsText=%s", wsText.c_str());
	libTools::CCharacter::strcpy_my(pwszText, wsText.c_str());
	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPIV GenerateCode(_In_ LPCWSTR pwszText, _Out_ LPWSTR pwszWinCode)
{
	std::vector<std::wstring> Vec;
	libTools::CCharacter::Split(std::wstring(pwszText), L",", Vec, libTools::CCharacter::em_Split_Option_KeepOnly_RemoveEmptyEntries);
	if (Vec.size() != 4)
	{
		LOG_MSG_CF(L"pwszText=[%s]", pwszText);
		::MessageBoxW(NULL, L"无效的参数...", L"", NULL);
		return 0;
	}


	Tranfer_SpecifyCustomerContent Content;
	Content.IdentifyText = Vec.at(0);
	Content.wsWinScore = Vec.at(1);
	Content.wsLoseScore = Vec.at(2);
	Content.dwCodeIndex = std::stoul(Vec.at(3));
	Content.dwRandCode = 0xFFFFFFFF;

	std::wstring wsQueryCode, wsResult;
	if (!CFeature::GetInstance().GenerateStandardCode(Content, wsQueryCode, wsResult))
		return FALSE;


	wsprintfW(pwszWinCode, L"%s,%s", wsResult.c_str(), wsQueryCode.c_str());
	return TRUE;
}