#ifndef __CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_LOCALACTION_H__
#define __CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_LOCALACTION_H__

#include "ServerAction.h"

class CLocalAction
{
public:
	CLocalAction() = default;
	~CLocalAction() = default;

	static CLocalAction& GetInstance();
public:
	BOOL Refresh(_In_ CONST std::wstring& wsText);

	CONST std::vector<GameIdentityContent>& GetVecIdentify() CONST;

	CONST std::vector<std::wstring>& GetVecCodeType() CONST;
private:
	std::vector<GameIdentityContent>	_VecIdentifier;
	std::vector<std::wstring>			_VecCodeType;
};



#endif // !__CATCHFISHINSTALLER_CATCHFISHGENERATECODE_FEATURE_LOCALACTION_H__
