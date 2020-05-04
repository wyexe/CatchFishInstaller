#ifndef __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_STATISTICS_STATISTICSRECORDER_H__
#define __CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_STATISTICS_STATISTICSRECORDER_H__

#include <queue>
#include <DbManagerLib/BaseDbManager.h>
#include <ProcessLib/Lock/Lock.h>
#include <Feature/Common/Common.h>


class CStatisticsRecorder : public libTools::CBaseDbManager
{
public:
	CStatisticsRecorder();
	~CStatisticsRecorder();

	static CStatisticsRecorder& GetInstance();

	virtual VOID Initialize() override;


	// Add Record
	DWORD AddInstallRecord(_In_ CONST InstallDiskRecord_Tranfer& Content);

	//
	VOID AddInstallDiskRecord(_In_ DWORD dwInstallId, _In_ CONST InstallDiskRecord_Tranfer& Content);

	// 更新当前使用打码记录
	VOID AddGenerateCodeRecord(_In_ CONST GenerateCodeContent& Contact, _In_ CONST std::wstring& wsUser);

	//
	BOOL ExcuteClientSQL(_In_ CONST std::wstring& wsSQL, _Out_ std::vector<Table>& VecTable);
};


#endif // !__CATCHFISHINSTALL_CATCHFISHSERVER_FEATURE_STATISTICS_STATISTICSRECORDER_H__
