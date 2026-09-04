//对duilib进行修改以实现richtext防止截断单词的支持



// 判断 Unicode 码点是否属于 CJK 字符（中日韩统一表意文字）
static inline bool IsCJKCodePoint(uint32_t cp) {
    return (cp >= 0x4E00 && cp <= 0x9FFF) ||  // CJK 统一表意文字
        (cp >= 0x3400 && cp <= 0x4DBF) ||  // CJK 扩展 A
        (cp >= 0x20000 && cp <= 0x2A6DF) ||  // CJK 扩展 B
        (cp >= 0x2A700 && cp <= 0x2B73F) ||  // CJK 扩展 C
        (cp >= 0x2B740 && cp <= 0x2B81F) ||  // CJK 扩展 D
        (cp >= 0x2B820 && cp <= 0x2CEAF) ||  // CJK 扩展 E
        (cp >= 0xF900 && cp <= 0xFAFF) ||  // CJK 兼容表意文字
        (cp >= 0x2F800 && cp <= 0x2FA1F) ||  // CJK 兼容补充
        (cp >= 0x3000 && cp <= 0x303F) ||  // CJK 符号和标点
        (cp >= 0xFF00 && cp <= 0xFFEF);      // 全角 ASCII、全角标点
}

static inline bool IsBreakSafeCodePoint(uint32_t cp) {
    return cp == L' ' || cp == L'\t' ||  // 空格、制表符
        cp == L'-' ||                  // 连字符
        cp == L',' || cp == L'.' ||   // 英文标点
        cp == L';' || cp == L':' ||
        cp == L'!' || cp == L'?' ||
        cp == L')' || cp == L']' ||
        cp == L'}' ||                  // 闭合括号类
        IsCJKCodePoint(cp);             // 中文字符后面可以断行
}


