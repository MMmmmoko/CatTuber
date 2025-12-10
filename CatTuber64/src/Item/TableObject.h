#ifndef _TableObject_h
#define _TableObject_h


//桌子对象除了自身之外还将为笔和角色提供运动数据


#include"json/json.h"
#include"Model/IModel.h"
#include"Input/InputCommon.h"
#include"ModelControl.h"


class MixDrawList;



class TableObject
{
	friend class ClassicItem;

public:

	//Question:创意工坊路径和preferPath是有很小的可能会变化的，需要对路径进行一下包装防止路径过于绝对吗？
	//包装也可能反而导致寻找不到文件(切换路径而不搬运文件).
	bool LoadFromPath(const char* u8PackPath,const Json::Value& bindingJson= Json::Value());//入参应该是个文件夹或者资源包
	const char* GetPackPath() { return resourcePath.c_str(); };
	IModel* GetModel() { return _model; };



	void Update(uint64_t deltaTicksNS);
	//void Update_Late(uint64_t deltaTicksNS);//第二轮Update
	//其实是将模型推送到DrawList
	void Draw(MixDrawList* drawList);
	void OnLoopEnd();//一些清理和状态重置工作

	//有时希望不显示模型让模型不工作，但暂时不希望删除模型（如预览其他模式时），
	// 所以使用这个方法设置停止工作的标识
	void SetWorkingFlag(bool bWorking) { working = bWorking; };




	virtual Json::Value GenerateAttributes();
	//如果出现问题返回空
	static TableObject* CreateFromAttributes(const Json::Value& applyJson);
	static void ReleaseObj(TableObject*);//Release需要和Create搭配，免得其他人不知道怎么正确释放资源




	const std::vector<ModelButtonControl>& GetModelButtons() { return modelButtonVec; };
	const std::vector<ModelAxisControl>& GetModelAxes() { return modelAxisVec; };
	const std::vector<ModelAnimationControl>& GetModelAnimations() { return modelAnimationVec; };





	//物理按键到模型按钮（索引）与action的绑定？
	//物理按键到actionName的绑定？
	//物理按键到模型按钮actionName的绑定？
	//需要添加模型参数吗？
	//JsonRoot[PackPath]= [AppBasePath]/... [AppPrefPath]/...  [WorkshopPath]/..  创意工坊路径需要在软件退出时保存以便在离线时读取
	//JsonRoot[PackName]//模型名需要提供两份？一份提供给软件验证？一份多语言提供给UI显示？
	//JsonRoot[BindingName]="测试中文"
	//参考_SetUpControlAndAnimation函数，但不完全一致
	//直接按索引来吗？
	//JsonRoot[Buttons][0][Type]="ButtonToAction"
	//JsonRoot[Buttons][0][Button][0]="Keyborad.A"//WinKeyboardInput.cpp
	//JsonRoot[Buttons][0][DownAction]="Table.Button.0.Down"
	//JsonRoot[Buttons][0][UpAction]="Table.Button.0.Up"
	//Action
	


	bool LoadBindingByName(const char* bindingName);//读取手动保存的绑定
	void LoadBinding();//先尝试读取被App保存的用户设置的绑定，然后尝试读取Pack文件中缓存的绑定，如果没有则根据模型参数名生成绑定
	void ClearBinding();//模型卸载的时候移除绑定的回调//预览其他模型时不需要这个，因为有个working判断，仅在删除此对象时调用

private:


	//注册按钮、轴等数据，即将具体函数绑定到对应Action上
	void RegisterAllActionFunc(bool falseToUnregister=true);
	void UnregisterAllActionFunc();//Action与实际成员函数的绑定


	void OnButtonDown(int btnIndex);//控制参数的同时还要触发按钮上绑定的动画//发送一个TableButtonDown的Action（当Event用）
	void OnButtonUp(int btnIndex);//发送一个TableButtonUp的Action（当Event用）
	void OnAxisValueChange(int axisGroupIndex, int axisIndex,float value);//控制对应参数
	void OnAnimationPlay(int animationIndex);

	void _UpdateAxisVec();


	//模型一些软件参数
	float offsetX = 0.f;
	float offsetY = 0.f;
	float scale = 1.f;


	//模型自身参数
	bool working = false;
	IModel* _model=NULL;
	ClassicItem* _pParentItem = NULL;
	std::string resourcePath;

	//模型控件
	std::vector<ModelButtonControl> modelButtonVec;
	std::vector<ModelAxisControl> modelAxisVec;
	std::vector<ModelAnimationControl> modelAnimationVec;

	//辅助数据
	//用于计算当前帧按下的按钮数设立一个缓存防止反复申请内存
	std::vector<ModelButtonControl*> _pushedButtnVec;
	std::vector<struct ModelAxisControl*> _noMoveMouseAxisControls_L;//未移动但是被标注了active的鼠标轴
	std::vector<struct ModelAxisControl*> _noMoveMouseAxisControls_R;//未移动但是被标注了active的鼠标轴
};





#endif