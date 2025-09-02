#ifndef _InputParser_h
#define _InputParser_h
#include<SDL3/SDL.h>
#include<string>
#include<unordered_map>
//解析按钮名，因为比较长所以单独开文件
class InputParser
{
public:
	//eg   "A"/"a" -> "button_A"
	//WinKeyboardInput.cpp
	//WinMouseInput.cpp
	static const char* KeyNameToBaseName(const std::string& keyName);
	static const char* ParamNameToButtonBaseName(const std::string& paramName);
	static const char* ParamNameToAxisBaseName(const std::string& paramName);
	static const char* ButtonBaseNameToUIName(const char* buttonBaseName);

	SDL_Scancode ButtonBaseNameToScanCode(const char* buttonBaseName);

private:
	static InputParser& GetIns() { static InputParser ref; return ref; }
	InputParser();

	std::unordered_map<std::string, const char*> keyTable;//eg   "A"/"a" -> "Keyboard.A"
	std::unordered_map<std::string, SDL_Scancode> baseNameToScanCodeMap;
	

};







#endif