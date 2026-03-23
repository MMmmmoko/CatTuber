//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是增加从自由资源包中获取文件的路径合法性
//用法是#include进GlobalManager.h的public段


//#define GlobalManager_patch_cpp
//#include"../../CatTuber64/duilib_CodePatch/GlobalManager_patch.h"
//#undef GlobalManager_patch_cpp

#ifdef GlobalManager_patch_h
public:
	//返回fullpash, CatTuber中路径合规则直接原样返回
	typedef FilePath (*FilePathToFullPathFunc)(const FilePath& resPath);
	void SetFilePathToFullPathFuncFunc(FilePathToFullPathFunc _filePathToFullPathFunc) { filePathToFullPathFunc= _filePathToFullPathFunc; };
private:
	static inline FilePathToFullPathFunc filePathToFullPathFunc=nullptr;
public:
#endif // ImageAttribute_patch_h



	//cpp中插入GetExistsResFullPath函数的判别条件最后
#ifdef GlobalManager_patch_cpp
	if (imageFullPath.IsEmpty()&& filePathToFullPathFunc) {
		imageFullPath = filePathToFullPathFunc(resPath);
	}

#endif