#ifndef ImageDecoder_EX_H_
#define ImageDecoder_EX_H_

#include "duilib/Image/ImageDecoder.h"
#include "duilib/Image/ImageDecoder_PNG.h"
#include "duilib/Image/ImageDecoder_GIF.h"



#define DUILIB_IMAGE_SUPPORT_JPEG_TURBO 1
#include "duilib/Image/ImageDecoder_JPEG.h"
#include "duilib/Image/Image_JPEG.h"
#undef DUILIB_IMAGE_SUPPORT_JPEG_TURBO
namespace ui
{
    /** PNG/APNG格式的图片解码器接口
    */

    //这里用于解决通过字符串读取内存或者Pack中的图像
    class ImageDecoder_PNG_EX : public ImageDecoder_PNG
    {
    public:

        /** 检查该解码器是否支持给定的文件名
        * @param [in] imageFilePath 实体文件名(比如："File.jpg"，可以带路径), 或者虚拟文件名（比如： "icon:1"）
        */
        //这里识别标识为Pack内图片的路径
        virtual bool CanDecode(const DString& imageFilePath) const override;

        /** 加载解码图片数据，返回解码后的图像数据
        @param [in] decodeParam 图片解码的相关参数
        */
        virtual std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam) override;
    };





//#define DUILIB_IMAGE_SUPPORT_JPEG_TURBO 1
//#ifdef DUILIB_IMAGE_SUPPORT_JPEG_TURBO
    class ImageDecoder_JPG_EX : public ImageDecoder_JPEG
    {
    public:

        /** 检查该解码器是否支持给定的文件名
        * @param [in] imageFilePath 实体文件名(比如："File.jpg"，可以带路径), 或者虚拟文件名（比如： "icon:1"）
        */
        //这里识别标识为Pack内图片的路径
        virtual bool CanDecode(const DString& imageFilePath) const override;

        /** 加载解码图片数据，返回解码后的图像数据
        @param [in] decodeParam 图片解码的相关参数
        */
        virtual std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam) override;
    };
//#endif

    class ImageDecoder_GIF_EX : public ImageDecoder_GIF
    {
    public:

        /** 检查该解码器是否支持给定的文件名
        * @param [in] imageFilePath 实体文件名(比如："File.jpg"，可以带路径), 或者虚拟文件名（比如： "icon:1"）
        */
        //这里识别标识为Pack内图片的路径
        virtual bool CanDecode(const DString& imageFilePath) const override;

        /** 加载解码图片数据，返回解码后的图像数据
        @param [in] decodeParam 图片解码的相关参数
        */
        virtual std::unique_ptr<IImage> LoadImageData(const ImageDecodeParam& decodeParam) override;
    };

} //namespace ui

#endif //UI_IMAGE_IMAGE_DECODER_PNG_H_
