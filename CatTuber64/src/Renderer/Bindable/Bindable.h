#ifndef _Bindable_h
#define _Bindable_h

#include<SDL3/SDL.h>
#include<string>

namespace rendering
{
	class Bindable
	{
	public:
		virtual void Bind() = 0;
		//virtual void InitializeParentReference(const Drawable&)noexcept {};


		//virtual std::string GetUID() const noexcept
		//{
		//	SDL_assert(false);
		//	return"";
		//}

		virtual ~Bindable() = default;


	};
	//class CloningBindable : public Bindable
	//{
	//public:
	//	virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	//};
}





#endif