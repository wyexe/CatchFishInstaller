#include "LocalConfig.h"
#include <CharacterLib/Character.h>
#include <FileLib/File.h>
#include <LogLib/Log.h>

#define _SELF L"LocalConfig.cpp"

std::wstring CLocalConfig::GetConfigValue(_In_ CONST std::wstring& wsKeyName, _In_ CONST std::wstring& wsConfigName)
{
	auto wsPath = libTools::CCharacter::MakeCurrentPath(L"\\Config.ini");
	if (!libTools::CFile::FileExist(wsPath))
	{
		LOG_MSG_CF(L"Config.ini 文件不存在!");
		return L"";
	}



	WCHAR wszText[1024] = { 0 };
	const BOOL bRetCode = ::GetPrivateProfileStringW(wsKeyName.c_str(), wsConfigName.c_str(), L"Error", wszText, _countof(wszText) - 1, wsPath.c_str());
	if (!bRetCode || std::wstring(wszText) == L"Error")
	{
		LOG_MSG_CF(L"读取Config[%s] 失败!", wsKeyName.c_str());
		return L"";
	}


	return wszText;
}
