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