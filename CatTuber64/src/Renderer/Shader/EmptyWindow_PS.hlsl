
cbuffer CBuffer : register(b0, space3)
{
    float windowW;
    float windowH;
    float offsetX;
    float offsetY;
    
    float unitWidth;
    float _Density;//密度factor，与坐标相乘得到网格id
    
    float cornerRadius;
    
    float padding;
    
    
    float4 color1;
    float4 color2;
}




struct VSOut
{
    float2 uv : TexCoord;
    float4 pos : SV_Position;
};

float4 EmptyWindow_PS(VSOut In) : SV_TARGET
{
    //圆角计算
    float2 curPos_px = float2(In.uv.x * windowW, In.uv.y * windowH);
    
     //用于圆角alpha计算的坐标
    //先获取当前坐标相对于中心的坐标
    //由于对称性，这里使用绝对值
    float2 cornerCalcPos_px = abs(curPos_px - 0.5f * float2(windowW, windowH));
    //获取相对于移除圆角环后的内部矩形的顶点的坐标
    //此时矩形在x为负 y为负的象限
    cornerCalcPos_px = cornerCalcPos_px - float2(0.5f * windowW - cornerRadius, 0.5f * windowH - cornerRadius);
    //eg::
    //        y+
    //        *        *cornerCalcPos_px
    //**********
    //        *  *     
    //        *    *
    //****************** x+
    //        *     *
    //        *     *
    //        *     *
    
    //下面讨论的矩形是移除圆角环后的内部矩形
    //eg::
    //        y+
    //        *        *cornerCalcPos_px
    //**********
    //        *  *     
    //        *    *
    //****************** x+
    //*********     *
    //*********     *
    //*********     *
    
    //max(cornerCalcPos_px, 0.0)截断cornerCalcPos_px的负值
    //如果点在第三象限，结果会为0
    //否则
    float outside = length(max(cornerCalcPos_px, 0.0));
    

    
    
    //在矩形内部时，得到是一个负的距离值，
    //在矩形外部时，因为x y存在正数，所以max会返回正数，然后被min截断为0，
    //这里讨论的是无圆角环带时的内部矩形，在读资料的时候很多资料没强调这点，
    //因为我们开始的目标就是判断点是否在‘圆角’矩形内，
    //在不逐行理解而跳读的情况下很容易误解为圆角矩形，或者外边组成的矩形。
    //float inside = min(max(cornerCalcPos_px.x, cornerCalcPos_px.y), 0.0);
    
    //所以inside和outside只能有一方为0
    //这是着色器里常见的为了减少分支而两头考虑的写法，正常CPU代码中直接if处理，逻辑会清晰很多
    
    //这里实际不是求的和，因为其中一方必有一个为0，
    //若点在矩形内部，那么outside为0 ，dist=inside为负值，必定小于正值的圆角半径
    //若点在矩形外部，那么inside为0 ，dist=outside，
    //计算outside时，坐标已经被max(point, 0)推向第一象限了，
    //因此此时dist可以直接视为，绘制的点到圆角圆心的距离
    //float dist = outside + inside;
    
    
    //alpha抗锯齿
    float alpha = 1.f - clamp(outside - (cornerRadius - 0.5f), 0.f, 1.f);
     
    
    
    //////
    //渲染网格
    //
    curPos_px += float2(offsetX,offsetY);
    float2 gridUV = curPos_px * _Density;
    float2 gridPos = floor(gridUV);
    float checker = fmod(abs(gridPos.x + gridPos.y), 2.0);
    float4 _currentColor = lerp( color1 ,color2 ,checker);
    float4 _otherColor = lerp(color1, color2, 1-checker);
    
    const float pixHeight = 1.f;
    const float _pixHeight = 1.f;
    //和上面一样，利用绝对值对称性计算坐标然后抗锯齿
    {
        //float2 posInRect = abs(gridUV - gridPos - float2(0.5f, 0.5f)) * unitWidth;
        float2 posInRect = abs(gridUV - gridPos - float2(0.5f, 0.5f)) * unitWidth;
        
        //posInRect = posInRect-float2(0.5f * unitWidth - 0.5f, 0.5f * unitWidth - 0.5f) ;
        posInRect = posInRect - float2(0.5f * unitWidth - 0.5*pixHeight, 0.5f * unitWidth - 0.5*pixHeight);
        //posInRect = float2(0.5f * unitWidth, 0.5f * unitWidth) - posInRect;
        
 ////
        //              ^Y
        //              |
        //      side    |  corner
        //--------------|-|0.5px
        //--------------*------>x
        //              | |
        //              | | side
        //   inside     | |
        //              | |
        
        
 ////
        //                ^Y
        //                |
        //      side      |  corner
        //--------------|-|---->x
        //-----------------
        //              | |
        //              | | side
        //   inside     | |
        //              | |
        
        
        //使在inside的时候其他区域为0
        float inside = 1.0f - step(0.0f, min(max(posInRect.x, posInRect.y), 0.0f));
        
        float cornerFlag = step(0.0, min(posInRect.x, posInRect.y));
        
        float sideFlag = 1.f - inside - cornerFlag;
        
        //混合颜色
    //    {
    //// uv.x = u, uv.y = v
    //        float4 C0 = lerp(C00, C10, uv.x);
    //        float4 C1 = lerp(C01, C11, uv.x);
    //        return lerp(C0, C1, uv.y);
    //    }
        
 
        
        float4 calColor = inside * _currentColor +
        cornerFlag * lerp(lerp(_currentColor, _otherColor, posInRect.x * _pixHeight), lerp(_otherColor, _currentColor, posInRect.x * _pixHeight), posInRect.y * _pixHeight) +
        sideFlag * lerp(_currentColor, _otherColor, max(posInRect.x, posInRect.y) * _pixHeight);
        
        //calColor = sideFlag * lerp(_currentColor, _otherColor, max(posInRect.x, posInRect.y) / pixHeight);
        
        
        _currentColor = calColor;
        
        
    }
    
    
    
    return alpha * _currentColor;
}