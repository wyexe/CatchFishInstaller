#ifndef __CATCHFISHINSTALLER_CATCHFISHSERVER_FEATURE_CONFIG_LOCALCONFIG_H__
#define __CATCHFISHINSTALLER_CATCHFISHSERVER_FEATURE_CONFIG_LOCALCONFIG_H__

#include <CatchFishServer/Feature/Common/Common.h>

class CLocalConfig
{
public:
	CLocalConfig() = default;
	~CLocalConfig() = default;

	static std::wstring GetConfigValue(_In_ CONST std::wstring& wsKeyName, _In_ CONST std::wstring& wsConfigName);
};


#endif // __CATCHFISHINSTALLER_CATCHFISHSERVER_FEATURE_CONFIG_LOCALCONFIG_H__
