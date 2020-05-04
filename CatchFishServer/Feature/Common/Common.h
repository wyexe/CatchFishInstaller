#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_COMMON_COMMON_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_COMMON_COMMON_H__

#include <string>
#include <memory>
#include <map>
#include <TimeLib/TimeCharacter.h>
#include <SocketCommon/SocketBuffer.h>
#include "GenrateCodeExport.h"

#ifdef _DEBUG
#pragma comment(lib,"SocketCommonLib_Debug.lib")
#else
#pragma comment(lib,"SocketCommonLib.lib")
#endif // _DEBUG


#define SERVER_IP		L"120.79.171.219"
#define 老板_SERVER_IP	L"47.104.242.137"
#define SERVER_PORT	12366

struct GameIdentity
{
	std::wstring wsGameType;
	std::wstring wsGameName;

	bool operator == (CONST GameIdentity& RightParam) CONST
	{
		return this->wsGameType == RightParam.wsGameType && this->wsGameName == RightParam.wsGameName;
	}
};


enum em_Sock_Msg
{
	em_Sock_Msg_Error = 0xF,
	em_Sock_Msg_Download_File = 0x10,
	em_Sock_Msg_KeepLive,
	em_Sock_Msg_BinaryCode,
	em_Sock_Msg_Record_Install,
	em_Sock_Msg_Version_File,
	em_Sock_Msg_Generate_TestCode,
	em_Sock_Msg_Generate_StandardCode,
	em_Sock_Msg_Get_FishList,
	em_Sock_Msg_Get_CodeTypeList,
	em_Sock_Msg_Download_Identifier,
	em_Sock_Msg_OtherServer_KeepAlive,
	em_Sock_Msg_ExcuteSQL,
	em_Sock_Msg_Generate_Specify_StandCode,
	em_Sock_Msg_CreateNew_CatchFishFileContent,
	em_Sock_Msg_CreateNew_ChildSpecifyStandCode,
	em_Sock_Msg_Contact_Detail,
	em_Sock_Msg_Upload_GameFileHead,
	em_Sock_Msg_Upload_GameFileContent,
	em_Sock_Msg_IsExist_GameFile,
	em_Sock_Msg_Get_VecIdentifier,
	em_Sock_Msg_Query_CountCode,
};


struct CatchFishBinaryCode
{
	DWORD dwAddr;
	DWORD dwValue;
};

struct CatchFishStructContent : public libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	// Tranfer
	GameIdentity						Identifier;
	std::wstring						wsFullPath;
	DWORD								dwFileCRC32;
	UINT								uFileSize;
	BOOL								IsDisableDeleteGameDirectoryInjectorFile;
	std::vector<std::wstring>			VecSupplementPath;
	std::vector<CatchFishBinaryCode>	VecCode;
	std::wstring						wsCopyDestPath;
	BOOL								IsExistRestore;
	DWORD								dwSeriesSerialNumber;
	std::wstring						wsInjectorPath;
	enum版本								emVersion;
	bool								IsRandDiskSerailNumber;
	bool								IsDifferenceDisk;
	bool								IsNewFramework;
	// No Tranfer
	libTools::CTimeCharacter::TimeContent ActiveDate;


	virtual CONST CatchFishStructContent& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << Identifier.wsGameType << Identifier.wsGameName << wsFullPath << dwFileCRC32 << uFileSize;
		SocketBuffer << dwSeriesSerialNumber << wsCopyDestPath << IsExistRestore << wsInjectorPath << static_cast<DWORD>(emVersion);
		SocketBuffer << IsRandDiskSerailNumber << IsDisableDeleteGameDirectoryInjectorFile << IsDifferenceDisk;
		SocketBuffer << IsNewFramework << VecSupplementPath.size();
		for (auto& itm : VecSupplementPath)
			SocketBuffer << itm;

		SocketBuffer << VecCode.size();
		for (auto& itm : VecCode)
			SocketBuffer << itm.dwAddr << itm.dwValue;


		return *this;
	}

	virtual CatchFishStructContent & operator << (libTools::CSocketBuffer & SocketBuffer) override
	{
		UINT uSize = 0;
		SocketBuffer >> Identifier.wsGameType >> Identifier.wsGameName >> wsFullPath >> dwFileCRC32;
		SocketBuffer >> uFileSize >> dwSeriesSerialNumber >> wsCopyDestPath >> reinterpret_cast<DWORD&>(IsExistRestore);
		SocketBuffer >> wsInjectorPath >> reinterpret_cast<DWORD&>(emVersion) >> IsRandDiskSerailNumber;
		SocketBuffer >> IsDisableDeleteGameDirectoryInjectorFile >> IsDifferenceDisk;
		SocketBuffer >> IsNewFramework >> uSize;
		for (UINT i = 0; i < uSize; ++i)
		{
			std::wstring wsText;
			SocketBuffer >> wsText;
			VecSupplementPath.push_back(wsText);
		}


		SocketBuffer >> uSize;
		for (UINT i = 0; i < uSize; ++i)
		{
			CatchFishBinaryCode BCode;
			SocketBuffer >> BCode.dwAddr >> BCode.dwValue;
			VecCode.push_back(BCode);
		}

		return *this;
	}

	void Release()
	{
		VecCode.clear();
		VecSupplementPath.clear();
	}
};

