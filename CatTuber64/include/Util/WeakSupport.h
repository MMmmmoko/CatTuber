#pragma once

#include<functional>
#include<memory>


typedef std::function<void(intptr_t eventid, int64_t param1, int64_t param2)> NamedEventCallback;


typedef unsigned int CallbackID;

extern const CallbackID InvalidCallbackID ;

namespace cat
{

    class Weakable;

    template <class T>
    class WeakCallback
    {

    public:

        mutable T _func;
        std::weak_ptr<Weakable> _weakPtr;
        //static WeakCallback NullCallback;

        WeakCallback()
        {

        }

        WeakCallback(std::weak_ptr<Weakable> weak, const T& func)
            :_weakPtr(weak), _func(func)
        {


        }

        WeakCallback(std::weak_ptr<Weakable> weak, T&& func)
            :_weakPtr(weak), _func(std::move(func))
        {


        }


        template<class WeakType>
        WeakCallback(const WeakType& weak_callback) :
            _weakPtr(weak_callback._weakPtr),
            _func(weak_callback._func)
        {

        }




        //返回false表示函数过期，需要从原列表中删除对应项
        template<class... Args>
        auto operator()(Args&&... args) const
#if _MSC_VER>1900
#else
            ->decltype(func(std::forward<Args>(args)...))
#endif
        {
            auto sptr=_weakPtr.lock();

            if (sptr)
            {
                return _func(std::forward<Args>(args)...);
            }
            return  decltype(_func(std::forward<Args>(args)...))();

        }



        //bool operator==(const WeakCallback<T>& b)const
        //{
        //    return func == b.func;
        //}


        bool Expired()const
        {
            return _weakPtr.expired();
        }

    };

    class Weakable
    {

        std::shared_ptr<Weakable> _this;

        typedef void(*NullReleaseFunc)(Weakable*);
    public:
        Weakable()
        {
            _this.reset(this,[](Weakable* x) {});
        }
        virtual ~Weakable() {}


        template <class CallbackType>
        WeakCallback<CallbackType> ToWeakFunc(const CallbackType& func)
        {

            //std::weak_ptr<Weakable> weak = _this;

            return WeakCallback<CallbackType>(GetWeakFlag(), func);
        }

        std::weak_ptr<Weakable> GetWeakFlag()
        {
            //if (_this.use_count() == 0) {
            //    _this.reset((Weakable*)NULL);
            //}
            return _this;
        }
    };

    // global function 
    template<class F, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<F>::value>::type>
    auto myBind(F&& f, Args && ... args)
        -> decltype(std::bind(f, args...))
    {
        return std::bind(f, args...);
    }

    // const class member function 
    template<class R, class C, class... DArgs, class P, class... Args>
    auto myBind(R(C::* f)(DArgs...) const, P&& p, Args && ... args)
        -> WeakCallback<decltype(std::bind(f, p, args...))>
    {
        std::weak_ptr<Weakable> weak_flag = ((Weakable*)p)->GetWeakFlag();
        auto bind_obj = std::bind(f, p, args...);
        static_assert(std::is_base_of<Weakable, C>::value, "nbase::SupportWeakCallback should be base of C");
        WeakCallback<decltype(bind_obj)> weak_callback(weak_flag, std::move(bind_obj));
        return weak_callback;
    }

    template<class R, class C, class... DArgs, class P, class... Args>
    auto myBind(R(C::* f)(DArgs...), P&& p, Args && ... args)
        -> WeakCallback<decltype(std::bind(f, p, args...))>
    {
        std::weak_ptr<Weakable> weak_flag = ((Weakable*)p)->GetWeakFlag();
        auto bind_obj = std::bind(f, p, args...);
        static_assert(std::is_base_of<Weakable, C>::value, "nbase::SupportWeakCallback should be base of C");
        WeakCallback<decltype(bind_obj)> weak_callback(weak_flag, std::move(bind_obj));
        return weak_callback;
    }

}