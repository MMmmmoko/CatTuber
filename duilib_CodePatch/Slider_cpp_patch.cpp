//由于一些原因，CatTuber必须改动duilib的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。


//这个修改是改善滑块体验

#include "duilib/Control/Slider.h"
#include "duilib/Utils/AttributeUtil.h"



namespace ui {

    void Slider::HandleEvent(const EventArgs& msg)
    {
        if (IsDisabledEvents(msg)) {
            //如果是鼠标键盘消息，并且控件是Disabled的，转发给上层控件
            Box* pParent = GetParent();
            if (pParent != nullptr) {
                pParent->SendEventMsg(msg);
            }
            else {
                BaseClass::HandleEvent(msg);
            }
            return;
        }
        if (msg.eventType == kEventMouseButtonDown || msg.eventType == kEventMouseDoubleClick) {
            UiPoint newPtMouse(msg.ptMouse);
            newPtMouse.Offset(GetScrollOffsetInScrollBox());
            //UiRect rcThumb = GetThumbRect();
            //if (rcThumb.ContainsPt(newPtMouse)) {
            //    SetMouseFocused(true);
            //}
            //无论是否点到滑块均设置鼠标焦点
                SetMouseFocused(true);
            return;
        }
        if (msg.eventType == kEventMouseButtonUp) {
            UiSize szThumb = GetThumbSize();
            double oldValue = GetValue();
            if (IsMouseFocused()) {
                SetMouseFocused(false);
            }
            const int32_t nMin = GetMinValue();
            const int32_t nMax = GetMaxValue();
            if (IsHorizontal()) {
                if (msg.ptMouse.x >= GetRect().right - szThumb.cx / 2) {
                    SetValue(nMax);
                }
                else if (msg.ptMouse.x <= GetRect().left + szThumb.cx / 2) {
                    SetValue(nMin);
                }
                else {
                    double newValue = nMin + double((nMax - nMin) * (msg.ptMouse.x - GetRect().left - szThumb.cx / 2)) / (GetRect().right - GetRect().left - szThumb.cx);
                    SetValue(newValue);
                }
            }
            else {
                if (msg.ptMouse.y >= GetRect().bottom - szThumb.cy / 2) {
                    SetValue(nMin);
                }
                else if (msg.ptMouse.y <= GetRect().top + szThumb.cy / 2) {
                    SetValue(nMax);
                }
                else {
                    double newValue = nMin + double((nMax - nMin) * (GetRect().bottom - msg.ptMouse.y - szThumb.cy / 2)) / (GetRect().bottom - GetRect().top - szThumb.cy);
                    SetValue(newValue);
                }
            }
            SendEvent(kEventValueChanged, (WPARAM)GetValue(), (LPARAM)oldValue);
            Invalidate();
            return;
        }
        if (msg.eventType == kEventMouseWheel) {
            double oldValue = GetValue();
            int32_t detaValue = msg.eventData;
            if (detaValue > 0) {
                SetValue(GetValue() + GetChangeStep());
                SendEvent(kEventValueChanged, (WPARAM)GetValue(), (LPARAM)oldValue);
                return;
            }
            else {
                SetValue(GetValue() - GetChangeStep());
                SendEvent(kEventValueChanged, (WPARAM)GetValue(), (LPARAM)oldValue);
                return;
            }
        }
        if (msg.eventType == kEventMouseMove) {
            UiSize szThumb = GetThumbSize();
            const int32_t nMin = GetMinValue();
            const int32_t nMax = GetMaxValue();
            if (IsMouseFocused()) {
                double oldValue = GetValue();
                if (IsHorizontal()) {
                    if (msg.ptMouse.x >= GetRect().right - szThumb.cx / 2) {
                        SetValue(nMax);
                    }
                    else if (msg.ptMouse.x <= GetRect().left + szThumb.cx / 2) {
                        SetValue(nMin);
                    }
                    else {
                        double newValue = nMin + double((nMax - nMin) * (msg.ptMouse.x - GetRect().left - szThumb.cx / 2)) / (GetRect().right - GetRect().left - szThumb.cx);
                        SetValue(newValue);
                    }
                }
                else {
                    if (msg.ptMouse.y >= GetRect().bottom - szThumb.cy / 2) {
                        SetValue(nMin);
                    }
                    else if (msg.ptMouse.y <= GetRect().top + szThumb.cy / 2) {
                        SetValue(nMax);
                    }
                    else {
                        double newValue = nMin + double((nMax - nMin) * (GetRect().bottom - msg.ptMouse.y - szThumb.cy / 2)) / (GetRect().bottom - GetRect().top - szThumb.cy);
                        SetValue(newValue);
                    }
                }
                SendEvent(kEventValueChanged, (WPARAM)GetValue(), (LPARAM)oldValue);
                Invalidate();
            }
            return;
        }

        Progress::HandleEvent(msg);
    }
}