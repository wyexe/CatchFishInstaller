#ifndef fghg8934woeifjwehgklniywf
#define fghg8934woeifjwehgklniywf

enum enum版本
{
	em_Null = 0,
	em_正版千里马,
	em_盗版千里马,
	em_正版海龙龟,
	em_正版聚宝盆,
	em_正版鳄鱼王,
	/*em_悟空闹海,*/
	em_明牌鱼招财猫,
	em_幽灵潜艇,
	em_常山赵子龙,
	em_聚宝盆20180525,
	em_聚宝盆盗版3,
	em_恭喜发财之悟空闹海20180603,
	em_皇家龙舟20180604,
	em_金猴20180604,
	em_金鸡下蛋之三色金龙鱼20180604,
	/*em_精卫填海20180604  --*/
	em_牛霸江湖20180604,
	em_万马奔腾新20180604,
	em_月光宝盒20180604,
	em_战狼20180604,
	em_宝莲灯钻石美人鱼20180605,
	em_Currency_UnExist_RandCode,
	em_Currency_Exist_RandCode,
	em_新3A修改算码 = 1000,
	em_正版聚宝盆_改ID算法20180812,
	em_新架构算2019010 = 3000,
	em_新架构算2019010_2 = 4000,   //
};

struct type_OutInfo
{
	char as取次码[256]{};		char as私活码[256]{};
	char as测试码[256]{};
	char as加密ID[256]{};


	char n_as原始id[256]{};
	char n_as加密id[256]{};
	int  n次数 = -1;
	int  n还原系统随机码 = -1;     //有还原系统的游戏才用
	int  n_shu;
	int  n_yin;
	char n次数_as次数码[256]{};


	struct  type_Hide
	{
		char as取次码[256]{};		char as私活码[256]{};
		char as测试码[256]{};
		char as加密ID[256]{};


		char n_as原始id[256]{};
		char n_as加密id[256]{};
		int  n次数 = -1;
		int  n还原系统随机码 = -1;     //有还原系统的游戏才用
		int  n_shu;
		int  n_yin;
		char n次数_as次数码[256]{};
	};
	type_Hide  dat_Hide;
};


using type_v生成码 = int(__stdcall*)(type_OutInfo& OutInfo);
using type_vsf = type_v生成码(WINAPIV*)(enum版本 enumVersion);

//导出函数  type_v生成码 __cdecl sf(enum版本 enumVersion)







#endif
