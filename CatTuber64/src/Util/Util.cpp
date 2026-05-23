#include<SDL3/SDL.h>
#include<SDL3_image/SDL_image.h>
#include"Net/SDL_net.h"
#include<fstream>
#include"Util/Util.h"
#include"AppSettings.h"
#include"AppContext.h"
#include"Pack/Pack.h"


unsigned char* util::SDL_LoadFileToMem(const char* path, size_t* size)
{
    //通过SDL加载文件
    SDL_IOStream* modelFileStream = SDL_IOFromFile(path, "r");
    if (!modelFileStream)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Open File Failed: %s", path);
        *size = 0;
        return NULL;
    }
    uint64_t _size = SDL_GetIOSize(modelFileStream);
    void* buffer = SDL_malloc(_size);
    SDL_ReadIO(modelFileStream, buffer, _size);

    SDL_CloseIO(modelFileStream); // 关闭文件流


    *size = _size;
	return (unsigned char*)buffer;
}

std::vector<uint8_t> util::SDL_LoadFileToMem(const char* path)
{
    //通过SDL加载文件
    SDL_IOStream* modelFileStream = SDL_IOFromFile(path, "r");
    if (!modelFileStream)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Open File Failed: %s", path);
        return std::vector<uint8_t>();
    }
    uint64_t _size = SDL_GetIOSize(modelFileStream);
    std::vector<uint8_t> result;
    result.resize(_size);

    SDL_ReadIO(modelFileStream, result.data(), _size);

    SDL_CloseIO(modelFileStream); // 关闭文件流


    return std::move(result);

}

void util::SDL_FreeMem(unsigned char* mem)
{
    SDL_free(mem);
}

void util::SDL_CopyFolder(const char* oldFolder, const char* newFolder)
{

    SDL_CreateDirectory(newFolder);
    //先判断oldFoloer是否存在
    SDL_EnumerateDirectoryCallback fileCallback = [](void* userdata, const char* dirname, const char* fname) -> SDL_EnumerationResult
        {
            char pathBuf[1024];
            SDL_snprintf(pathBuf, 1024, "%s%s", dirname, fname);

            char newPathBuf[1024];
            SDL_snprintf(newPathBuf, 1024, "%s/%s", (const char*)userdata, fname);

            SDL_PathInfo pathInfo;
            if (SDL_GetPathInfo(pathBuf,&pathInfo))
            {

                if (pathInfo.type == SDL_PATHTYPE_DIRECTORY)
                {
 
                    //SDL_CreateDirectory(newPathBuf);
                    util::SDL_CopyFolder(pathBuf, newPathBuf);
                }
                else if (pathInfo.type == SDL_PATHTYPE_FILE)
                {
                    SDL_CopyFile(pathBuf, newPathBuf);
                }
            }



            return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
        };



    SDL_EnumerateDirectory(oldFolder, fileCallback, (void*)newFolder);
}

Json::Value util::BuildJsonFromMem(const char* mem, size_t memSize)
{
    Json::CharReaderBuilder builder;

    builder["emitUTF8"] = true;

    std::unique_ptr<Json::CharReader>reader(builder.newCharReader());

    Json::Value root;
    std::string errs;
    bool ok = reader->parse(mem, mem+ memSize,&root,&errs);
    if (!ok)
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Failed to parse json : %s",errs.c_str());
        return Json::Value();
    }
    return root;
}

Json::Value util::BuildJsonFromFile(const char* filePath)
{
    size_t size;
    auto mem=SDL_LoadFileToMem(filePath,&size);
    if(!mem)
        return Json::Value();
    
    Json::Value result=BuildJsonFromMem((const char*)mem,size);
    SDL_FreeMem(mem);
    return result;
}

bool util::SaveJsonToFile(const Json::Value& json, const char* filePath)
{
    Json::StreamWriterBuilder builder;
    builder["emitUTF8"] = true;
    std::string jsonStr=Json::writeString(builder, json);


    return SDL_SaveFile(filePath, jsonStr.c_str(), jsonStr.size());

    //std::unique_ptr<Json::StreamWriter> writer(
    //    builder.newStreamWriter());


    //std::ostream stream;

    //改用SDL实现，原生C++接口比较难以处理中文文件名

    



    //std::ofstream ofs;
    //ofs.open(filePath);
    //if (!ofs)
    //{
    //    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Failed to write file: %s", filePath);
    //    return false;
    //}

    //writer->write(json, &ofs);
    //ofs.close();
    //return true;
}



