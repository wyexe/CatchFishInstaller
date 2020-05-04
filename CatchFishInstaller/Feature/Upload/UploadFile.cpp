#include "UploadFile.h"
#include <FileLib/File.h>
#include <filesystem>
#include <iostream>
#include <AlgroithmLib/Encrypt/CRC32.h>
#include <ProcessLib/Common/ResHandleManager.h>
#include <LogLib/Log.h>
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>

#define _SELF L"UploadFile.cpp"
CUploadFile& CUploadFile::GetInstance()
{
	static CUploadFile Instance;
	return Instance;
}

BOOL CUploadFile::IsExistCompatibleFile(_In_ WCHAR wchDisk)
{
	std::wstring wsPath1, wsPath2; 
	// 3A系列 
	wsPath1 = L"Z:\\game\\fish.exe";
	wsPath2 = L"Z:\\game\\preprocess.exe";
	wsPath1[0] = wchDisk;
	wsPath2[0] = wchDisk;
	if (libTools::CFile::FileExist(wsPath2) && libTools::CFile::FileExist(wsPath1))
	{
		_wsGamePath = wsPath1;
		_wsGameType = L"3A系列";
		return TRUE;
	}

	// jskj系列
	wsPath1 = L"I:\\fish\\fish.exe";
	wsPath2 = L"I:\\WINDOWS\\jskj\\catchfish.exe";
	wsPath1[0] = wchDisk;
	wsPath2[0] = wchDisk;
	if (libTools::CFile::FileExist(wsPath1) && libTools::CFile::FileExist(wsPath2))
	{
		_wsGameType = L"jskj系列";
		_wsGamePath = wsPath2;
		return TRUE;
	}

	// 明牌鱼系列
	wsPath1 = L"H:\\tggame\\tggame.exe";
	wsPath1[0] = wchDisk;
	if (libTools::CFile::FileExist(wsPath1))
	{
		_wsGameType = L"明牌鱼系列";
		_wsGamePath = wsPath1;
		return TRUE;
	}


	// 极星鱼系列
	{
		CONST static std::vector<std::wstring> Vec = 
		{
			{ L"load.MPQ", L"data.HNG", L"hInfo.MPQ", L"data.MPQ" }
		};


		// 搜索二级目录和System32
		wsPath1 = wchDisk;
		BOOL bExist = FALSE;
		for (auto& p : std::experimental::filesystem::v1::directory_iterator(wsPath1))
		{
			for (auto& p2 : std::experimental::filesystem::v1::directory_iterator(p.path().wstring()))
			{
				auto wsFileName = p2.path().filename().wstring();
				if (wsFileName.find(L".exe") != std::wstring::npos)
				{
					wsPath2 = wsFileName;
				}


				auto itr = std::find_if(Vec.begin(), Vec.end(), [wsFileName](CONST std::wstring& itm) { return itm == wsFileName; });
				if (itr != Vec.end())
				{
					// Exist File
					bExist = TRUE;
				}


				if (bExist && !wsPath2.empty())
				{
					_wsGameType = L"极星鱼系列";
					_wsGamePath = wsFileName;
					return TRUE;
				}
			}
		}
	}


	// 满堂彩系列
	{
		CONST static std::vector<std::wstring> Vec =
		{
			{ L"game_eft.ini", L"game_eft.txt", L"game_update.ini", L"game.exe" }
		};



		BOOL bExist = TRUE;
		for (auto& itm : Vec)
		{
			wsPath1 = L"I:\\";
			wsPath1[0] = wchDisk;

			if (!libTools::CFile::FileExist(wsPath1 + itm))
			{
				bExist = FALSE;
				break;
			}
		}

		
		if (bExist)
		{
			_wsGameType = L"满堂彩系列";

			wsPath1 = L"I:\\";
			wsPath1[0] = wchDisk;
			_wsGamePath = wsPath1 + L"game.exe";
			return TRUE;
		}

	}


	// 海王系列
	{
		wsPath1 = L"I:\\fish\\fish1.exe";
		wsPath2 = L"I:\\fish\\Sms.dll";
		wsPath1[0] = wchDisk;
		wsPath2[0] = wchDisk;
		if (libTools::CFile::FileExist(wsPath1) && libTools::CFile::FileExist(wsPath2))
		{
			_wsGameType = L"海王系列";
			_wsGamePath = wsPath1;
			return TRUE;
		}
	}

	return FALSE;
}

