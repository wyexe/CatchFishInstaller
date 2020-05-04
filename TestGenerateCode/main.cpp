#include <Windows.h>
#include <iostream>
#include <CharacterLib/Character.h>
#include <CatchFishServer/Feature/Common/Common.h>
#include <CatchFishServer/Feature/Common/GenrateCodeExport.h>
#include <CatchFishServer/Feature/Res/ResText.h>
#include <TimeLib/TimeRand.h>

#ifdef _DEBUG
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"SocketCommonLib_Debug.lib")
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"TimeLib_Debug.lib")
#else
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"SocketCommonLib.lib")
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"TimeLib.lib")
#endif // _DEBUG


BOOL GenerateTestCode(_In_ CONST std::wstring& wsSerialNumber)
{
	static type_vsf ExprtPtr = nullptr;
	if (ExprtPtr == nullptr)
	{
		HMODULE hmDLL = ::LoadLibraryW(libTools::CCharacter::MakeCurrentPath(L"\\_安装接口.dll").c_str());
		if (hmDLL == NULL)
		{
			std::wcout << (L"加载GenerateCode.dll失败!") << std::endl;
			return FALSE;
		}

		ExprtPtr = reinterpret_cast<type_vsf>(::GetProcAddress(hmDLL, "sf"));
		if (ExprtPtr == nullptr)
		{
			std::wcout << (L"获取导出函数失败!");
			return FALSE;
		}
	}

	
	type_v生成码 GenrateCodePtr = ExprtPtr(enum版本::em_常山赵子龙);
	if (GenrateCodePtr == nullptr)
	{
		//std::wcout << L"生成测试码不支持游戏[" << itm.Identifier.wsGameType.c_str() << L"-" << itm.Identifier.wsGameName.c_str() << L"]" << std::endl;
		return FALSE;
	}

	//std::wcout << L"正在生成游戏:[" << itm.Identifier.wsGameType.c_str() << L"-" << itm.Identifier.wsGameName.c_str() << L"] 的测试!" << std::endl;
	for (int i = 0;i <= 10; ++i)
	{
		type_OutInfo Content = { 0 };
		Content.n还原系统随机码 = -1;
		if (Content.n还原系统随机码 != -1)
		{
			std::wcout << L"随机码=" << Content.n还原系统随机码 << std::endl;
		}



		Content.n_shu = 0;
		Content.n_yin = 0; // 3 = KillScoreCode1 
		Content.n次数 = 1;
		std::wcout << L"赢1000币, 输0, 次数=1" << std::endl;


		//libTools::CCharacter::strcpy_my(Content.n_as加密id, libTools::CCharacter::UnicodeToASCII(wsSerialNumber).c_str());
		libTools::CCharacter::strcpy_my(Content.n_as加密id, libTools::CCharacter::UnicodeToASCII(wsSerialNumber).c_str());
		if (!GenrateCodePtr(Content))
		{
			std::wcout << L"生成测试码失败!" << std::endl;
			return FALSE;
		}

		std::wcout << L"次数码=" << libTools::CCharacter::ASCIIToUnicode(Content.n次数_as次数码).c_str() << L", 查询码=" << libTools::CCharacter::ASCIIToUnicode(Content.as取次码).c_str() << std::endl;
	}
	

	return TRUE;
}

std::wstring GenerateLaoBanEncodeSerailNumber(_In_ CONST std::wstring& wsSerialNumber)
{
	//return libTools::CCharacter::MakeFormatText(L"FTGC%04X%s", _pFishGameStructContent->dwSeriesSerialNumber, wsSerialNumber.c_str());
	std::wstring wsDiskSerialNumber;
	for (UINT i = 0; i < wsSerialNumber.size(); i += 2)
	{
		WCHAR wszText[3] = { 0 };
		wszText[0] = wsSerialNumber[i];
		wszText[1] = wsSerialNumber[i + 1];


		wsDiskSerialNumber += libTools::CCharacter::MakeFormatText(L"%02x", std::stoul(wszText, nullptr, 16) ^ 0xFF);
	}
	return libTools::CCharacter::MakeFormatText(L"FTGD%04X%s", 0x0305, wsDiskSerialNumber.c_str());
}

int main()
{
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();
	//std::wstring wsSerialNumber = L"AA180111000003084709"; // 376a09eab4089978
	std::wstring wsSerialNumber = GenerateLaoBanEncodeSerailNumber(L"376a09eab4089978"); // 


	std::wcout << L"Disk Encrypt SerialNumber:";
	//std::wcin >> wsSerialNumber;
	GenerateTestCode(wsSerialNumber);
	system("pause");
	return 0;
}