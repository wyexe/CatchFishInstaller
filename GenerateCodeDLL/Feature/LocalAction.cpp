#include "LocalAction.h"
#include <set>
#include <LogLib/Log.h>

#ifdef _DEBUG
#pragma comment(lib,"LogLib_Debug.lib")
#else
#pragma comment(lib,"LogLib.lib")
#endif // _DEBUG




#define _SELF L"LocalAction.cpp"
CLocalAction& CLocalAction::GetInstance()
{
	static CLocalAction Instance;
	return Instance;
}

BOOL CLocalAction::Refresh(_In_ CONST std::wstring& wsText)
{
	if (!CServerAction::GetInstance().Run(wsText))
	{
		LOG_MSG_CF(L"连接服务器校验失败! Err=Run");
		return FALSE;
	}

	_VecIdentifier.clear();
	if (!CServerAction::GetInstance().GetFishList(_VecIdentifier))
	{
		LOG_MSG_CF(L"连接服务器校验失败! Err=GetFishList");
		return FALSE;
	}


	_VecCodeType.clear();
	if (!CServerAction::GetInstance().GetVecCodeType(_VecCodeType))
	{
		LOG_MSG_CF(L"连接服务器校验失败! Err=GetVecCodeType");
		return FALSE;
	}


	return TRUE;
}

CONST std::vector<GameIdentityContent>& CLocalAction::GetVecIdentify() CONST
{
	return _VecIdentifier;
}

CONST std::vector<std::wstring>& CLocalAction::GetVecCodeType() CONST
{
	return _VecCodeType;
}
