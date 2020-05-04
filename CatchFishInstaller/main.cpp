#include <Windows.h>
#include <string>
#include <iostream>
#include <LogLib/Log.h>
#include <CharacterLib/Character.h>
#include <Feature/Install/Installer.h>
#include <Feature/Socket/Client/IocpClient.h>
#include <Feature/Clear/ClearFile.h>
#include <FileLib/File.h>
#include <CatchFishInstaller/Feature/Socket/Download/Downloader.h>
#define _SELF L"main.cpp"



VOID ViewTestCode()
{
	GameIdentity Ideneifier;
	Ideneifier.wsGameType = L"JSKJ系列";
	Ideneifier.wsGameName = L"20181222皇家礼炮";
	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);


	std::wstring wsCountCode, wsTestCode;
	std::wstring wsOriginDiskSerialNumber = L"SZHYPO1612230511221";
	if (!CDownloader::GetInstance().GetGenerateCode(Ideneifier, wsOriginDiskSerialNumber, wsCountCode, wsTestCode))
	{
		std::wcout << L"从服务器上下载测试码和取次码失败!" << std::endl;
		return;
	}

	//auto _wsDiskSerialNumber = libTools::CCharacter::MakeFormatText(L"FTGC%04X%s", 0x0303, wsOriginDiskSerialNumber.c_str());
	std::wstring wsDiskSerialNumber = wsOriginDiskSerialNumber;

	auto ptr = [&wsDiskSerialNumber]
	{
		std::wstring wsNewText;
		for (UINT i = 0; i < wsDiskSerialNumber.size(); i += 2)
		{
			WCHAR wszText[3] = { 0 };
			wszText[0] = wsDiskSerialNumber[i];
			wszText[1] = wsDiskSerialNumber[i + 1];


			wsNewText += libTools::CCharacter::MakeFormatText(L"%02x", std::stoul(wszText, nullptr, 16) ^ 0xFF);
		}
		wsDiskSerialNumber = libTools::CCharacter::MakeFormatText(L"FTGD%04X%s", 0x0515, wsNewText.c_str());
	};

	// ==VV==%04X%s
	ptr();
	exit(0);
}

namespace dhj8938rwjf
{
	int __stdcall _添加导入表(const char* str_TargetDll, const char* str_AddDllName, const char* str_AddFunctionName);

	BOOL __stdcall _已修改过(void* pbuff_, DWORD lens, const char* str_AddDllName);
}

int main(int argc, char* argv[])
{
	//dhj8938rwjf::_添加导入表("Z:\\海洋世界20180710\\gdi32.dll", "uspdat.dll", "ftsWordBreak");
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();
	if (::OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"CATCHFISH_INSTALLER") != nullptr)
	{
		std::wcout << L"安装程序已经运行了……禁止运行多个!" << std::endl;
		system("pause");
		return 0;
	}


	::CreateMutexW(nullptr, FALSE, L"CATCHFISH_INSTALLER");
	libTools::CLog::GetInstance().SetClientName(L"CatchFishInstaller", libTools::CCharacter::MakeCurrentPath(L"\\"));
	if (!CIocpClient::GetInstance().Run(老板_SERVER_IP, SERVER_PORT, 10 * 1000))
	{
		LOG_MSG_CF(L"连接服务器失败!");
		return 0;
	}

#ifdef _DEBUG
	//ViewTestCode();
#endif // _DEBUG



	if (argc == 1)
	{
		CInstaller::GetInstance().Install(libTools::CCharacter::MakeCurrentPath(L"\\"));
		system("pause");
	}
	else if (argc == 2)
	{
		const std::wstring wsArg = libTools::CCharacter::ASCIIToUnicode(argv[1]);
		if (wsArg == L"?")
		{
			std::wcout << L"-> ReleaseDirectory" << std::endl;
			std::wcout << L"-> UnInstall" << std::endl;
			std::wcout << L"-> IngoreDiskSerialNumber" << std::endl;
			std::wcout << L"-> Install dumpTool" << std::endl;
			std::wcout << L"-> pull log" << std::endl;
			std::wcout << L"-> pull dump" << std::endl;
			std::wcout << L"-> ClearRestore" << std::endl;
			std::wcout << L"-> ClearTheCount" << std::endl;
		}
		else if (wsArg == L"uninstall")
		{
			CInstaller::GetInstance().UnInstall();
		}
		else if (wsArg == L"ingorediskserialnumber")
		{
			CInstaller::GetInstance().SetIngoreDiskSerialNumber();
			CInstaller::GetInstance().Install(libTools::CCharacter::MakeCurrentPath(L"\\"));
		}
		else if (wsArg == L"clearrestore")
		{
			CInstaller::GetInstance().ClearRestoreSystem();
		}
		else if (wsArg == L"clearthecount")
		{
			CInstaller::GetInstance().ClearTheCount();
		}
		else
		{
			if (libTools::CFile::FileExist(wsArg))
			{
				std::wcout << L"识别 => " << std::endl;
				CInstaller::GetInstance().IsExistGame(wsArg);
			}
			else
			{
				std::wcout << L"安装 => " << std::endl;
				CInstaller::GetInstance().Install(wsArg);
			}
		}
	}
	else if (argc == 3) // Installer.exe Install dumpTool
	{
		const std::string Argv3 = libTools::CCharacter::MakeTextToLower(std::string(argv[1]));
		const std::string Argv4 = libTools::CCharacter::MakeTextToLower(std::string(argv[2]));
		if (Argv3 == "install")
		{
			if (Argv4 == "dumptool")
				CInstaller::GetInstance().InstallDumpTool();
			else
				std::wcout << L"Invalid Arg4" << std::endl;
		}
		else if (Argv3 == "pull")
		{
			if (Argv4 == "log")
				CInstaller::GetInstance().PullLog();
			else
				std::wcout << L"Invalid Arg4" << std::endl;
		}
		else
			std::wcout << L"Invalid Arg3" << std::endl;
	}
	else
		std::wcout << L"Invalid Arg Count:" << argc << std::endl;

	system("pause");
	return 0;
}