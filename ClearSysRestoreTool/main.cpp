#include <Windows.h>
#include <iostream>
#include <CharacterLib/Character.h>
#include <InjectorLib/DllInjector/DllInjector.h>
#include <CatchFishInstaller/Feature/Clear/ClearFile.h>

#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"user32.lib")


int main()
{
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();


	if (!libTools::CDllInjector::IsRunAsAdministrator())
	{
		std::wcout << L"Please Run as Admin" << std::endl;
		system("pause");
		return 0;
	}

	for (;;)
	{
		std::wcout << L"---------------------" << std::endl;
		std::wcout << L"1¡¢ClearSysRestore" << std::endl;
		std::wcout << L"---------------------" << std::endl;
		std::wcout << L">";


		std::wstring wsLine;
		std::getline(std::wcin, wsLine);
		if (libTools::CCharacter::MakeTextToLower(wsLine) == L"clearsysrestore")
		{
			std::wstring wsDisk;
			std::wcout << std::endl << L"Disk:(for exmaple: H)";
			std::wcin >> wsDisk;
			if (wsDisk.empty())
			{
				std::wcout << std::endl << L"Invalid Disk!" << std::endl;
				continue;
			}


			static WCHAR wchPath[MAX_PATH] = { L"Z:\\Persi0.sys" };
			wchPath[0] = wsDisk[0];
			CClearFile::GetInstance().ClearGameRestoreSystem_YouLingQianTing(libTools::CCharacter::UnicodeToASCII(wchPath).c_str());
			std::wcout << std::endl;
		}
		else
		{
			std::wcout << std::endl << L"Invalid Command!" << std::endl;
		}
	}
	return 0;
}