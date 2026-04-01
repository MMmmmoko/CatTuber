#include "ImageDecoder_EX.h"


#include "duilib/Core/GlobalManager.h"
#include "duilib/Image/Image_Bitmap.h"
#include "duilib/Image/Image_PNG.h"
#include "duilib/Image/Image_Animation.h"
#include "duilib/Image/Image_GIF.h"



#include "duilib/Utils/FilePathUtil.h"

#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/AttributeUtil.h"

#include"Pack/Pack.h"


namespace ui
{


    struct UIImageLoadAttr
    {
        float fadeH[4] = {0,0,1000,1000};//范围0~1  字符串为0~100整数
        int32_t destSizeW=260;
        int32_t destSizeH=40;
        float centerPos = 0.5f;//范围0~1
    };
    
    static bool ParseImageLoadAttr(const DString& strImageLoadString, UIImageLoadAttr* pAttr)
    {
        if (strImageLoadString.find(_T(':')) == DString::npos) {
            //不含有等号，说明没有属性，直接返回
            return false;
        }
        std::wstring str = strImageLoadString;
        for (auto& x : str)
        {
            if (x == L':')x = L'=';
        }

        std::vector<std::pair<DString, DString>> attributeList;
        AttributeUtil::ParseAttributeList(str, _T('\"'), attributeList);



        for (const auto& attribute : attributeList) {
            const DString& name = attribute.first;
            const DString& value = attribute.second;
            if (name.empty() || value.empty()) {
                continue;
            }
            if (name == _T("fadeH"))  {
                //图片资源文件名，根据此设置去加载图片资源
                UiMargin margin;
                AttributeUtil::ParseMarginValue(value.c_str(), margin);
                pAttr->fadeH[0] = static_cast<float>(margin.left)/1000.f;
                pAttr->fadeH[1] = static_cast<float>(margin.top) / 1000.f;
                pAttr->fadeH[2] = static_cast<float>(margin.right) / 1000.f;
                pAttr->fadeH[3] = static_cast<float>(margin.bottom) / 1000.f;
            }
            else if (name == _T("destSize"))
            {
                ui::UiSize size;
                AttributeUtil::ParseSizeValue(value.c_str(), size);
                pAttr->destSizeW = size.cx;
                pAttr->destSizeH = size.cy;
            }
            else if (name == _T("centerPos"))
            {
              
                pAttr->centerPos=std::wcstof(value.c_str(), nullptr);
            }
            else {
                ASSERT(!"ImageAttribute::ModifyAttribute: fount unknown attribute!");
            }
        }


        return true;
    }







    static inline void ModBitmap(UIImageLoadAttr& loadAttr, IBitmap* pBitmap)
    {
        uint8_t* pData = (uint8_t*)pBitmap->LockPixelBits();
        uint32_t w = pBitmap->GetWidth();
        uint32_t h = pBitmap->GetHeight();
        float realMove = loadAttr.centerPos - 0.5f;
        float k, k1, k2;
        if (w * loadAttr.destSizeH > loadAttr.destSizeW * h)
        {
            //图比目标绘制区域还宽，y轴顶满
            realMove = realMove * loadAttr.destSizeW / w;

            k = (float)loadAttr.destSizeW * h / float(w * loadAttr.destSizeW);
            k1 = 0.5f * (1.f - k);
            k2 = k1 + k;
        }
        else
            k = 1.f;
        k1 = 0.f;
        k2 = 1.f;

        int32_t movePixs = static_cast<int32_t>(realMove * w);


        for (uint32_t x = 0; x < w; x++)
        {
            int32_t realX;
            if (movePixs > 0)
                realX = w - 1 - x;
            else
                realX = x;
            int32_t sampleX = realX - movePixs;

            float posInDest = (float)realX / w;
            posInDest = (posInDest - k1) / k;
            float fadeFactor = 1.f;
            if (posInDest < loadAttr.fadeH[0])fadeFactor = 0.f;
            else if (posInDest < loadAttr.fadeH[1])fadeFactor = (posInDest - loadAttr.fadeH[0]) / (loadAttr.fadeH[1] - loadAttr.fadeH[0]);
            else if (posInDest < loadAttr.fadeH[2])fadeFactor = 1.f;
            else if (posInDest < loadAttr.fadeH[3])fadeFactor = 1.f - (posInDest - loadAttr.fadeH[2]) / (loadAttr.fadeH[3] - loadAttr.fadeH[2]);
            else fadeFactor = 0;
            fadeFactor = std::clamp(fadeFactor, 0.f, 1.f);


            for (uint32_t y = 0; y < h; y++)
            {
                uint8_t* curPix = pData + y * w * 4 + realX * 4;
                uint8_t* samplePix = pData + y * w * 4 + sampleX * 4;
                if (sampleX < 0 || sampleX >= (int32_t)w)
                {
                    //采样出界
                    curPix[0] = 0;
                    curPix[1] = 0;
                    curPix[2] = 0;
                    curPix[3] = 0;
                    continue;
                }
                curPix[0] = samplePix[0];//A,BGRA
                curPix[1] = samplePix[1];//A,BGRA
                curPix[2] = samplePix[2];//A,BGRA
                curPix[3] = static_cast<uint8_t>(samplePix[3] * fadeFactor);//A,BGRA

                //curPix[0] = 0;//A,BGRA
                //curPix[1] = 0;//A,BGRA
                //curPix[2] = 255;//A,BGRA
                //curPix[3] = 255;//A,BGRA
            }
        }
        pBitmap->UnLockPixelBits();
    }




