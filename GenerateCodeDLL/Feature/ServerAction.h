#ifndef __CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_SERVERACTION_H__
#define __CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_SERVERACTION_H__

#include "BaseServerAction.h"


struct GameIdentityContent
{
	GameIdentity Idenfier;
	BOOL		 ExistRandCode;
	DWORD		 dwIdentifierSerialNumber;
};

class CServerAction : public CBaseServerAction
{
public:
	CServerAction() = default;
	~CServerAction();

	static CServerAction& GetInstance();
public:
	BOOL GenerateStandardCode(_In_ CONST GenerateCodeContent& Content, _Out_ std::wstring& wsQueryCode, _Out_ std::wstring& wsCheatCode);

	BOOL GetFishList(_Out_ std::vector<GameIdentityContent>& VecIdentifyer);

	BOOL GetVecCodeType(_Out_ std::vector<std::wstring>& VecCodeType);

	BOOL GetGenerateCode(_In_ CONST GameIdentity& Identifier, _In_ std::wstring& wsDiskSerialNumber, _Out_ std::wstring& wsCountCode, _Out_ std::wstring& wsTestCode);

	BOOL CreateNewChildGenerateCode(_In_ CONST SpecifyCustomerContent& Content, _Out_ std::wstring& IdentityText, _Out_ BOOL& bExistRandCode);

	BOOL CreateNewFishGame(_In_ libTools::CSocketBuffer& SocketBuffer);
};



#endif // !__CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_SERVERACTION_H__
