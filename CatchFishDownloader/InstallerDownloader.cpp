#include "InstallerDownloader.h"
#include <filesystem>
#include <iostream>
#include <future>
#include <Shlwapi.h> // PathRemoveFileSpecW
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>
#include <LogLib/Log.h>
#include <CharacterLib/Character.h>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <FileLib/File.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <ProcessLib/Process/Process.h>
#include <AlgroithmLib/Encrypt/CRC32.h>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#pragma comment(lib,"SocketClientLib_Debug.lib")
#pragma comment(lib,"AlgroithmLib_Debug.lib")
#else
#pragma comment(lib,"SocketClientLib.lib")
#pragma comment(lib,"LogLib.lib")
#pragma comment(lib,"AlgroithmLib.lib")

#endif // _DEBUG
#pragma comment(lib,"Shlwapi.lib")
#define _SELF L"InstallerDownloader.cpp"

CInstallerDownloader::CInstallerDownloader() : _uInstallerFileSize(0)
{

}

CInstallerDownloader::~CInstallerDownloader()
{
	libTools::CLog::GetInstance().Release();
	CIocpClient::GetInstance().Stop();
}

BOOL CInstallerDownloader::RunInstaller()
{
	if (!Initialize())
	{
		return FALSE;
	}
	else if (!DownloadContactContent())
	{
		EchoError(L"下载联系人失败!");
		return FALSE;
	}
	else if (!WriteContactConfig())
	{
		EchoError(L"写入配置项失败!");
		return FALSE;
	}


	if (IsDownloadInstaller())
	{
		if (!DownloadInstaller())
		{
			EchoError(L"从服务器下载安装程序失败!");
			return FALSE;
		}


		// Write Installer to File
		libTools::CFile::WriteFile(_wsInstallerPath.c_str(), reinterpret_cast<CONST BYTE*>(_InstallerBuffer.get()), _uInstallerFileSize);
	}


	std::wcout << L"准备自动运行安装程序……" << std::endl;
	return CreateInstallerProc();
}

BOOL CInstallerDownloader::Initialize()
{
	//libTools::CLog::GetInstance().SetClientName(L"CatchFishDownloader", libTools::CCharacter::MakeCurrentPath(L"\\"));
	if (!CIocpClient::GetInstance().Run(老板_SERVER_IP, SERVER_PORT, 10 * 1000))
	{
		EchoError(L"连接服务器失败!!!");
		return FALSE;
	}


	std::wcout << L"连接服务器成功!" << std::endl;
	auto tmpPath = std::experimental::filesystem::v1::temp_directory_path();
	if (!std::experimental::filesystem::v1::is_directory(tmpPath))
	{
		EchoError(L"无法找到系统的临时目录!");
		return FALSE;
	}


	_wsInstallerPath = tmpPath.wstring();
	if (_wsInstallerPath.back() != L'\\')
		_wsInstallerPath.append(L"\\");


	_wsInstallerPath += L"CatchFishInstaller\\";
	if (!std::experimental::filesystem::v1::is_directory(std::experimental::filesystem::v1::path(_wsInstallerPath)))
	{
		std::experimental::filesystem::v1::create_directory(std::experimental::filesystem::v1::path(_wsInstallerPath));
	}
	_wsInstallerPath += L"Installer.exe";
	return TRUE;
}

VOID CInstallerDownloader::EchoError(_In_ CONST std::wstring& wsErrText) CONST
{
	std::wcout << wsErrText.c_str() << std::endl;
}

DWORD CInstallerDownloader::GetServerInstallerCRC32() CONST
{
	DWORD dwCRC32 = 0;

	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Version_File);
	SendSocketBuffer << L"Installer" << L"Installer";
	CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&dwCRC32](libTools::CSocketBuffer& SocketBuffer)
	{
		SocketBuffer >> dwCRC32;
	});

	return dwCRC32;
}

DWORD CInstallerDownloader::GetLocalInstallerCRC32() CONST
{
	LPVOID  lpFileContent = nullptr;
	SIZE_T	uFileSize = 0;


	if (!libTools::CFile::ReadFileContent(_wsInstallerPath, lpFileContent, uFileSize))
	{
		EchoError(L"读取安装器的内容失败!");
		return FALSE;
	}


	SetResDeleter(lpFileContent, [](LPVOID& Ptr) { ::VirtualFree(Ptr, 0, MEM_RELEASE); Ptr = nullptr; });
	return libTools::CCRC32::GetCRC32(static_cast<LPCSTR>(lpFileContent), uFileSize);
}

