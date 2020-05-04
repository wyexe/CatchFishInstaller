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
		HMODULE hmDLL = ::LoadLibraryW(libTools::CCharacter::MakeCurrentPath(L"\\_��װ�ӿ�.dll").c_str());
		if (hmDLL == NULL)
		{
			std::wcout << (L"����GenerateCode.dllʧ��!") << std::endl;
			return FALSE;
		}

		ExprtPtr = reinterpret_cast<type_vsf>(::GetProcAddress(hmDLL, "sf"));
		if (ExprtPtr == nullptr)
		{
			std::wcout << (L"��ȡ��������ʧ��!");
			return FALSE;
		}
	}

	
	type_v������ GenrateCodePtr = ExprtPtr(enum�汾::em_��ɽ������);
	if (GenrateCodePtr == nullptr)
	{
		//std::wcout << L"���ɲ����벻֧����Ϸ[" << itm.Identifier.wsGameType.c_str() << L"-" << itm.Identifier.wsGameName.c_str() << L"]" << std::endl;
		return FALSE;
	}

	//std::wcout << L"����������Ϸ:[" << itm.Identifier.wsGameType.c_str() << L"-" << itm.Identifier.wsGameName.c_str() << L"] �Ĳ���!" << std::endl;
	for (int i = 0;i <= 10; ++i)
	{
		type_OutInfo Content = { 0 };
		Content.n��ԭϵͳ����� = -1;
		if (Content.n��ԭϵͳ����� != -1)
		{
			std::wcout << L"�����=" << Content.n��ԭϵͳ����� << std::endl;
		}



		Content.n_shu = 0;
		Content.n_yin = 0; // 3 = KillScoreCode1 
		Content.n���� = 1;
		std::wcout << L"Ӯ1000��, ��0, ����=1" << std::endl;


		//libTools::CCharacter::strcpy_my(Content.n_as����id, libTools::CCharacter::UnicodeToASCII(wsSerialNumber).c_str());
		libTools::CCharacter::strcpy_my(Content.n_as����id, libTools::CCharacter::UnicodeToASCII(wsSerialNumber).c_str());
		if (!GenrateCodePtr(Content))
		{
			std::wcout << L"���ɲ�����ʧ��!" << std::endl;
			return FALSE;
		}

		std::wcout << L"������=" << libTools::CCharacter::ASCIIToUnicode(Content.n����_as������).c_str() << L", ��ѯ��=" << libTools::CCharacter::ASCIIToUnicode(Content.asȡ����).c_str() << std::endl;
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