    static inline void ModImageData(UIImageLoadAttr& loadAttr, IBitmapImage* pImage)
    {
        bool e;
        ModBitmap(loadAttr,pImage->GetBitmap(&e).get());



    }
    static inline void ModImageData(UIImageLoadAttr& loadAttr, IAnimationImage* pAnimationImage)
    {
        int32_t frameCount = pAnimationImage->GetFrameCount();
        for (int i = 0; i < frameCount; i++)
        {
            ui::IAnimationImage::AnimationFrame frame;
            pAnimationImage->ReadFrameData(i, {}, &frame);
            ModBitmap(loadAttr, frame.m_pBitmap.get());
        }
    }








    //PNG
    //PNG
    //PNG
    //PNG

    bool ui::ImageDecoder_PNG_EX::CanDecode(const DString& imageFilePath) const
    {
        if (Pack::PackPath_GetPackPath(imageFilePath).empty())return false;
        return __super::CanDecode(Pack::PackPath_GetFilePathInPack(imageFilePath));
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
            std::string packPath = StringConvert::WStringToUTF8(Pack::PackPath_GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if(!pack.Open(packPath.c_str()))return nullptr;
            fileData=pack.LoadFile(StringConvert::WStringToUTF8(Pack::PackPath_GetFilePathInPack(imageFilePath.ToString())).c_str());
        }








        //检查是否有参数
        UIImageLoadAttr loadAttr;
		bool hasAttr = ParseImageLoadAttr(Pack::PackPath_GetFileLoadParam(imageFilePath.ToString()), &loadAttr);


        if (hasAttr)bAsyncDecode = false;

        if (!pImagePNG->LoadImageFile(fileData,
            imageFilePath,
            bLoadAllFrames,
            bAsyncDecode,
            fImageSizeScale,
            rcMaxDestRectSize,
            bAssertEnabled)) {
            return nullptr;
        }


        if (hasAttr)
            ModImageData(loadAttr, pAnimationImage.get());


        if (!bLoadAllFrames || (pImagePNG->GetFrameCount() == 1)) {
            //单帧，加载位图图片
			//auto result = Image_Bitmap::MakeImage(pAnimationImage);





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

        if (Pack::PackPath_GetPackPath(imageFilePath).empty())return false;


        return __super::CanDecode(Pack::PackPath_GetFilePathInPack(imageFilePath));
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
            std::string packPath = StringConvert::WStringToUTF8(Pack::PackPath_GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if (!pack.Open(packPath.c_str()))return nullptr;
            fileData = pack.LoadFile(StringConvert::WStringToUTF8(Pack::PackPath_GetFilePathInPack(imageFilePath.ToString())).c_str());
        }




        Image_JPEG* pImageJPEG = new Image_JPEG;
        std::shared_ptr<IBitmapImage> pImage(pImageJPEG);



        //检查是否有参数
        UIImageLoadAttr loadAttr;
        bool hasAttr = ParseImageLoadAttr(Pack::PackPath_GetFileLoadParam(imageFilePath.ToString()), &loadAttr);


        if (hasAttr)bAsyncDecode = false;



        if (pImageJPEG->LoadImageFile(fileData,
            imageFilePath,
            fImageSizeScale,
            bAsyncDecode,
            rcMaxDestRectSize,
            bAssertEnabled)) {

            if (hasAttr)
            {
                ModImageData(loadAttr, pImage.get());
            }


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
        if (Pack::PackPath_GetPackPath(imageFilePath).empty())return false;
        return __super::CanDecode(Pack::PackPath_GetFilePathInPack(imageFilePath));
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
            std::string packPath = StringConvert::WStringToUTF8(Pack::PackPath_GetPackPath(imageFilePath.ToString()));
            Pack pack;
            if (!pack.Open(packPath.c_str()))return nullptr;
            fileData = pack.LoadFile(StringConvert::WStringToUTF8(Pack::PackPath_GetFilePathInPack(imageFilePath.ToString())).c_str());
        }



        Image_GIF* pImageGIF = new Image_GIF;
        std::shared_ptr<IAnimationImage> pAnimationImage(pImageGIF);






        UIImageLoadAttr loadAttr;
        bool hasAttr = ParseImageLoadAttr(Pack::PackPath_GetFileLoadParam(imageFilePath.ToString()), &loadAttr);


        if (hasAttr)bAsyncDecode = false;

        if (!pImageGIF->LoadImageFile(fileData,
            imageFilePath,
            bLoadAllFrames,
            bAsyncDecode,
            fImageSizeScale,
            rcMaxDestRectSize,
            bAssertEnabled)) {
            return nullptr;
        }


        if (hasAttr)
            ModImageData(loadAttr, pAnimationImage.get());



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