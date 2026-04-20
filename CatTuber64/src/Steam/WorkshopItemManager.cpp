#if 0

#include "WorkshopItemManager.h"
#include"../CatGlobal.h"
#include"steam_api.h"
#include"CatSteam.h"
#include"JsonUtil.h"
#include"../CatTimmer.h"

namespace cat::steam
{


	WorkshopItemManager& WorkshopItemManager::GetInstance()
	{
		static WorkshopItemManager ref;
		return ref;
	}

	void WorkshopItemManager::RefreshWorkshopItemList()
	{
		if (!IsSteamInited())
		{
			return;
		}

		itemlist.characterList.clear();
		itemlist.handhelditemList.clear();
		itemlist.tableList.clear();
		itemlist.KPSList.clear();
		itemlist.BCMList.clear();

		


		uint32 itemcount=SteamUGC()->GetNumSubscribedItems();

		if (itemcount > 0)
		{
			PublishedFileId_t* pItemlist = new PublishedFileId_t[itemcount];
			uint32 itemcount_filled=SteamUGC()->GetSubscribedItems(pItemlist, itemcount);


			char pathbuffer[1024];
			for (uint32 i = 0; i < itemcount_filled; i++)
			{
				uint64 itemsize;
				uint32 timestamp;

				//bool result = SteamUGC()->GetItemState(pItemlist[i]);
				uint32 unItemState = SteamUGC()->GetItemState(pItemlist[i]);
				if (unItemState & k_EItemStateDownloading)
				{
					//表示此物品当前正在下载到客户端
				}
				//else if ((unItemState & k_EItemStateInstalled) && (unItemState & k_EItemStateNeedsUpdate))
				//{
				//	//表示此物品已安装但需要更新
				//}
				else{
				

					bool result = SteamUGC()->GetItemInstallInfo(pItemlist[i], &itemsize, pathbuffer, sizeof(pathbuffer), &timestamp);
					if (result && itemsize > 0)
					{
						CATLOG_INFO_("WorkshopItem:"<< pathbuffer);


						//辨别创意工坊种类 并分别pushback
						std::filesystem::path itempath= pathbuffer;
						itempath.append(L"itemdesc");
						if (std::filesystem::exists(itempath)&& !std::filesystem::is_directory(itempath))
						{
							//存在itemdesc文件，开始读取
							auto itemjson=cat::util::GetJsonFromFile(itempath);

							//判断物品类型
							if (itemjson["ItemType"].isString())
							{
								std::string itemtype = itemjson["ItemType"].asString();
								if (itemtype == CTB_ITEMTYPESTR_CHARACTER)
								{
									auto& item= itemlist.characterList.emplace_back();
									item.itempath = pathbuffer;
									item.itemid = pItemlist[i];
								}
								else if (itemtype == CTB_ITEMTYPESTR_HANDHELDITEM)
								{

									auto& item = itemlist.handhelditemList.emplace_back();
									item.itempath = pathbuffer;
									item.itemid = pItemlist[i];

								}
								else if (itemtype == CTB_ITEMTYPESTR_TABLE)
								{
									auto& item = itemlist.tableList.emplace_back();
									item.itempath = pathbuffer;
									item.itemid = pItemlist[i];


								}
								else if (itemtype == CTB_ITEMTYPESTR_KPS)
								{

									auto& item = itemlist.KPSList.emplace_back();
									item.itempath = pathbuffer;
									item.itemid = pItemlist[i];

								}
								else if (itemtype == CTB_ITEMTYPESTR_BCM016)
								{
									auto& item = itemlist.BCMList.emplace_back();
									item.itempath = pathbuffer;
									item.itemid = pItemlist[i];


								}

							}
						}



					}



				}
			}
		}

		//auto ugchandle = SteamUGC()->CreateQueryUserUGCRequest(SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Subscribed, k_EUGCMatchingUGCType_Items_ReadyToUse, 
		//	k_EUserUGCListSortOrder_SubscriptionDateDesc, 2233050, 1337970,1);

		//auto callresult=SteamUGC()->SendQueryUGCRequest(ugchandle);
		//metadataResult.Set(callresult, this, &WorkshopItemManager::OnGetItemMetaData);

		//bNeedWaitCallResult = true;
		//while (bNeedWaitCallResult)
		//{
		//	SteamAPI_RunCallbacks();
		//	Sleep(20);
		//}
		//
		




	}

