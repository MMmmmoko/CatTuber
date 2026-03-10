#include "ImageDecoder_EX.h"


#include "duilib/Core/GlobalManager.h"
#include "duilib/Image/Image_Bitmap.h"
#include "duilib/Image/Image_PNG.h"
#include "duilib/Image/Image_Animation.h"
#include "duilib/Image/Image_GIF.h"



#include "duilib/Utils/FilePathUtil.h"

#include "duilib/Utils/StringConvert.h"

#include"Pack/Pack.h"


namespace ui
{

    //资源包路径分为两个部分[无"/"的资源包路径]资源包内部路径
    static std::wstring GetPackPath(const DString& imageFilePath)
    {
        size_t i = 0;
        int stackCount = 0;
        for (i = 0; i < imageFilePath.size(); i++)
        {
            if (imageFilePath[i] == L'[')stackCount++;
            if (imageFilePath[i] == L']')stackCount--;
            if (0 == stackCount)break;
        }
        if (0 == i)return L"";
        if (stackCount != 0)return L"";
        return imageFilePath.substr(1, i - 1);
    }
    static std::wstring GetFilePathInPack(const DString& imageFilePath)
    {
        size_t i = 0;
        int stackCount = 0;
        for (i = 0; i < imageFilePath.size(); i++)
        {
            if (imageFilePath[i] == L'[')stackCount++;
            if (imageFilePath[i] == L']')stackCount--;
            if (0 == stackCount)break;
        }
        if (0 == i)return L"";
        if (stackCount != 0)return L"";
        return imageFilePath.substr(i+1);
    }





    //PNG
    //PNG
    //PNG
    //PNG

    bool ui::ImageDecoder_PNG_EX::CanDecode(const DString& imageFilePath) const
    {
        if (GetPackPath(imageFilePath).empty())return false;
        return __super::CanDecode(imageFilePath);
    }


