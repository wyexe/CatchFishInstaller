#ifndef __CATCHFISHINSTALLER_SINGLEGENERATECODE_FEATURE_H__
#define __CATCHFISHINSTALLER_SINGLEGENERATECODE_FEATURE_H__

#include <GenerateCodeDLL\Feature\BaseServerAction.h>

class CFeature : public CBaseServerAction
{
public:
	CFeature() = default;
	~CFeature() = default;

	static CFeature& GetInstance();


	BOOL GenerateStandardCode(_In_ CONST Tranfer_SpecifyCustomerContent& Content, _Out_ std::wstring& wsQueryCode, _Out_ std::wstring& wsCheatCode);

	BOOL QueryCountCode(_In_ CONST std::wstring& wsCountCode, _Out_ std::wstring& wsText);
private:

};


#endif // !__CATCHFISHINSTALLER_SINGLEGENERATECODE_FEATURE_H__