	bool WorkshopItemManager::GetAuthorName(CSteamID authorid,std::string& name)
	{
		if (!IsSteamInited())
			return false;
		//bNeedWaitCallResult = true;
		if (SteamFriends()->RequestUserInformation(authorid, true))
		{
			//返回true表示需要从回调中获取信息


		//auto callresult=SteamUGC()->SendQueryUGCRequest(ugchandle);
		//metadataResult.Set(callresult, this, &WorkshopItemManager::OnGetItemMetaData);

			//添加计时器，三秒为极限
			CatTimmer timmer;

		while (true)
		{

			SteamAPI_RunCallbacks();
			//Sleep(20);

			for (auto it = idstack.begin(); it != idstack.end(); it++)
			{
				if (*it == authorid)
				{
					//idstack中添加了对应id，表示获取成功，删除此id
					idstack.erase(it);
					name = SteamFriends()->GetFriendPersonaName(authorid);
					return true;
				}
			}


			if (timmer.GetTime() > 3)
			{
				//bNeedWaitCallResult = false;
				return false;
			}
		}
		

		}
		else
		{
			//bNeedWaitCallResult = false;
			//false表示信息已经立即可用
			name=SteamFriends()->GetFriendPersonaName(authorid);

			return true;
		}

	}

	std::filesystem::path WorkshopItemManager::GetItemPath(const char* itemtype, const std::wstring& itemid)
	{

		if (itemtype == CTB_ITEMTYPESTR_CHARACTER)
		{
			for (auto& x : itemlist.characterList)
			{
				if (x.itempath.filename() == itemid)
					return x.itempath;
			}


		}
		else if (itemtype == CTB_ITEMTYPESTR_HANDHELDITEM)
		{

			for (auto& x : itemlist.handhelditemList)
			{
				if (x.itempath.filename() == itemid)
					return x.itempath;
			}

		}
		else if (itemtype == CTB_ITEMTYPESTR_TABLE)
		{
			for (auto& x : itemlist.tableList)
			{
				if (x.itempath.filename() == itemid)
					return x.itempath;
			}



		}
		else if (itemtype == CTB_ITEMTYPESTR_KPS)
		{
			for (auto& x : itemlist.KPSList)
			{
				if (x.itempath.filename() == itemid)
					return x.itempath;
			}


		}
		else if (itemtype == CTB_ITEMTYPESTR_BCM016)
		{
			for (auto& x : itemlist.BCMList)
			{
				if (x.itempath.filename() == itemid)
					return x.itempath;
			}
		}


		return std::filesystem::path();
	}

	PublishedFileId_t WorkshopItemManager::GetItemIDFromPath(std::filesystem::path itempath, const char* itemtype)
	{
		if (NULL == itemtype)
		{
			for (auto& x : itemlist.BCMList)
			{
				if (x.itempath == itempath)
				{
					return x.itemid;
				}
			}
			for (auto& x : itemlist.characterList)
			{
				if (x.itempath == itempath)
				{
					return x.itemid;
				}
			}
			for (auto& x : itemlist.handhelditemList)
			{
				if (x.itempath == itempath)
				{
					return x.itemid;
				}
			}
			for (auto& x : itemlist.tableList)
			{
				if (x.itempath == itempath)
				{
					return x.itemid;
				}
			}
			for (auto& x : itemlist.KPSList)
			{
				if (x.itempath == itempath)
				{
					return x.itemid;
				}
			}
		}
		else
		{

			if (0 == strcmp(CTB_ITEMTYPESTR_BCM016, itemtype))
			{
				for (auto& x : itemlist.BCMList)
				{
					if (x.itempath == itempath)
					{
						return x.itemid;
					}
				}
			}
			else if(0 == strcmp(CTB_ITEMTYPESTR_CHARACTER, itemtype))
			{
				for (auto& x : itemlist.characterList)
				{
					if (x.itempath == itempath)
					{
						return x.itemid;
					}
				}
			}
			else if (0 == strcmp(CTB_ITEMTYPESTR_HANDHELDITEM, itemtype))
			{
				for (auto& x : itemlist.handhelditemList)
				{
					if (x.itempath == itempath)
					{
						return x.itemid;
					}
				}
			}
			else if (0 == strcmp(CTB_ITEMTYPESTR_KPS, itemtype))
			{
				for (auto& x : itemlist.KPSList)
				{
					if (x.itempath == itempath)
					{
						return x.itemid;
					}
				}
			}
			else if (0 == strcmp(CTB_ITEMTYPESTR_TABLE, itemtype))
			{
				for (auto& x : itemlist.tableList)
				{
					if (x.itempath == itempath)
					{
						return x.itemid;
					}
				}
			}

		}




		return k_PublishedFileIdInvalid;

	}

