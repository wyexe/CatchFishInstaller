#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_RES_RESTEXT_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_RES_RESTEXT_H__

#include <ProcessLib/Lock/Lock.h>
#include <CatchFishServer/Feature/Common/Common.h>


class CResText : public libTools::CThreadLock
{
public:
	CResText() = default;
	~CResText() = default;

	static CResText& GetInstance();

	
	//////////////////////////////////////////////////////////////////////////
	//
	BOOL	ActionToCatchFishStructContent_If_Exist(_In_ CONST GameIdentity& Ideneifier, _In_ std::function<VOID(CatchFishStructContent&)> Ptr);

	//////////////////////////////////////////////////////////////////////////
	CONST std::wstring& ConvertScoreIndexToText(_In_ em_GenerateCode_ScoreType emScoreType, _In_ DWORD Index) CONST;

	INT				    ConvertScoreTextToIndex(_In_ em_GenerateCode_ScoreType emScoreType, _In_ CONST std::wstring& wsScoreText) CONST;

	VOID ActionToJsonFileContent(_In_ std::function<VOID(JsonFileContent&)> Ptr);

	BOOL Refresh();

	VOID SaveCatchFishStructContentToJson(_In_ CONST CatchFishStructContent& Content);

	VOID SaveSpecifyCustomerContentToJson(_In_ CONST SpecifyCustomerContent& Content);

	BOOL ActiveGame(_In_ CONST GameIdentity& Identifier);

	BOOL GetContactContentByName(_In_ CONST std::wstring& wsName, _Out_ ContactContent& Contact) CONST;

	BOOL GenerateCode(_In_ CONST GameIdentity& Identifier,  _In_ _Out_ type_OutInfo& Content);

	BOOL RefreshVecCatchFishStructContent();

	BOOL RefreshSpecifyCustomerJson();
private:
	BOOL CheckJsonContent() CONST;
private:
	JsonFileContent					_JsonFileContent;
};



#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_RES_RESTEXT_H__
