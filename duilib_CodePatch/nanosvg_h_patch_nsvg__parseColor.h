//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


extern unsigned int nsvgPatch_getColorName(const char* str);
static unsigned int nsvg__parseColorName(const char* str)
{
	return nsvgPatch_getColorName(str);

	//int i, ncolors = sizeof(nsvg__colors) / sizeof(NSVGNamedColor);

	//for (i = 0; i < ncolors; i++) {
	//	if (strcmp(nsvg__colors[i].name, str) == 0) {
	//		return nsvg__colors[i].color;
	//	}
	//}

	//return NSVG_RGB(128, 128, 128);
}
