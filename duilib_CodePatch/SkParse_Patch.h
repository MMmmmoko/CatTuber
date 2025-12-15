#pragma once
//这段代码仅作为提醒使用
//修改的是SkParse.h和SkParseColor.cpp文件以让skia能寻找duilib中定义的颜色




//SkParse.h
#ifndef SkParse_DEFINED
#define SkParse_DEFINED

#include "include/core/SkColor.h"
#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"

#include <cstddef>
#include <cstdint>

class SK_API SkParse {
public:
    static int Count(const char str[]); // number of scalars or int values
    static int Count(const char str[], char separator);
    static const char* FindColor(const char str[], SkColor* value);
    static const char* FindHex(const char str[], uint32_t* value);
    static const char* FindNamedColor(const char str[], size_t len, SkColor* color);
    static const char* FindS32(const char str[], int32_t* value);
    static const char* FindScalar(const char str[], SkScalar* value);
    static const char* FindScalars(const char str[], SkScalar value[], int count);

    static bool FindBool(const char str[], bool* value);
    // return the index of str in list[], or -1 if not found
    static int  FindList(const char str[], const char list[]);


    //
    //
    //
    //
    //
    //修改部分
    static void SetColorFindFullback(const char* (*FullbackFunc)(const char str[], SkColor* value));
    //
    //
    //
    //
    //
};

#endif




//SkParse.cpp

static const char* (*colorFullbackFunc)(const char* str, size_t len, SkColor* value) = nullptr;
void SkParse::SetColorFindFullback(const char* (*FullbackFunc)(const char* str,
    size_t len,
    SkColor* value)) {
    colorFullbackFunc = FullbackFunc;
}

const char* SkParse::FindNamedColor(const char* name, size_t len, SkColor* color) {
    const auto rec = std::lower_bound(std::begin(gColorNames),
        std::end(gColorNames),
        name, // key
        [](const char* name, const char* key) {
            return strcmp(name, key) < 0;
        });

    if (rec == std::end(gColorNames) || 0 != strcmp(name, *rec)) {


        if (colorFullbackFunc) {
            return colorFullbackFunc(name, len, color);
        }
        //return nullptr;
    }

    if (color) {
        int index = rec - gColorNames;
        *color = SkColorSetRGB(gColors[index].r, gColors[index].g, gColors[index].b);
    }

    return name + strlen(*rec);
}