CONST std::wstring& CUploadFile::GetGameType() CONST
{
	return _wsGameType;
}

CONST std::wstring& CUploadFile::GetGamePath() CONST
{
	return _wsGamePath;
}

BOOL CUploadFile::UpLoadGameFile()
{
	auto wsFileName = std::experimental::filesystem::v1::path(_wsGamePath).filename().wstring();

	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;

	if (!libTools::CFile::ReadFileContent(_wsGamePath, lpFileContent, uFileSize))
	{
		std::wcout << L"读取游戏文件[" << _wsGamePath.c_str() << L"] 失败!" << std::endl;
		return FALSE;
	}
	SetResDeleter(lpFileContent, [](LPVOID& p) { ::VirtualFree(p, 0, MEM_RELEASE); });

	_dwUploadPercent = 0;
	{
		// 上传文件头
		libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Upload_GameFileHead);
		SendSocketBuffer << _wsGamePath;
		SendSocketBuffer << _wsGameType;
		SendSocketBuffer << uFileSize;
		SendSocketBuffer << libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFileContent), uFileSize);
		if (!CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [](libTools::CSocketBuffer&) {}))
		{
			return FALSE;
		}
	}
	


	// 上传文件内容 每次上传1M
	CONST UINT uMaxTranferSize = 1024 * 1024;
	UINT uUploadCount = uFileSize / uMaxTranferSize + (uFileSize % uMaxTranferSize != 0 ? 1 : 0);
	for (UINT i = 0;i < uUploadCount; ++i)
	{
		_dwUploadPercent = i * 100 / uUploadCount;
		libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_Upload_GameFileContent);

		UINT uTranferSize = (uFileSize - i * uMaxTranferSize < uMaxTranferSize) ? (uFileSize - i * uMaxTranferSize) : uMaxTranferSize;
		SendSocketBuffer << uTranferSize;
		SendSocketBuffer.SetDataPtr(reinterpret_cast<CONST CHAR*>(lpFileContent) + i * uMaxTranferSize, uTranferSize);
		if (!CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [](libTools::CSocketBuffer&) {}))
		{
			std::wcout << L"SyncSend Timeout!" << std::endl;
			return FALSE;
		}
	}

	
	return TRUE;
}

BOOL CUploadFile::IsExistGameFileInServer(_Out_ BOOL& bResult) CONST
{
	auto wsFileName = std::experimental::filesystem::v1::path(_wsGamePath).filename().wstring();

	LPVOID lpFileContent = nullptr;
	SIZE_T uFileSize = 0;

	if (!libTools::CFile::ReadFileContent(_wsGamePath, lpFileContent, uFileSize))
	{
		std::wcout << L"读取游戏文件[" << _wsGamePath.c_str() << L"] 失败!" << std::endl;
		return FALSE;
	}
	SetResDeleter(lpFileContent, [](LPVOID& p) { ::VirtualFree(p, 0, MEM_RELEASE); });

	//em_Sock_Msg_IsExist_GameFile
	libTools::CSocketBuffer SendSocketBuffer(em_Sock_Msg_IsExist_GameFile);
	SendSocketBuffer << libTools::CCRC32::GetCRC32(reinterpret_cast<LPCSTR>(lpFileContent), uFileSize);



	return CIocpClient::GetInstance().SyncSend(SendSocketBuffer, [&bResult](libTools::CSocketBuffer& SocketBuffer)
	{
		SocketBuffer >> reinterpret_cast<DWORD&>(bResult);
	});
}

DWORD CUploadFile::GetUploadFilePercent() CONST
{
	return _dwUploadPercent;
}