struct Tranfer_SpecifyCustomerContent : public libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	std::wstring IdentifyText;
	std::wstring wsWinScore;
	std::wstring wsLoseScore;
	DWORD		 dwCodeIndex;
	DWORD		 dwRandCode;

	virtual CONST Tranfer_SpecifyCustomerContent& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << this->IdentifyText << this->wsWinScore << this->wsLoseScore << dwCodeIndex << dwRandCode;
		return *this;
	}

	virtual Tranfer_SpecifyCustomerContent& operator << (libTools::CSocketBuffer& SocketBuffer) override
	{
		SocketBuffer >> this->IdentifyText >> this->wsWinScore >> this->wsLoseScore >> dwCodeIndex >> dwRandCode;
		return *this;
	}
};

struct SpecifyCustomerContent : public libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	std::wstring EncryptDiskSerialNumber;
	std::wstring IdentityText;
	GameIdentity Identifier;
	std::wstring wsQueryCode;

	virtual CONST SpecifyCustomerContent& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << Identifier.wsGameType << Identifier.wsGameName << EncryptDiskSerialNumber;
		return *this;
	}

	virtual SpecifyCustomerContent& operator << (libTools::CSocketBuffer& SocketBuffer) override
	{
		SocketBuffer >> Identifier.wsGameType >> Identifier.wsGameName >> EncryptDiskSerialNumber;
		return *this;
	}
};

struct JsonFileContent
{
	std::vector<CatchFishStructContent> VecContent;
	std::vector<std::wstring>			VecCodeType;
	std::vector<SpecifyCustomerContent> VecSpecifyCustomerContent;
};

struct CatchFishFileContent
{
	std::shared_ptr<CHAR>	FileContent;
	UINT					uFileSize;
	std::wstring			wsFileName;
	DWORD					dwCRC32;
};

struct CatchFishFileStruct
{
	GameIdentity			Identifier;
	std::vector<CatchFishFileContent> VecFileContent;
};

struct GenerateCodeContent : public libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	GameIdentity Identifier;
	std::wstring wsOriginDiskSerialNumber;
	std::wstring wsEncryptDiskSerialNumber;
	std::wstring wsCodeType;
	std::wstring wsWinScore;
	std::wstring wsLoseScore;
	DWORD		 dwCodeIndex;
	DWORD		 dwRandCode;
	BOOL	     IsHideCode;

	virtual CONST GenerateCodeContent& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << this->Identifier.wsGameType << this->Identifier.wsGameName << this->wsOriginDiskSerialNumber;
		SocketBuffer << this->wsEncryptDiskSerialNumber << this->wsCodeType << this->dwRandCode << this->dwCodeIndex;
		SocketBuffer << this->wsWinScore << this->wsLoseScore << this->IsHideCode;
		return *this;
	}
	virtual GenerateCodeContent& operator << (libTools::CSocketBuffer& SocketBuffer) override
	{
		SocketBuffer >> this->Identifier.wsGameType >> this->Identifier.wsGameName >> this->wsOriginDiskSerialNumber;
		SocketBuffer >> this->wsEncryptDiskSerialNumber >> this->wsCodeType >> this->dwRandCode >> this->dwCodeIndex;
		SocketBuffer >> this->wsWinScore >> this->wsLoseScore >> this->IsHideCode;
		return *this;
	}
};

struct SingleInstallDiskRecord
{
	// Generate in Client
	std::wstring	wsOriginText;

