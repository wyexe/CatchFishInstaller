#include <Windows.h>
#include <iostream>
#include "InstallerDownloader.h"
#include <CharacterLib/Character.h>
#include <InjectorLib/DllInjector/DllInjector.h>
#include <filesystem>
#include <CatchFishInstaller/Feature/Clear/ClearFile.h>

#ifdef _DEBUG
#pragma comment(lib,"InjectorLib_Debug.lib")
#else
#pragma comment(lib,"InjectorLib.lib")
#endif // _DEBUG

std::string GetInstallerPath()
{
	CHAR szPath[MAX_PATH] = { 0 };
	::GetTempPathA(MAX_PATH, szPath);
	if (szPath[strlen(szPath) - 1] != '\\')
		::lstrcatA(szPath, "\\");

	::lstrcatA(szPath, "CatchFishInstaller\\");
	if (!std::experimental::filesystem::v1::is_directory(std::experimental::filesystem::v1::path(szPath)))
	{
		std::experimental::filesystem::v1::create_directory(std::experimental::filesystem::v1::path(szPath));
	}

	return std::string(szPath);
}

int main(int argc, char* argv[])
{
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();

	if (!libTools::CDllInjector::IsRunAsAdministrator())
	{
		std::wcout << L"必须运行在管理员权限下!" << std::endl;
		system("pause");
		return 0;
	}

	if (::OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"CATCHFISH_DOWNLOADER") != NULL)
	{
		std::wcout << L"下载程序已经启动! 不能启动多个!" << std::endl;
		system("pause");
		return 0;
	}


	::CreateMutexW(NULL, FALSE, L"CATCHFISH_DOWNLOADER");
	
	if (argc == 1)
	{
		CInstallerDownloader().RunInstaller();
		::Sleep(5 * 1000);
	}
	else if (argc == 2)
	{
		// Downloader.exe OpenInstallerDir
		std::string Arg = libTools::CCharacter::MakeTextToLower(std::string(argv[1]));
		if (Arg == libTools::CCharacter::MakeTextToLower(std::string("OpenInstallerDir")))
		{
			std::string Cmd = "explorer \"";
			Cmd += GetInstallerPath();
			Cmd += "\"";
			system(Cmd.c_str());
		}
		else if (Arg == libTools::CCharacter::MakeTextToLower(std::string("?")))
		{
			std::wcout << L"-> OpenInstallerDir" << std::endl;
		}
	}

	
	return 0;
}