BOOL CInstallerDownloader::IsDownloadInstaller() CONST
{
	std::wcout << L"正在检查安装程序..." << std::endl;


	// Check Installer.exe in Temp
	if (!std::experimental::filesystem::v1::exists(std::experimental::filesystem::v1::path(_wsInstallerPath)))
	{
		LOG_C_D(L"UnExist Installer.exe in Temp");
		return TRUE;
	}


	// comp CRC32
	DWORD dwLocalInstallerCRC32 = GetLocalInstallerCRC32();
	if (dwLocalInstallerCRC32 == 0)
		return FALSE;

	
	DWORD dwServerInstallerCRC32 = GetServerInstallerCRC32();
	if (dwServerInstallerCRC32 == 0)
		return FALSE;


	return dwLocalInstallerCRC32 != dwServerInstallerCRC32;
}

BOOL CInstallerDownloader::DownloadInstaller()
{
	BOOL IsRunAsyc = TRUE;
	std::wcout << L"下载最新版的安装程序中 ." << std::endl;

	auto Printer = std::async(std::launch::async, [&IsRunAsyc]
	{
		while (IsRunAsyc)
		{
			std::wcout << L".";
			for (int i = 0;i < 10 && IsRunAsyc; ++i)
			{
				::Sleep(100);
			}
		}
	});


	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Download_File);
	SendSocketBuffer << L"Installer" << L"Installer";
	BOOL bResult = CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [this](libTools::CSocketBuffer& SocketBuffer)
	{
		UINT uFileCount = 0;
		SocketBuffer >> uFileCount;


		std::wstring wsFileName;
		if (uFileCount != 0)
		{
			SocketBuffer >> wsFileName >> _dwFileCRC32 >> _uInstallerFileSize;
			_InstallerBuffer = SocketBuffer.GetSizeBuffer(_uInstallerFileSize);
		}
	});


	IsRunAsyc = FALSE;
	Printer.wait();
	return bResult;
}

BOOL CInstallerDownloader::CreateInstallerProc() CONST
{
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi = { 0 };

	WCHAR wszProcWorkDirectory[MAX_PATH] = { 0 };
	wsprintfW(wszProcWorkDirectory, L"%s", _wsInstallerPath.c_str());
	if (!::PathRemoveFileSpecW(wszProcWorkDirectory))
	{
		::MessageBoxW(nullptr, libTools::CCharacter::MakeFormatText(L"Remove ProcPath[%s] FileSpec Faild", wszProcWorkDirectory).c_str(), L"Error", NULL);
		return FALSE;
	}

	WCHAR wszCmd[MAX_PATH] = { 0 };
	wsprintfW(wszCmd, L" \"%s\"", libTools::CCharacter::MakeCurrentPath(L"\\").c_str());
	if (!::CreateProcessW(_wsInstallerPath.c_str(), wszCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		//::MessageBoxW(nullptr, CCharacter::MakeFormatText(L"CreateProcess Fiald! ErrorCode=%X", ::GetLastError()).c_str(), L"Error", NULL);
		return FALSE;
	}



	::CloseHandle(pi.hProcess);
	return TRUE;
}

BOOL CInstallerDownloader::WriteContactConfig() CONST
{
	HANDLE hFileMap = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(DownloaderMemoryShareContent), DOWNLOADER_MEMORYSHARE_NAME);
	if (hFileMap == NULL)
	{
		LOG_MSG_CF(L"创建内存映射失败! Err=%d", GetLastError());
		return FALSE;
	}


	DownloaderMemoryShareContent* pShareContent = reinterpret_cast<DownloaderMemoryShareContent *>(::MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_SHARE_WRITE, NULL, NULL, sizeof(DownloaderMemoryShareContent)));
	if (pShareContent == nullptr)
	{
		LOG_MSG_CF(L"文件映射失败! Err=%d", GetLastError());
		return FALSE;
	}


	memset(pShareContent, 0, sizeof(pShareContent));
	::GetCurrentDirectoryW(_countof(pShareContent->wszWorkDirectory), pShareContent->wszWorkDirectory);
	libTools::CCharacter::strcpy_my(pShareContent->wszTelphone, _Contact.wsTelphone.c_str());
	libTools::CCharacter::strcpy_my(pShareContent->wszQQNumber, _Contact.wsQQNumber.c_str());
	libTools::CCharacter::strcpy_my(pShareContent->wszWeChat, _Contact.wsWechat.c_str());
	return TRUE;
}

BOOL CInstallerDownloader::DownloadContactContent()
{
	std::wcout << L"正在初始化中……" << std::endl;

	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Contact_Detail);
	SendSocketBuffer << L"代理1号";
	return CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [this](libTools::CSocketBuffer& SocketBuffer)
	{
		_Contact = SocketBuffer.GetStruct<ContactContent_Tranfer>().Content;
	});
}
