#include"Dui.h"
#include"SettingsPage.h"


void SettingsPage_sound::InitContents(class SettingsPage* parent)
{
    //音频
    {
        double volumevalue = AppSettings::GetIns().GetVolumeValue();
        slider_volume = static_cast<ui::Slider*>(parent->FindSubControl(L"slider_volume"));
        slider_volume->SetValue(volumevalue * 100.);
        slider_volume->AttachValueChanged(std::bind(&SettingsPage_sound::OnSoundSliderValueChanged, this, std::placeholders::_1));
    
        text_volume= static_cast<ui::Label*>(parent->FindSubControl(L"text_volume"));

        text_volume->SetText(std::to_wstring(static_cast<int>(SDL_round(volumevalue))));

    }
}

bool SettingsPage_sound::OnSoundSliderValueChanged(const ui::EventArgs& msg)
{

    //音频只有一个滑块

    double value = slider_volume->GetValue();
    //100
    AppSettings::GetIns().SetVolumeValue(value / 100.);
    text_volume->SetText(std::to_wstring(static_cast<int>(SDL_round(value / 100.))));
    return true;


}