	std::mutex trackingmtx;
	void WorkshopItemManager::StartPlaytimeTracking(PublishedFileId_t itemid)
	{
		//
		std::thread thread([this, itemid] {
			PublishedFileId_t id = itemid;
			trackingmtx.lock();
			SteamUGC()->StartPlaytimeTracking(&id, 1);
			while (!trackingmtx.try_lock())
			{
				SteamAPI_RunCallbacks();
				Sleep(50);
			}
			trackingmtx.unlock();


			});
		thread.detach();

	}

	void WorkshopItemManager::StopPlaytimeTracking(PublishedFileId_t itemid)
	{
		std::thread thread([this, itemid] {
			PublishedFileId_t id = itemid;
			trackingmtx.lock();
			SteamUGC()->StopPlaytimeTracking(&id, 1);
			while (!trackingmtx.try_lock())
			{
				SteamAPI_RunCallbacks();
				Sleep(50);
			}
			trackingmtx.unlock();
			});

		thread.detach();
	}


	void WorkshopItemManager::OnTrackingStateChange(StartPlaytimeTrackingResult_t* pCallback, bool bIOFailure)
	{
		trackingmtx.unlock();
		return;
	}

	void WorkshopItemManager::AddPlaytimeTrackingRef(PublishedFileId_t itemid,int count)
	{
		if (itemid == k_PublishedFileIdInvalid || count == 0)
			return;

		auto it = _itemidtable.find(itemid);
		if (it != _itemidtable.end())
		{

			if (0 == it->second && it->second + count > 0)
			{
				StartPlaytimeTracking(itemid);
			}
			else if (0< it->second&& it->second + count<=0)
			{
				StopPlaytimeTracking(itemid);
			}

			it->second += count;
			if (0 < it->second)
				it->second = 0;
		}
		else
		{
			if (count > 0)
			{
				_itemidtable[itemid] = count;
				StartPlaytimeTracking(itemid);
			}

		}


	}



	void WorkshopItemManager::OnGetItemMetaData(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
	{
		if (bIOFailure)
		{
			CATLOG_ERROR_("WorkshopItem MetaData IO Failure.");
			bNeedWaitCallResult = false;
			return;
		}

		bNeedWaitCallResult = false;
		CATLOG_INFO_("QueryItem Completed. Total matching results count:" << pCallback->m_unTotalMatchingResults);

		if (pCallback->m_eResult == EResult::k_EResultOK)
		{

			char buf[1024];

			for (int i = 0; i < pCallback->m_unNumResultsReturned; i++)
			{
				if (SteamUGC()->GetQueryUGCMetadata(pCallback->m_handle, i, buf, sizeof(buf)))
				{
					//获取到METADATA
					CATLOG_INFO_("WorkshopItem MetaData"<<buf);
				}
				else
				{
					CATLOG_ERROR_("GetQueryUGCMetadata Failure.");
				}

				

			}
		}


		bNeedWaitCallResult = false;

	}



	void WorkshopItemManager::OnGetAuthorName(PersonaStateChange_t* pCallback)
	{
		//if (bIOFailure)
		//{
		//	CATLOG_ERROR_("Get WorkshopItem Author Name, IO Failure.");
		//	bNeedWaitCallResult = false;
		//	lastResult = false;
		//	return;
		//}


		idstack.push_back(pCallback->m_ulSteamID);
		//bNeedWaitCallResult = false;
		lastResult = true;
	}

}

#endif