#if 0
std::string util::ReadJsonStringWithLang(const Json::Value& json, const char* curLang, const char* fullback)
{
    //从json中按语言读取字符串
   // "Name": { 
     //   "schinese": "米米",
     //       "english" : "Mimi"
   // },
    
    //最先尝试目标curLang, 然后依次尝试curLang的后备语言，如果没有项目则返回空
 
    if (nullptr == fullback)fullback = "";


	if (json.isNull())return std::string(fullback);

    if (json.isMember(curLang) && json[curLang].isString())
    {
		return json[curLang].asString();
    }

	//未匹配//尝试后备语言
    const std::vector<std::string>& fullbackLangVec=AppSettings::GetLocalLanguageFullbackVec();


    for (auto& x : fullbackLangVec)
    {
        if (json.isMember(x) && json[x].isString())
        {
            return json[curLang].asString();
        }
    }

    //未匹配//尝试任意内容项目
	Json::Value::Members members=json.getMemberNames();
    for (auto& x : members)
    {
        if (json.isMember(x) && json[x].isString())
        {
            return json[curLang].asString();
        }
    }
    

	//未匹配 //返回默认值

    return std::string(fullback);
}
#endif



bool util::IsStringEndsWith(const std::string& str, const char* end)
{
    std::string ending = end;
    if (str.size() < ending.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

bool util::IsStringStartWith(const std::string& str, const char* end)
{
    if (end == NULL)return false;
    size_t len = SDL_strlen(end)-1;
    if (str.size() < len)
    {
        return false;
    }
    return 0==SDL_memcmp(str.data(), end, len);
}

bool util::StringIsNumber(const std::string& str)
{
    for(auto x:str)
    {
        if (x >= '0' && x <= '9')
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string util::StringToUpper(const std::string& str)
{
    std::string temStr = str;
    for (char& c : temStr) {
        c = std::toupper(static_cast<unsigned char>(c));
    }
    return temStr;
}

std::string& util::ReplaceString(std::string& org, const char* strToBeReplace, const char* newstr)
{
    // TODO: 在此处插入 return 语句


        // 找到第一个匹配位置
    size_t startPos = 0;
    std::string from(strToBeReplace);
    std::string to(newstr);
    while ((startPos = org.find(from, startPos)) != std::string::npos) {
        org.replace(startPos, from.length(), to);
        startPos += to.length(); // 继续往后找，避免死循环
    }
    return org;
}

std::string util::GetFileNameFromPath(const std::string& path)
{
    size_t pos = path.find_last_of("/\\"); // 兼容两种分隔符
    std::string filename = (pos == std::string::npos) ? path : path.substr(pos + 1);
    return filename;
}

std::string util::RemoveExtension(const std::string& path)
{
    size_t pos = path.find_last_of("."); 
    std::string filename = (pos == std::string::npos) ? path : path.substr(pos + 1);
    return filename;
}

void util::AppendSeparator(std::string& path)
{
    if (path.empty())
    {
        path = path + "/";
        return;
    }
    if (path[path.size() - 1] == '\\' || path[path.size() - 1] == '/')
    {
        return;
    }
    path = path + "/";
}

bool util::PathEqual(const std::string& path1, const std::string& path2)
{
    std::string _pathstr1 = path1;
    std::string _pathstr2 = path2;
    ReplaceString(_pathstr1,"\\","");
    ReplaceString(_pathstr1,"/","");
    ReplaceString(_pathstr2,"\\","");
    ReplaceString(_pathstr2,"/","");

    return _pathstr1== _pathstr2;
}

std::string util::GetStringFromMultiLangJsonNode(const Json::Value& json)
{
    //如果json是string则不进行后续操作
    if (json.isString())
        return json.asString();
    //语言采用SteamAPI的形式
    //https://partner.steamgames.com/doc/store/localization/languages

    //先查询UI设置的语言
    std::string UISettingLang = AppSettings::GetIns().GetMiscLanguage();
    if (UISettingLang!="unspecified"&& json.isMember(UISettingLang) && json[UISettingLang].isString())
    {
        return json[UISettingLang].asString();
    }



    auto& langVec = AppSettings::GetLocalLanguageFullbackVec();
    for (auto& x : langVec)
    {
        if (json.isMember(x) && json[x].isString())
        {
            return json[x].asString();
        }
    }


    //没有的话从json中随便挑个语言
    if (json.isObject())
    {
        for (auto& key:json.getMemberNames())
        {
            if (json[key].isString())
            {
                return json[key].asString();
            }
        }
    }


    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Failed to get multilang json string.");
    return std::string();
}






SDL_GPUTexture* util::LoadTextureFromPack(Pack* pack, const char* pathInPack, int* w , int* h )
{
    std::vector<uint8_t> fileBytes= pack->LoadFile(pathInPack);
    SDL_IOStream* io = SDL_IOFromConstMem(fileBytes.data(), fileBytes.size());
    if (!io) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_IOFromConstMem failed: %s/%s:%s", SDL_GetError(), pack->GetPath(), pathInPack);
        return nullptr;
    }

    SDL_Surface* surface = IMG_Load_IO(io, true);
    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "WARNING: Can not load texture:%s:%s", pack->GetPath(), pathInPack);  
        return nullptr;
    }

    SDL_GPUTextureCreateInfo texInfo = {};
    texInfo.type = SDL_GPU_TEXTURETYPE_2D,
        texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        texInfo.width = (surface != NULL) ? static_cast<uint32_t>(surface->w) : 10,
        texInfo.height = (surface != NULL) ? static_cast<uint32_t>(surface->h) : 10,
        texInfo.layer_count_or_depth = 1,
        //texInfo.num_levels = 1,
        texInfo.num_levels = (uint32_t)floor(log2((double)(texInfo.width > texInfo.height ? texInfo.width : texInfo.height))) + 1;
    texInfo.usage = (texInfo.num_levels == 1) ? SDL_GPU_TEXTUREUSAGE_SAMPLER : (SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);
    //SDL_GPUTexture* s

    SDL_GPUTexture* gpuTexture = SDL_CreateGPUTexture(AppContext::GetGraphicDevice(), &texInfo);
    if (gpuTexture)
    {
        //上传纹理
        if (surface)
        {
            SDL_GPUTransferBufferCreateInfo tbInfo = {};
            tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            tbInfo.size = surface->pitch * surface->h;


            SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(AppContext::GetGraphicDevice(), &tbInfo);
            void* mapped = SDL_MapGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer, false);
            memcpy(mapped, surface->pixels, tbInfo.size);
            SDL_UnmapGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer);

            
            SDL_GPUCommandBuffer* cmd = AppContext::GetSDL3RenderContext()->GetCopyCommandBuffer();
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

            SDL_GPUTextureTransferInfo src = { };
            src.offset = 0;
            src.pixels_per_row = surface->w;
            src.rows_per_layer = surface->h;
            src.transfer_buffer = transferBuffer;

            SDL_GPUTextureRegion dst = {};
            dst.texture = gpuTexture;
            //mip_level layer被初始化为0了，不管
            dst.w = surface->w;
            dst.h = surface->h;
            dst.d = 1;


            SDL_UploadToGPUTexture(copyPass, &src, &dst, false);
            SDL_EndGPUCopyPass(copyPass);

            //MIPMAP
            if (texInfo.num_levels > 1)
                SDL_GenerateMipmapsForGPUTexture(cmd, gpuTexture);

            //SDL_SubmitGPUCommandBuffer(cmd);

            SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), transferBuffer);


            if (w)*w = surface->w;
            if (h)*h = surface->h;
            SDL_DestroySurface(surface);
        }
    }

    return gpuTexture;
}