	std::wstring	wsDiskSerialNumber;
	DWORD			dwPhysicsDiskIndex;
	DWORD			dwDiskSize;


	// Generate in Server
	std::wstring	wsEncodeSerialNumber;
	std::wstring	wsLaoBanEncodeSerialNumber;
	std::wstring	wsTestCode;
	std::wstring	wsCountCode;
};

struct InstallDiskRecord_Tranfer : public libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	// Fill in Client
	GameIdentity Identifier;
	std::wstring wsOriginDiskSerialNumber;
	std::wstring wsEncryptDiskSerialNumber;


	std::vector<SingleInstallDiskRecord> Vec;


	// Fill in Server
	std::wstring wsClientIp;


	// 
	virtual CONST InstallDiskRecord_Tranfer& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << Identifier.wsGameType << Identifier.wsGameName << wsOriginDiskSerialNumber << wsEncryptDiskSerialNumber << Vec.size();
		for (auto& itm : Vec)
		{
			SocketBuffer << itm.wsDiskSerialNumber << itm.dwPhysicsDiskIndex << itm.dwDiskSize << itm.wsOriginText;
		}

		return *this;
	}

	virtual InstallDiskRecord_Tranfer& operator << (libTools::CSocketBuffer& SocketBuffer) override
	{
		SocketBuffer >> Identifier.wsGameType >> Identifier.wsGameName >> wsOriginDiskSerialNumber >> wsEncryptDiskSerialNumber;


		UINT uSize = 0;
		SocketBuffer >> uSize;
		for (UINT i = 0; i < uSize; ++i)
		{
			SingleInstallDiskRecord itm;
			SocketBuffer >> itm.wsDiskSerialNumber >> itm.dwPhysicsDiskIndex >> itm.dwDiskSize >> itm.wsOriginText;
			Vec.push_back(itm);
		}

		return *this;
	}
};

struct ContactContent
{
	std::wstring wsName;
	std::wstring wsTelphone;
	std::wstring wsQQNumber;
	std::wstring wsWechat;


};

struct ContactContent_Tranfer : libTools::BaseSocketBuffer<libTools::CSocketBuffer>
{
	ContactContent Content;

	virtual CONST ContactContent_Tranfer& operator >> (libTools::CSocketBuffer& SocketBuffer) CONST override
	{
		SocketBuffer << Content.wsTelphone << Content.wsQQNumber << Content.wsWechat << Content.wsName;
		return *this;
	}

	virtual ContactContent_Tranfer& operator << (libTools::CSocketBuffer& SocketBuffer) override
	{
		SocketBuffer >> this->Content.wsTelphone >> this->Content.wsQQNumber >> this->Content.wsWechat << Content.wsName;
		return *this;
	}
};


struct UploadFileContent
{
	std::wstring wsGamePath;
	std::wstring wsGameType;
	UINT		 uMaxFileSize;
	DWORD		 dwFileCRC32;
	UINT		 uCurrentSize;
	std::shared_ptr<CHAR> FileContentPtr;
};


enum class em_GenerateCode_ScoreType
{
	LoseScore,
	WinScore,
	KillScore,
};


struct GenerateCodeScoreContent
{
	em_GenerateCode_ScoreType emScoreType;
	std::vector<std::wstring> VecScoreText;
};


CONST static std::map<em_GenerateCode_ScoreType, std::vector<std::wstring>> g_VecGenerateCodeScoreContent =
{
	{
		em_GenerateCode_ScoreType::LoseScore,
		{
			L"0",
			L"1000",
			L"3000",
			L"5000",
			L"10000",
		}
	},
	{
		em_GenerateCode_ScoreType::WinScore,
		{
			L"1000",
			L"2000",
			L"3000",
			L"5000",
			L"8000",
			L"10000",
			L"15000",
			L"20000",
			L"100000",
		}
	},
	{
		em_GenerateCode_ScoreType::KillScore,
		{
			L"10%杀分码",
			L"20%杀分码",
			L"30%杀分码",
			L"40%杀分码",
			L"50%杀分码",
		}
	},
};


#define DOWNLOADER_MEMORYSHARE_NAME		L"DOWNLOADER_MEMORY_SHARE"
struct DownloaderMemoryShareContent
{
	WCHAR wszWorkDirectory[MAX_PATH];
	WCHAR wszTelphone[64];
	WCHAR wszQQNumber[64];
	WCHAR wszWeChat[64];
};

#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_COMMON_COMMON_H__
