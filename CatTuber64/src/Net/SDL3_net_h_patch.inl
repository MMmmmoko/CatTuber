//对原库做了个补充
//文件分离以在未来可能的更新后不覆盖代码，并到时候提醒具体的插入位置

//在SDL_net.h的末尾include本文件

extern bool NET_GetLocalLANAddressStr(char* buf, int buflen);
extern bool NET_GetLocalComputerName(char* buf, int buflen);