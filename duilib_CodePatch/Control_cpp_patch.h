//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加从自由资源包中获取文件的路径合法性
//用法是#include进GlobalManager.h的public段


//#define GlobalManager_patch_cpp
//#include"../../CatTuber64/duilib_CodePatch/GlobalManager_patch.h"
//#undef GlobalManager_patch_cpp





	//cpp中插入LoadImageInfo函数的ImageLoadPathType::kVirtualPath判别条件前




//作为提醒写在这里：
// 之后更换duilib库的时候要在release项目中添加如下预处理器定义
//_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR=1

if (sImagePath[0]==L'[')
{
	imageLoadPath.m_pathType = ImageLoadPathType::kVirtualPath;

}


