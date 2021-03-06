#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <CharacterLib/Character.h>
#include <ProcessLib/Process/Process.h>
#include <LogLib/Log.h>
#include <FileLib/File.h>
#include <locale.h>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"FileLib_Debug.lib")
#pragma comment(lib,"CharacterLib_Debug.lib")
#pragma comment(lib,"ProcessLib_Debug.lib")
#pragma comment(lib,"user32.lib")
#else
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"FileLib.lib")
#pragma comment(lib,"CharacterLib.lib")
#pragma comment(lib,"ProcessLib.lib")
#pragma comment(lib,"user32.lib")
#endif // _DEBUG


#define _SELF L"main.cpp"

std::string UnicodeToASCII(_In_ CONST std::wstring& wstr)
{
	std::string str;
	const int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr) + 1; // 936
	char* szAnsi = new char[ansiLen];
	ZeroMemory(szAnsi, ansiLen * sizeof(CHAR));


	::WideCharToMultiByte(CP_ACP, NULL, wstr.c_str(), static_cast<int>(wstr.length()), szAnsi, ansiLen, nullptr, nullptr);
	szAnsi[ansiLen - 1] = '\0';

	str = szAnsi;
	delete[] szAnsi;
	return str;
}

using DirectoryPath = std::experimental::filesystem::v1::path;
using DirectoryItr = std::experimental::filesystem::v1::directory_iterator;

void DumpFile()
{
	std::wstring wsPath = LR"(C:\Users\TestWin7\Desktop\Fish\版本更新\)";
	std::wstring wsLpk = wsPath + (wsPath.back() == L'\\' ? L"lpk.dll" : L"\\lpk.dll");
	DirectoryPath GameDir(wsPath);
	for (DirectoryItr DirItr(GameDir); DirItr != DirectoryItr(); ++DirItr)
	{
		if (!std::experimental::filesystem::v1::is_directory(DirItr->path()))
			continue;

		//LOG_MSG_CF(L"Path=%s", DirItr->path().wstring().c_str());
		::CopyFileW(wsLpk.c_str(), (DirItr->path().wstring() + L"\\lpk.dll").c_str(), TRUE);
		for (DirectoryItr itr(DirItr->path()); itr != DirectoryItr(); ++itr)
		{
			auto wsFileName = libTools::CCharacter::MakeTextToLower(itr->path().filename().wstring());
			if (wsFileName.find(L"gamealone_enc") == -1)
			{
				//::DeleteFileW(itr->path().wstring().c_str());
				continue;
			}


			std::wcout << L"Dumping " << DirItr->path().filename().wstring().c_str() << std::endl;
			PROCESS_INFORMATION pi = { 0 };
			libTools::CProcess::CreateProc(itr->path().wstring(), FALSE, L"", &pi);
			//::Sleep(1000);
			::WaitForSingleObject(pi.hProcess, INFINITE);
			::CloseHandle(pi.hProcess);
			//LOG_MSG_CF(L"Path=%s", itr->path().wstring().c_str());
		}
	}
}

void DeleteFile_ExceptDumpFile()
{
	std::wstring wsPath = LR"(C:\Users\TestWin7\Desktop\Fish\版本更新\)";
	std::wstring wsLpk = wsPath + (wsPath.back() == L'\\' ? L"lpk.dll" : L"\\lpk.dll");
	DirectoryPath GameDir(wsPath);
	for (DirectoryItr DirItr(GameDir); DirItr != DirectoryItr(); ++DirItr)
	{
		if (!std::experimental::filesystem::v1::is_directory(DirItr->path()))
			continue;

		//LOG_MSG_CF(L"Path=%s", DirItr->path().wstring().c_str());
		::CopyFileW(wsLpk.c_str(), (DirItr->path().wstring() + L"\\lpk.dll").c_str(), TRUE);
		for (DirectoryItr itr(DirItr->path()); itr != DirectoryItr(); ++itr)
		{
			auto wsFileName = libTools::CCharacter::MakeTextToLower(itr->path().filename().wstring());
			if (wsFileName.find(L"dump.exe") == -1)
			{
				::DeleteFileW(itr->path().wstring().c_str());
				continue;
			}
			//LOG_MSG_CF(L"Path=%s", itr->path().wstring().c_str());
		}
	}
}

// ida.exe -B "E:\BaiduNetdiskDownload\捕鱼\海王传说系列\007渔乐财神终结版1\gamealone_enc.dump"
void GenerateIdaFile()
{
	std::wstring wsPath = LR"(E:\BaiduNetdiskDownload\捕鱼\海王传说系列\)";
	DirectoryPath GameDir(wsPath);
	for (DirectoryItr DirItr(GameDir); DirItr != DirectoryItr(); ++DirItr)
	{
		if (!std::experimental::filesystem::v1::is_directory(DirItr->path()))
			continue;

		std::wstring wsGamePath = DirItr->path().wstring() + L"\\gamealone_enc.dump.exe";
		if (!libTools::CFile::FileExist(wsGamePath))
			continue;


		std::wcout << L"Generating " << DirItr->path().filename().wstring().c_str() << std::endl;
		std::wstring wsFormatText = libTools::CCharacter::MakeFormatText(LR"(D:\IDA 7.0\ida.exe -B "%s")", wsGamePath.c_str());
		PROCESS_INFORMATION pi = { 0 };
		if (!libTools::CProcess::CreateProc(wsFormatText, FALSE, DirItr->path().wstring(), &pi))
		{
			LOG_MSG_CF(L"CreateProc Faild: Path[%s]", wsGamePath.c_str());
			continue;
		}

		::WaitForSingleObject(pi.hProcess, INFINITE);
	}
}


// ida -S"Z:\CatchFish_jskj.py" "E:\BaiduNetdiskDownload\捕鱼\海王传说系列\007渔乐财神终结版1\gamealone_enc.idb"
VOID ExcuteIdaScript()
{
	std::wstring wsPath = LR"(E:\BaiduNetdiskDownload\捕鱼\海王传说系列\)";
	DirectoryPath GameDir(wsPath);
	for (DirectoryItr DirItr(GameDir); DirItr != DirectoryItr(); ++DirItr) 
	{
		if (!std::experimental::filesystem::v1::is_directory(DirItr->path()))
			continue;

		std::wstring wsGamePath = DirItr->path().wstring() + L"\\gamealone_enc.dump.idb";
		if (!libTools::CFile::FileExist(wsGamePath))
			continue;



		std::wcout << L"Generating " << DirItr->path().filename().wstring().c_str() << std::endl;
		std::wstring wsFormatText = libTools::CCharacter::MakeFormatText(LR"(D:\IDA 7.0\ida.exe -S"Z:\CatchFish_jskj.py" "%s")", wsGamePath.c_str());
		PROCESS_INFORMATION pi = { 0 };
		if (!libTools::CProcess::CreateProc(wsFormatText, FALSE, DirItr->path().wstring(), &pi))
		{
			LOG_MSG_CF(L"CreateProc Faild: Path[%s]", wsGamePath.c_str());
			continue;
		}

		::WaitForSingleObject(pi.hProcess, INFINITE);
	}
}

int main()
{
	libTools::CCharacter::SetConsoleLanguage();
	libTools::CCharacter::SetSpecialCharacterMode();
	ExcuteIdaScript();
	//GenerateIdaFile();
	//DumpFile();
	//DeleteFile_ExceptDumpFile();
	return 0;
}