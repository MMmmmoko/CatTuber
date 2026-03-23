#include "duilib/Core/ZipManager.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/ZipStreamIO.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/FilePathUtil.h"

#include "duilib/third_party/zlib/zlib.h"
#include "duilib/third_party/zlib/contrib/minizip/unzip.h"
namespace ui
{
#ifdef DUILIB_BUILD_FOR_WIN

    bool ZipManager::OpenResZip(HMODULE hModule, LPCTSTR resourceName, LPCTSTR resourceType, const DString& password)
    {
        HRSRC rsc = ::FindResource(hModule, resourceName, resourceType);
        ASSERT(rsc != nullptr);
        if (rsc == nullptr) {
            return false;
        }
        uint8_t* pData = (uint8_t*)::LoadResource(hModule, rsc);
        uint32_t nDataSize = ::SizeofResource(hModule, rsc);
        ASSERT((pData != nullptr) && (nDataSize > 0));
        if ((pData == nullptr) || (nDataSize == 0)) {
            return false;
        }
        CloseResZip();
        m_password = password;
        m_pZipStreamIO = std::make_unique<ZipStreamIO>(pData, nDataSize);
        zlib_filefunc_def pzlib_filefunc_def;
        m_pZipStreamIO->FillFopenFileFunc(&pzlib_filefunc_def);
        m_hzip = ::unzOpen2(nullptr, &pzlib_filefunc_def);
        return m_hzip != nullptr;
    }
#endif
}