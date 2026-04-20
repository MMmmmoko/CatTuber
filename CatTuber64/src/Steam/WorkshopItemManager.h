#pragma once
#if 0
#include <windows.h>
#include<iostream>
#include<filesystem>
#include<steam_api.h>
#include<map>



namespace cat::steam
{
	

	struct WorkshopItem
	{
		std::filesystem::path itempath;
		PublishedFileId_t itemid;
	};











	class WorkshopItemManager
	{
	private:
		WorkshopItemManager(){};
	public:
		static WorkshopItemManager& GetInstance();
	
		void RefreshWorkshopItemList();

		const auto& GetItemList() { return itemlist; };
		bool GetAuthorName(CSteamID uid, std::string& name);//返回是否调用成功，调用不成功的话可以提示开发者不再继续循环调用
		std::filesystem::path GetItemPath(const char* type,const std::wstring& itemid);

		PublishedFileId_t GetItemIDFromPath(std::filesystem::path itempath, const char* itemtype=NULL);

		void AddPlaytimeTrackingRef(PublishedFileId_t itemid, int count);


	private:
		struct 
		{
			std::vector<WorkshopItem> characterList;
			std::vector<WorkshopItem> handhelditemList;
			std::vector<WorkshopItem> tableList;
			std::vector<WorkshopItem> KPSList;
			std::vector<WorkshopItem> BCMList;
		}itemlist;
		
		bool namecalllock = false;
		bool bNeedWaitCallResult = false;
		bool lastResult = false;
		//bool bNeedWaitCallResult = false;

		void StartPlaytimeTracking(PublishedFileId_t itemid);
		void StopPlaytimeTracking(PublishedFileId_t itemid);
		void OnTrackingStateChange(StartPlaytimeTrackingResult_t* pCallback, bool bIOFailure);
		CCallResult<WorkshopItemManager, StartPlaytimeTrackingResult_t > trackingResult;
		bool _NeedWaitTrackingResultCall = false;



		void OnGetItemMetaData(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
		CCallResult<WorkshopItemManager, SteamUGCQueryCompleted_t > metadataResult;



		STEAM_CALLBACK(WorkshopItemManager, OnGetAuthorName, PersonaStateChange_t);


		//void OnGetAuthorName(PersonaStateChange_t* pCallback);

		//查询用
		std::vector<CSteamID> idstack;
		//计算物品使用时间用
		std::map<PublishedFileId_t, int> _itemidtable;
	};



};

#endif





