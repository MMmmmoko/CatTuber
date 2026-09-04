//unzip.c的unz64local_CheckCurrentFileCoherencyHeader函数头部
//替换掉
//if (err == UNZ_OK)
//{
//    if (unz64local_getLong(&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
//        err = UNZ_ERRNO;
//    else if (uMagic != 0x04034b50)
//        err = UNZ_BADZIPFILE;
//}


//仅对普通大众用户隐藏文件格式信息，高级计算机使用者可能自己就能轻松发现仅为魔数串改，所以这里不在公开的代码中做隐藏
    //if (err==UNZ_OK)
    //{
    //    if (unz64local_getLong(&s->z_filefunc, s->filestream,&uMagic) != UNZ_OK)
    //        err=UNZ_ERRNO;
    //    else if (uMagic!=0x04034b50|| uMagic!=0x07505A43)//原魔数为PZ\0x3\0x4,我不希望文件被轻易识别为ZIP压缩包，这里兼容魔数CZP7
    //        err=UNZ_BADZIPFILE;
    //}
//弃用，7Z根本不看魔数的，光改文件头没卵用 淦