MIX_Audio* util::LoadSoundFromPack(Pack* pack, const char* pathInPack)
{
    std::vector<uint8_t> fileBytes = pack->LoadFile(pathInPack);
    SDL_IOStream* io = SDL_IOFromConstMem(fileBytes.data(), fileBytes.size());
    if (!io) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_IOFromConstMem failed: %s/%s:%s", SDL_GetError(), pack->GetPath(), pathInPack);
        return nullptr;
    }
    MIX_Audio* audio=MIX_LoadAudio_IO(AppContext::GetMixerDevice(),io,true,true);
    if (!audio)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "WARNING: Can not load audio:%s:%s", pack->GetPath(), pathInPack);
        return nullptr;
    }
    return audio;
}





std::string util::GetLocalIP()
{
    //这个方法得到的地址数量太多了，换一个方法获取地址
    //int addrCount;
    //NET_Address** addrs= NET_GetLocalAddresses(&addrCount);
    //for (int i = 0; i < addrCount; i++)
    //{
    //    SDL_Log("Local IP:%s", NET_GetAddressString(addrs[i]));
    //}
    //return std::string();



    // The method here is to connect a UDP socket to a public ip,
    // and get the local socket address with the getsockname function.
    // UDP connection will not send anything to the network, so this function won't cause any overhead.

    char buf[100];
    if (NET_GetLocalLANAddressStr(buf, sizeof(buf)))
    {
        return buf;
    }
    return std::string();
}

















































































































//CatTuber由MMmmmoko独立开发（https://space.bilibili.com/5808772），源码仅做展示不进行任何授权，所有权利归MMmmmoko所有