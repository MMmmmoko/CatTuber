#pragma once


#include<vector>
#include<mutex>
#include"WeakSupport.h"
namespace cat
{


	template <class T>
	class WeakCallbackList
	{
		struct CallBack
		{
			CallbackID id;
			WeakCallback<T> weakCallback;
		};
		unsigned int idUsage = 0;
		bool working = true;


		std::vector<CallBack> callbackList;
		std::mutex* mutex;
	public:
		WeakCallbackList()
		{
			mutex = new std::mutex;
		}
		~WeakCallbackList()
		{
			mutex->lock();
			mutex->unlock();
			delete mutex;
		}


		CallbackID Push(const WeakCallback<T>& weakCallback)
		{
			mutex->lock();
			callbackList.push_back({ idUsage,weakCallback });
			idUsage++;
			mutex->unlock();
			return idUsage;
		}

		void Push(CallbackID id,const WeakCallback<T>& weakCallback)
		{
			mutex->lock();
			callbackList.push_back({ id,weakCallback });
			mutex->unlock();
		}


		void Remove(CallbackID id)
		{
			mutex->lock();
			for (auto i = callbackList.begin(); i != callbackList.end();)
			{
				if (i->id==id)
				{
					callbackList.erase(i);
					return;
				}
			}
			mutex->unlock();
		
		}


		template<class... Args>
		void Call(Args&&... args) 
		{
		
			if (callbackList.empty())return;
			mutex->lock();
			for (auto i = callbackList.begin(); i != callbackList.end();)
			{
				if (i->weakCallback.Expired())
				{
					i=callbackList.erase(i);
				}
				else
				{
					(i->weakCallback)(std::forward<Args>(args)...);
						i++;
				}
			}

			mutex->unlock();
		};

		
	};


}