static size_t BreakTextAtWordBoundary(const void* text, size_t byteLength,
    SkTextEncoding textEncoding,
    const SkFont& font, const SkPaint& paint,
    SkScalar maxWidth,
    SkScalar* measuredWidth, SkScalar* measuredHeight,
    std::vector<SkGlyphID>& glyphs,
    std::vector<uint8_t>& glyphChars,
    std::vector<SkScalar>& glyphWidths,
    std::vector<uint8_t>* glyphCharList,
    std::vector<SkScalar>* glyphWidthList) {

    // 1. 先调用原始 breakText，获取按像素截断的结果
    size_t rawBreakLength = SkTextBox::breakText(
        text, byteLength, textEncoding, font, paint, maxWidth,
        measuredWidth, measuredHeight,
        glyphs, glyphChars, glyphWidths,
        nullptr, nullptr);

    // 整段都放得下，或完全放不下，直接返回
    if (rawBreakLength == 0 || rawBreakLength == byteLength) {
        return rawBreakLength;
    }

    // 2. 根据编码方式，获取截断位置处的字符和前一字符的 Unicode 码点
    //    这里以 UTF-16 为例，如果是 UTF-8 或 UTF-32 需要相应调整
    const uint16_t* u16Text = static_cast<const uint16_t*>(text);
    size_t totalChars = byteLength / sizeof(uint16_t);
    size_t breakCharIndex = rawBreakLength / sizeof(uint16_t);

    if (breakCharIndex == 0 || breakCharIndex >= totalChars) {
        return rawBreakLength;
    }

    uint32_t charAtBreak = u16Text[breakCharIndex];       // 截断位置的字符
    uint32_t charBeforeBreak = u16Text[breakCharIndex - 1]; // 截断位置前一个字符

    // 3. 情况A：截断位置在 CJK 字符之后 → 可以直接断行
    if (IsCJKCodePoint(charBeforeBreak)) {
        return SkTextBox::breakText(text, rawBreakLength, textEncoding,
            font, paint, maxWidth,
            measuredWidth, measuredHeight,
            glyphs, glyphChars, glyphWidths,
            glyphCharList, glyphWidthList);
    }

    // 4. 情况B：截断位置在空格等安全字符处 → 也可以直接断行
    if (IsBreakSafeCodePoint(charAtBreak) || IsBreakSafeCodePoint(charBeforeBreak)) {
        return SkTextBox::breakText(text, rawBreakLength, textEncoding,
            font, paint, maxWidth,
            measuredWidth, measuredHeight,
            glyphs, glyphChars, glyphWidths,
            glyphCharList, glyphWidthList);
    }

    // 5. 情况C：截断位置在英文单词中间 → 需要向前找空格
    for (size_t i = breakCharIndex; i > 0; --i) {
        uint32_t ch = u16Text[i - 1];

        if (ch == L' ' || ch == L'\t') {
            // 找到空格，在空格之后断行（包含空格）
            size_t newBreakLength = i * sizeof(uint16_t);

            SkScalar tempWidth = 0;
            SkTextBox::breakText(text, newBreakLength, textEncoding,
                font, paint, maxWidth,
                &tempWidth, nullptr,
                glyphs, glyphChars, glyphWidths,
                nullptr, nullptr);

            if (tempWidth <= maxWidth) {
                return SkTextBox::breakText(text, newBreakLength, textEncoding,
                    font, paint, maxWidth,
                    measuredWidth, measuredHeight,
                    glyphs, glyphChars, glyphWidths,
                    glyphCharList, glyphWidthList);
            }
            break;
        }

        // 如果遇到 CJK 字符，说明前面是中文，可以在这里断行
        if (IsCJKCodePoint(ch)) {
            size_t newBreakLength = i * sizeof(uint16_t);
            return SkTextBox::breakText(text, newBreakLength, textEncoding,
                font, paint, maxWidth,
                measuredWidth, measuredHeight,
                glyphs, glyphChars, glyphWidths,
                glyphCharList, glyphWidthList);
        }
    }

    // 6. 情况D：向前找不到任何断点（超长英文单词）
    //    策略：整个单词移到下一行，本次绘制长度为 0
    if (measuredWidth) *measuredWidth = 0;
    if (measuredHeight) *measuredHeight = 0;
    glyphs.clear();
    glyphChars.clear();
    glyphWidths.clear();
    if (glyphCharList) glyphCharList->clear();
    if (glyphWidthList) glyphWidthList->clear();
    return 0;
}