    std::unique_ptr<ui::IImage> ImageDecoder_PNG_EX::LoadImageData(const ImageDecodeParam& decodeParam)
    {

        bool bLoadAllFrames = decodeParam.m_bLoadAllFrames;
        bool bAsyncDecode = decodeParam.m_bAsyncDecode;
        float fImageSizeScale = decodeParam.m_fImageSizeScale;
        const UiSize& rcMaxDestRectSize = decodeParam.m_rcMaxDestRectSize;
        bool bAssertEnabled = decodeParam.m_bAssertEnabled;
        std::vector<uint8_t> emptyFileData;
        std::vector<uint8_t>& fileData = (decodeParam.m_pFileData != nullptr) ? *decodeParam.m_pFileData : emptyFileData;
        const FilePath& imageFilePath = decodeParam.m_imageFilePath;

        Image_PNG* pImagePNG = new Image_PNG;
        std::shared_ptr<IAnimationImage> pAnimationImage(pImagePNG);



        //插入从数据包中读文件的代码
        if (decodeParam.m_pFileData == nullptr)
        {
            std::string packPath = StringConvert::WStringToUTF8(  GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if(!pack.Open(packPath.c_str()))return nullptr;
            fileData=pack.LoadFile(StringConvert::WStringToUTF8(GetFilePathInPack(imageFilePath.ToString())).c_str());
        }





        if (!pImagePNG->LoadImageFile(fileData,
            imageFilePath,
            bLoadAllFrames,
            bAsyncDecode,
            fImageSizeScale,
            rcMaxDestRectSize,
            bAssertEnabled)) {
            return nullptr;
        }

        if (!bLoadAllFrames || (pImagePNG->GetFrameCount() == 1)) {
            //单帧，加载位图图片
            return Image_Bitmap::MakeImage(pAnimationImage);
        }
        else {
            //多帧图片
            std::unique_ptr<IImage> pImage(new Image_Animation(pAnimationImage));
            return pImage;
        }
    }







    //JPG
    //JPG
    //JPG
    //JPG
    //JPG




    bool ImageDecoder_JPG_EX::CanDecode(const DString& imageFilePath) const
    {

        if (GetPackPath(imageFilePath).empty())return false;
        return __super::CanDecode(imageFilePath);
    }


    std::unique_ptr<IImage> ImageDecoder_JPG_EX::LoadImageData(const ImageDecodeParam& decodeParam)
    {
        bool bAsyncDecode = decodeParam.m_bAsyncDecode;
        float fImageSizeScale = decodeParam.m_fImageSizeScale;
        const UiSize& rcMaxDestRectSize = decodeParam.m_rcMaxDestRectSize;
        bool bAssertEnabled = decodeParam.m_bAssertEnabled;
        std::vector<uint8_t> emptyFileData;
        std::vector<uint8_t>& fileData = (decodeParam.m_pFileData != nullptr) ? *decodeParam.m_pFileData : emptyFileData;
        const FilePath& imageFilePath = decodeParam.m_imageFilePath;




        if (decodeParam.m_pFileData == nullptr)
        {
            std::string packPath = StringConvert::WStringToUTF8(GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if (!pack.Open(packPath.c_str()))return nullptr;
            fileData = pack.LoadFile(StringConvert::WStringToUTF8(GetFilePathInPack(imageFilePath.ToString())).c_str());
        }




        Image_JPEG* pImageJPEG = new Image_JPEG;
        std::shared_ptr<IBitmapImage> pImage(pImageJPEG);

        if (pImageJPEG->LoadImageFile(fileData,
            imageFilePath,
            fImageSizeScale,
            bAsyncDecode,
            rcMaxDestRectSize,
            bAssertEnabled)) {
            return Image_Bitmap::MakeImage(pImage);
        }
        return nullptr;
    }



    //GIF
    //GIF
    //GIF
    //GIF
    //GIF
    //GIF
    //GIF
    //GIF
    //GIF
    //GIF






    bool ImageDecoder_GIF_EX::CanDecode(const DString& imageFilePath) const
    {
        if (GetPackPath(imageFilePath).empty())return false;
        return __super::CanDecode(imageFilePath);
    }


    std::unique_ptr<IImage> ImageDecoder_GIF_EX::LoadImageData(const ImageDecodeParam& decodeParam)
    {
        bool bLoadAllFrames = decodeParam.m_bLoadAllFrames;
        bool bAsyncDecode = decodeParam.m_bAsyncDecode;
        float fImageSizeScale = decodeParam.m_fImageSizeScale;
        const UiSize& rcMaxDestRectSize = decodeParam.m_rcMaxDestRectSize;
        bool bAssertEnabled = decodeParam.m_bAssertEnabled;
        std::vector<uint8_t> emptyFileData;
        std::vector<uint8_t>& fileData = (decodeParam.m_pFileData != nullptr) ? *decodeParam.m_pFileData : emptyFileData;
        const FilePath& imageFilePath = decodeParam.m_imageFilePath;



        if (decodeParam.m_pFileData == nullptr)
        {
            std::string packPath = StringConvert::WStringToUTF8(GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if (!pack.Open(packPath.c_str()))return nullptr;
            fileData = pack.LoadFile(StringConvert::WStringToUTF8(GetFilePathInPack(imageFilePath.ToString())).c_str());
        }



        Image_GIF* pImageGIF = new Image_GIF;
        std::shared_ptr<IAnimationImage> pAnimationImage(pImageGIF);

        if (!pImageGIF->LoadImageFile(fileData,
            imageFilePath,
            bLoadAllFrames,
            bAsyncDecode,
            fImageSizeScale,
            rcMaxDestRectSize,
            bAssertEnabled)) {
            return nullptr;
        }
        if (!bLoadAllFrames || (pImageGIF->GetFrameCount() == 1)) {
            //单帧，加载位图图片
            return Image_Bitmap::MakeImage(pAnimationImage);
        }
        else {
            //多帧图片
            std::unique_ptr<IImage> pImage(new Image_Animation(pAnimationImage));
            return pImage;
        }
    }













}