//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加从自由资源包中获取文件的路径合法性
//用法是#include进GlobalManager.h的public段


//#define GlobalManager_patch_cpp
//#include"../../CatTuber64/duilib_CodePatch/GlobalManager_patch.h"
//#undef GlobalManager_patch_cpp





	//cpp中插入LoadImageInfo函数的ImageLoadPathType::kVirtualPath判别条件前

//if ((rcSource.left < 0) || (rcSource.left >= (int32_t)pBitmap->GetWidth())) {
//    rcSource.left = 0;
//}
//if ((rcSource.top < 0) || (rcSource.top >= (int32_t)pBitmap->GetHeight())) {
//    rcSource.top = 0;
//}
//if ((rcSource.right < 0) || (rcSource.right > (int32_t)pBitmap->GetWidth())) {
//    rcSource.right = (int32_t)pBitmap->GetWidth();
//}
//if ((rcSource.bottom < 0) || (rcSource.bottom > (int32_t)pBitmap->GetHeight())) {
//    rcSource.bottom = (int32_t)pBitmap->GetHeight();
//}