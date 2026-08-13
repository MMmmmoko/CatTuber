//对原库做了个补充
//文件分离以在未来可能的更新后不覆盖代码，并到时候提醒具体的插入位置

//在SDL_net.c的最末尾include本文件

bool NET_GetLocalLANAddressStr(char* buf, int buflen)
{
    Socket sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        return false;
    }

    // Connect the socket to a public ip (here 1.1.1.1) on any
    // port. This will give the local address of the network interface
    // used for default routing which is usually what we want.


    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    inet_pton(AF_INET, "8.8.8.8", &server_addr.sin_addr);

    int nameLen = sizeof(server_addr);
    if (connect(sock, (struct sockaddr*)&server_addr, nameLen) == -1)
    {
        //CloseSocketHandle(sock);
        //
        //return NULL;
    }


    if (getsockname(sock, (struct sockaddr*)&server_addr, &nameLen) == -1)
    {
        CloseSocketHandle(sock);
        return false;
    }

    // Close the socket
    CloseSocketHandle(sock);

    // Finally build the IP address

    inet_ntop(server_addr.sin_family, &server_addr.sin_addr, buf, buflen);


    return true;

}




bool NET_GetLocalComputerName(char* buf, int buflen)
{
    
#ifdef SDL_PLATFORM_WINDOWS
    //Windows API 获取计算机名
	DWORD size = (DWORD)buflen;
    return GetComputerNameA(buf, &size);
#endif

#ifdef SDL_PLATFORM_MACOS
    return gethostname(buf, buflen) == 0
#endif


    return true;

}



unsigned short NET_GetServerPort(NET_Server* server)
{
    if (server)
    {
        return server->port;
    }
    return 0;
}

unsigned short NET_GetAvailablePort()
{
    SOCKET sock;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int port = -1;

    // 1. 创建 TCP 套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        //perror("socket creation failed");
        return -1;
    }

    // 2. 初始化地址结构体，将端口设置为 0
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定到所有可用网卡
    addr.sin_port = htons(0);                 // 关键：传入 0 让系统自动分配

    // 3. 绑定套接字
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        //perror("bind failed");
        CloseSocketHandle(sock);
        return -1;
    }

    // 4. 获取系统实际分配的端口号
    if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0) {
        //perror("getsockname failed");
        CloseSocketHandle(sock);
        return -1;
    }

    // 注意：getsockname 返回的是网络字节序，需要使用 ntohs 转换为主机字节序
    port = ntohs(addr.sin_port);

    // 5. 关闭套接字，释放端口（此时该端口变为可用状态）
    CloseSocketHandle(sock);

    return (unsigned short)(port&0xFFFF);
}