void DrawRichText::InternalDrawRichText(const UiRect& rcTextRect,
    const UiSize& szScrollOffset,
    IRenderFactory* pRenderFactory,
    const std::vector<RichTextData>& richTextData,
    uint8_t uFade,
    bool bMeasureOnly,
    RichTextLineInfoParam* pLineInfoParam,
    std::shared_ptr<DrawRichTextCache>* pDrawRichTextCache,
    std::vector<std::vector<UiRect>>* pRichTextRects)
{
    PerformanceStat statPerformance(_T("DrawRichText::InternalDrawRichText"));
    ASSERT((m_pRender != nullptr) && (m_pSkCanvas != nullptr) && (m_pSkPaint != nullptr) && (m_pSkPointOrg != nullptr));
    if ((m_pRender == nullptr) || (m_pSkCanvas == nullptr) || (m_pSkPaint == nullptr) || (m_pSkPointOrg == nullptr)) {
        return;
    }

    //内部使用string_view实现，避免字符串复制影响性能
    if (rcTextRect.IsEmpty()) {
        return;
    }
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return;
    }

    //绘制区域：绘制区域的坐标以 (rcTextRect.left,rcTextRect.top)作为(0,0)点
    UiRect rcDrawRect = rcTextRect;
    rcDrawRect.Offset(-szScrollOffset.cx, -szScrollOffset.cy);

    if ((pLineInfoParam != nullptr) || (pDrawRichTextCache != nullptr)) {
        ASSERT(bMeasureOnly);
        if (!bMeasureOnly) {
            return;
        }
        //使用(0,0)坐标点，作为估算的(0,0)点
        rcDrawRect.Offset(-rcDrawRect.left, -rcDrawRect.top);
        ASSERT(rcDrawRect.left == 0);
        ASSERT(rcDrawRect.top == 0);
    }

    //文本编码：固定为UTF16 或者 UTF32
    constexpr const SkTextEncoding textEncoding = (sizeof(DStringW::value_type) == sizeof(uint32_t)) ? SkTextEncoding::kUTF32 : SkTextEncoding::kUTF16;
    constexpr const size_t textCharSize = sizeof(DStringW::value_type);

    //当绘制超过目标矩形边界时，是否继续绘制
    const bool bBreakWhenOutOfRect = !bMeasureOnly && (pDrawRichTextCache == nullptr);

    std::vector<SharePtr<TPendingDrawRichText>> pendingTextData;
    pendingTextData.reserve(richTextData.size());

    const int32_t nTextRectRightMax = (int32_t)rcTextRect.right;   //绘制区域的最右侧
    const int32_t nTextRectBottomMax = (int32_t)rcTextRect.bottom; //绘制区域的最底端

    SkScalar xPos = (SkScalar)rcDrawRect.left;  //水平坐标：字符绘制的时候，是按浮点型坐标，每个字符所占的宽度是浮点型的，不能对齐到像素
    int32_t yPos = rcDrawRect.top;              //垂直坐标，对齐到像素，所以用整型
    int32_t nRowHeight = 0;   //行高（本行中，所有字符绘制高度的最大值，对齐到像素）
    uint32_t nLineNumber = 0; //物理行号
    uint32_t nRowIndex = 0;   //逻辑行号

    std::unordered_map<uint32_t, uint32_t> rowHeightMap;  //每行的实际行高表

    //字体缓存(由于创建字体比较耗时，所以尽量复用相同的对象)
    SharePtr<UiFontEx> lastFont;
    std::shared_ptr<IFont> spLastSkiaFont;

    if (pLineInfoParam != nullptr) {
        //设置起始行号
        nLineNumber = (uint32_t)pLineInfoParam->m_nStartLineIndex;
        ASSERT(pLineInfoParam->m_pLineInfoList != nullptr);
        if (pLineInfoParam->m_pLineInfoList == nullptr) {
            return;
        }
        ASSERT(nLineNumber < pLineInfoParam->m_pLineInfoList->size());
        if (nLineNumber >= pLineInfoParam->m_pLineInfoList->size()) {
            return;
        }
        //起始的逻辑行号
        nRowIndex = pLineInfoParam->m_nStartRowIndex;
    }

    //绘制属性
    SkPaint skPaint = *m_pSkPaint;
    if (uFade != 0xFF) {
        //透明度
        skPaint.setAlpha(uFade);
    }
    UiColor textColor;

    std::vector<SkGlyphID> glyphs;      //内部临时变量，为提升执行速度，在外部声明变量
    std::vector<uint8_t> glyphChars;    //内部临时变量，为提升执行速度，在外部声明变量
    std::vector<SkScalar> glyphWidths;  //内部临时变量，为提升执行速度，在外部声明变量

    std::vector<uint8_t> glyphCharList;   //每个字由几个字符构成
    std::vector<SkScalar> glyphWidthList; //每个字符的宽度

    //按换行符进行文本切分
    std::vector<std::wstring_view> lineTextViewList;

    //分行时文本切分的内部临时变量，为提升执行速度，在外部声明变量
    std::vector<uint32_t> lineSeprators;

    //是否正在绘制TAB键（按4个字符对齐）
    bool bDrawTabChar = false;

    //本行（逻辑行）已经绘制了多少个字符（不含回车和换行）
    size_t nRowCharCount = 0;

    for (size_t index = 0; index < richTextData.size(); ++index) {
        const RichTextData& textData = richTextData[index];
        if (textData.m_textView.empty()) {
            continue;
        }

        //设置文本颜色
        if (textColor != textData.m_textColor) {
            const UiColor& color = textData.m_textColor;
            skPaint.setARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB());
            textColor = textData.m_textColor;
        }

        std::shared_ptr<IFont> spSkiaFont;
        if ((spLastSkiaFont != nullptr) &&
            (textData.m_pFontInfo != nullptr) && (lastFont != nullptr) &&
            ((textData.m_pFontInfo == lastFont) || (*textData.m_pFontInfo == *lastFont))) {
            //复用缓存中的字体对象
            spSkiaFont = spLastSkiaFont;
        }
        else {
            spSkiaFont.reset(pRenderFactory->CreateIFont());
            ASSERT(spSkiaFont != nullptr);
            if (spSkiaFont == nullptr) {
                continue;
            }
            ASSERT(textData.m_pFontInfo != nullptr);
            if (textData.m_pFontInfo == nullptr) {
                continue;
            }
            if (!spSkiaFont->InitFont(*textData.m_pFontInfo)) {
                spSkiaFont.reset();
                continue;
            }
        }

        Font_Skia* pSkiaFont = dynamic_cast<Font_Skia*>(spSkiaFont.get());
        ASSERT(pSkiaFont != nullptr);
        if (pSkiaFont == nullptr) {
            continue;
        }
        const SkFont* pSkFont = pSkiaFont->GetFontHandle();
        ASSERT(pSkFont != nullptr);
        if (pSkFont == nullptr) {
            continue;
        }

        if (spLastSkiaFont != spSkiaFont) {
            spLastSkiaFont = spSkiaFont;
            lastFont = textData.m_pFontInfo;
        }

        const SkFont& skFont = *pSkFont;
        SkFontMetrics metrics;
        SkScalar fFontHeight = skFont.getMetrics(&metrics);     //字体高度，换行时使用
        fFontHeight *= textData.m_fRowSpacingMul;               //运用行间距
        const int32_t nFontHeight = SkScalarCeilToInt(fFontHeight);   //行高对齐到像素
        nRowHeight = std::max(nRowHeight, nFontHeight);
        if (nRowHeight <= 0) {
            continue;
        }
        const uint32_t uTextStyle = textData.m_textStyle;
        const bool bSingleLineMode = (uTextStyle & DrawStringFormat::TEXT_SINGLELINE) ? true : false; //是否为单行模式，单行模式下，不换行
        const bool bWordWrap = bSingleLineMode ? false : ((uTextStyle & DrawStringFormat::TEXT_WORD_WRAP) ? true : false);
        bool bBreakAll = false;//标记是否终止

        //按换行符进行文本切分
        lineTextViewList.clear();
        SplitLines(textData.m_textView, lineSeprators, lineTextViewList);

        //物理行内的逻辑行号(每个物理行中，从0开始编号)
        uint32_t nLineTextRowIndex = 0;

        for (const std::wstring_view& lineTextView : lineTextViewList) {
            bDrawTabChar = false;
            if (lineTextView.size() == 1) {
                if (lineTextView[0] == L'\r') {
                    //处理回车
                    if (pLineInfoParam != nullptr) {
                        OnDrawUnicodeChar(pLineInfoParam, lineTextView[0], 1, 2, nLineNumber, nLineTextRowIndex, xPos, yPos, 0, nRowHeight);
                    }
                    continue; //忽略回车
                }
                if (lineTextView[0] == L'\t') {
                    //处理TAB键
                    bDrawTabChar = true;
                }
                else if (lineTextView[0] == L'\n') {
                    //处理换行符
                    if (pLineInfoParam != nullptr) {
                        OnDrawUnicodeChar(pLineInfoParam, lineTextView[0], 1, 2, nLineNumber, nLineTextRowIndex, xPos, yPos, 0, nRowHeight);
                    }

                    //换行：执行换行操作(物理换行)
                    if (!bSingleLineMode) {
                        xPos = (SkScalar)rcDrawRect.left;
                        ASSERT(((int64_t)yPos + (int64_t)nRowHeight) < INT32_MAX);
                        yPos += nRowHeight;
                        rowHeightMap[nRowIndex] = nRowHeight;
                        nRowHeight = nFontHeight;
                        ++nRowIndex;
                        nRowCharCount = 0;
                        ++nLineTextRowIndex;
                        ++nLineNumber;
                    }
                    continue; //处理下一行
                }
            }

            //绘制的文本下标开始值
            const size_t textCount = lineTextView.size();
            size_t textStartIndex = 0;
            while (textStartIndex < textCount) {
                //估算文本绘制区域                
                size_t byteLength = (textCount - textStartIndex) * textCharSize;
                SkScalar maxWidth = SkIntToScalar(rcDrawRect.right) - xPos;//可用宽度
                if (!bWordWrap || bSingleLineMode) {
                    //不自动换行 或者 单行模式
                    maxWidth = SK_FloatInfinity;
                }
                ASSERT(maxWidth > 0);
                SkScalar textMeasuredWidth = 0;  //当前要绘制的文本，估算的所需宽度
                SkScalar textMeasuredHeight = 0; //当前要绘制的文本，估算的所需高度

                glyphCharList.clear();
                glyphWidthList.clear();

                //评估每个字符的矩形范围
                std::vector<uint8_t>* pGlyphCharList = &glyphCharList;
                std::vector<SkScalar>* pGlyphWidthList = &glyphWidthList;

                size_t nDrawLength = 0;
                if (bDrawTabChar) {
                    ASSERT(textCount == 1);
                    //绘制TAB键, 按4个字符对齐
                    const DStringW blank = L"    ";
                    size_t nBlankCount = nRowCharCount % blank.size();
                    nBlankCount = blank.size() - nBlankCount;
                    //nDrawLength = SkTextBox::breakText(blank.c_str(),
                    nDrawLength = BreakTextAtWordBoundary(blank.c_str(),
                        nBlankCount * sizeof(DStringW::value_type), textEncoding,
                        skFont, skPaint,
                        maxWidth, &textMeasuredWidth, &textMeasuredHeight,
                        glyphs, glyphChars, glyphWidths,
                        pGlyphCharList, pGlyphWidthList);
                    if (nDrawLength > 0) {
                        nDrawLength = textCount * sizeof(DStringW::value_type);
                        if (glyphs.empty()) {
                            glyphs.resize(1);
                            glyphChars.resize(1);
                            glyphChars[0] = 1;
                            glyphWidths.resize(1, textMeasuredWidth);
                        }
                        pGlyphCharList->resize(1);
                        (*pGlyphCharList)[0] = 1;
                        pGlyphWidthList->resize(1);
                        (*pGlyphWidthList)[0] = textMeasuredWidth;
                    }
                }
                else {
                    //breakText函数执行时间占比约30%
                    nDrawLength = BreakTextAtWordBoundary(lineTextView.data() + textStartIndex,
                        byteLength, textEncoding,
                        skFont, skPaint,
                        maxWidth, &textMeasuredWidth, &textMeasuredHeight,
                        glyphs, glyphChars, glyphWidths,
                        pGlyphCharList, pGlyphWidthList);
                }

                if (nDrawLength == 0) {
                    if (!bWordWrap || bSingleLineMode || (SkScalarTruncToInt(maxWidth) == rcDrawRect.Width())) {
                        //出错了(不能换行，或者换行后依然不够)
                        bBreakAll = true;
                        break;
                    }
                }
                else {
                    SharePtr<TPendingDrawRichText> spTextData(new TPendingDrawRichText);
                    spTextData->m_nDataIndex = (uint32_t)index;
                    spTextData->m_nLineNumber = nLineNumber;
                    spTextData->m_nRowIndex = nRowIndex;
                    spTextData->m_textView = std::wstring_view(lineTextView.data() + textStartIndex, nDrawLength / textCharSize);
                    spTextData->m_spFont = spSkiaFont;

                    spTextData->m_bgColor = textData.m_bgColor;
                    spTextData->m_textColor = textData.m_textColor;
                    spTextData->m_textStyle = textData.m_textStyle;

                    //绘制文字所需的矩形区域
                    spTextData->m_destRect.left = SkScalarTruncToInt(xPos); //左值：直接截断，如果有小数部分，直接去掉小数即可

                    SkScalar fRight = xPos + textMeasuredWidth;             //右值：如果有小数，则需要增加1个像素
                    spTextData->m_destRect.right = SkScalarCeilToInt(fRight);
                    spTextData->m_destRect.top = yPos;
                    spTextData->m_destRect.bottom = yPos + SkScalarCeilToInt(textMeasuredHeight); //记录字符的真实高度
                    pendingTextData.emplace_back(std::move(spTextData));

                    if (pLineInfoParam != nullptr) {
                        //评估每个字符的矩形范围
                        ASSERT(!glyphCharList.empty());
                        ASSERT(glyphCharList.size() == glyphWidthList.size());
                        if (glyphCharList.size() == glyphWidthList.size()) {
                            const size_t glyphCount = glyphCharList.size();
                            SkScalar glyphWidth = 0;
                            uint8_t glyphCharCount = 0;
                            SkScalar glyphLeft = (SkScalar)SkScalarTruncToInt(xPos);
                            for (size_t glyphIndex = 0; glyphIndex < glyphCount; ++glyphIndex) {
                                glyphWidth = glyphWidthList[glyphIndex];//字符宽度
                                glyphCharCount = glyphCharList[glyphIndex];  //该字占几个字符（UTF16编码，可能是1或者2）
                                ASSERT((glyphCharCount == 1) || (glyphCharCount == 2));
                                OnDrawUnicodeChar(pLineInfoParam, 0, glyphCharCount, glyphCount, nLineNumber, nLineTextRowIndex, glyphLeft, yPos, glyphWidth, nRowHeight);
                                glyphLeft += glyphWidth;
                            }
                        }
                    }

                    //统计本逻辑行已经绘制了多少个字符
                    nRowCharCount += glyphs.size();
                }

                bool bNextRow = false; //是否需要换行的标志
                if (nDrawLength < byteLength) {
                    //宽度不足，需要换行
                    if (!bSingleLineMode) {
                        bNextRow = true;
                    }
                    textStartIndex += nDrawLength / textCharSize;
                    ASSERT(((int64_t)xPos + (int64_t)textMeasuredWidth) < INT32_MAX);
                    xPos += textMeasuredWidth;
                }
                else {
                    //当前行可容纳文本绘制
                    textStartIndex = textCount;//标记，结束循环
                    ASSERT(((int64_t)xPos + (int64_t)textMeasuredWidth) < INT32_MAX);
                    xPos += textMeasuredWidth;
                    if (xPos >= nTextRectRightMax) {
                        //X坐标右侧已经超出目标矩形的范围
                        if (bBreakWhenOutOfRect && bSingleLineMode) {
                            //单行模式，终止绘制
                            bBreakAll = true;
                            break;
                        }
                        else if (bWordWrap && !bSingleLineMode) {
                            //在自动换行的情况下，换行
                            bNextRow = true;
                        }
                    }
                }

                if (bNextRow) {
                    //换行：执行换行操作（逻辑换行，对nLineNumber不增加）
                    xPos = (SkScalar)rcDrawRect.left;
                    ASSERT(((int64_t)yPos + (int64_t)nRowHeight) < INT32_MAX);
                    yPos += nRowHeight;
                    rowHeightMap[nRowIndex] = nRowHeight;
                    nRowHeight = nFontHeight;
                    ++nRowIndex;
                    nRowCharCount = 0;
                    ++nLineTextRowIndex;

                    if (bBreakWhenOutOfRect && (yPos >= nTextRectBottomMax)) {
                        //Y坐标底部已经超出目标矩形的范围，终止绘制
                        bBreakAll = true;
                        break;
                    }
                }
            }
            if (bBreakAll) {
                break;
            }
        }
        if (bBreakAll) {
            break;
        }
    }

    //记录最后一行的行高
    rowHeightMap[nRowIndex] = nRowHeight;

    //更新每行的行高(只有提前确定行高，才能正确绘制纵向对齐的文本)
    for (const SharePtr<TPendingDrawRichText>& spTextData : pendingTextData) {
        TPendingDrawRichText& textData = *spTextData;
        auto iter = rowHeightMap.find(textData.m_nRowIndex);
        ASSERT(iter != rowHeightMap.end());
        if (iter != rowHeightMap.end()) {
            textData.m_destRect.bottom = textData.m_destRect.top + iter->second;
        }
    }

    if (pRichTextRects != nullptr) {
        pRichTextRects->clear();
        pRichTextRects->resize(richTextData.size());
        for (const SharePtr<TPendingDrawRichText>& spTextData : pendingTextData) {
            const TPendingDrawRichText& textData = *spTextData;
            //保存绘制的目标区域，同一个文本，可能会有多个区域（换行时）
            ASSERT(textData.m_nDataIndex < pRichTextRects->size());
            std::vector<UiRect>& textRects = (*pRichTextRects)[textData.m_nDataIndex];
            textRects.push_back(textData.m_destRect);
        }
    }

    if (pDrawRichTextCache != nullptr) {
        //生成绘制缓存，但不执行绘制
        std::shared_ptr<DrawRichTextCache> spDrawRichTextCache = std::make_shared<DrawRichTextCache>();
        *pDrawRichTextCache = spDrawRichTextCache;
        spDrawRichTextCache->m_richTextData = richTextData;
        spDrawRichTextCache->m_textRect = rcTextRect;

        spDrawRichTextCache->m_textEncoding = textEncoding;
        spDrawRichTextCache->m_textCharSize = textCharSize;

        spDrawRichTextCache->m_pendingTextData.swap(pendingTextData);
    }
    else if (!bMeasureOnly) {
        UiRect rcTemp;
        for (const SharePtr<TPendingDrawRichText>& spTextData : pendingTextData) {
            const TPendingDrawRichText& textData = *spTextData;
            //执行绘制            
            const UiRect& rcDestRect = textData.m_destRect;
            if (!UiRect::Intersect(rcTemp, rcDestRect, rcTextRect)) {
                continue;
            }

            //绘制文字的背景色
            m_pRender->FillRect(rcDestRect, textData.m_bgColor, uFade);

            if (textColor != textData.m_textColor) {
                const UiColor& color = textData.m_textColor;
                skPaint.setARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB());
                textColor = textData.m_textColor;
            }

            //绘制文字
            if ((textData.m_textView.size() == 1) && (textData.m_textView[0] == L'\t')) {
                //绘制TAB键
                const char* text = (const char*)" ";
                const size_t len = 1; //字节数
                DrawTextString(rcDestRect, text, len, SkTextEncoding::kUTF8,
                    textData.m_textStyle | DrawStringFormat::TEXT_SINGLELINE,
                    skPaint, textData.m_spFont.get());
            }
            else {
                const char* text = (const char*)textData.m_textView.data();
                const size_t len = textData.m_textView.size() * textCharSize; //字节数
                DrawTextString(rcDestRect, text, len, textEncoding,
                    textData.m_textStyle | DrawStringFormat::TEXT_SINGLELINE,
                    skPaint, textData.m_spFont.get());
            }
        }
    }
}
