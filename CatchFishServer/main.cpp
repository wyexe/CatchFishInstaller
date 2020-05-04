#include <Windows.h>
#include <iostream>
#include <Feature/Socket/Server/IOCPServer.h>
#include <Feature/BinaryCode/BinaryCoder.h>
#include <Feature/Download/Downloader.h>
#include <Feature/Statistics/StatisticsRecorder.h>
#include <Feature/Res/ResText.h>
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>
#include <FileLib/File.h>
#include <ProcessLib/Process/Process.h>
#include "Feature/Socket/Echo/EchoPacket.h"
#include "Feature/Backup/FileBackup.h"

void Refresh()
{
	if(!CFileBackup::Run())
	{
		std::wcout << L"±¸·ÝÊ§°Ü!" << std::endl;
		system("pause");
		exit(0);
	}

	libTools::CException::InvokeAction(__FUNCTIONW__, [] 
	{
		if (!libTools::CFile::FileExist(libTools::CCharacter::MakeCurrentPath(L"\\Res.json")))
		{
			std::wcout << L"UnExist File:[Res.json]" << std::endl;
			system("pause");
			exit(0);
		}
		if (!CDownloader::GetInstance().Refresh())
		{
			std::wcout << L"CDownloader.RefreshDownFile = FALSE" << std::endl;
			system("pause");
			exit(0);
		}
		if (!CResText::GetInstance().Refresh())
		{
			std::wcout << L"CDownloader.RefreshGameIdentifierContent = FALSE" << std::endl;
			system("pause");
			exit(0);
		}
	});
}


int main()
{
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();

	if (!libTools::CProcess::IsExistProcName(L"FishMail.exe"))
	{
		std::wcout << L"UnExist Process FishMail.exe" << std::endl;
		system("pause");
		return 0;
	}
	
	libTools::CLog::GetInstance().SetClientName(L"CatchFishServer", libTools::CCharacter::MakeCurrentPath(L"\\"));
	CEchoPacket::Initialize();
	CStatisticsRecorder::GetInstance().Initialize();
	Refresh();


	CIOCPServer IocpServer;
	if (!IocpServer.Run(SERVER_PORT, 100))
	{
		std::wcout << L"IOCP.Run = FALSE" << std::endl;
		system("pause");
	}

	for (std::wstring wsCmd;;)
	{
		std::wcout << L"Tip: Refresh | ActiveGame" << std::endl;
		std::wcout << L"1>";
		std::wcin >> wsCmd;

		wsCmd = libTools::CCharacter::MakeTextToLower(wsCmd);
		if (wsCmd == L"exit")
		{
			break;
		}
		else if (wsCmd == L"refresh")
		{
			Refresh();
			std::wcout << L"     Done!";
		}
		else if (wsCmd == L"activegame")
		{
			GameIdentity Identifier;
			std::wcout << L"GameType:";
			std::wcin >> Identifier.wsGameType;
			std::wcout << std::endl;


			std::wcout << L"GameName:";
			std::wcin >> Identifier.wsGameName;
			std::wcout << std::endl;


			std::wcout << L"Active " << (CResText::GetInstance().ActiveGame(Identifier) ? L"Succ!" : L"Faild!");
		}
		std::wcout << std::endl;
	}
	
	return 0;
}