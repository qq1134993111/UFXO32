#pragma once
#include<memory>
#include<vector>
#include <stdint.h>
#include <chrono>
#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/thread.hpp"

class GeneralProcessor
{
public:
    GeneralProcessor() :
        ios_(),
        work_(new boost::asio::io_service::work(ios_)),
        strand_(ios_)
    {
    }

    GeneralProcessor(size_t thread_number) :GeneralProcessor()
    {
        Start(thread_number);
    }

    ~GeneralProcessor()
    {
        Stop(true);
    }

    bool Start(size_t thread_number = 1)
    {

        if (ios_.stopped())
        {
            ios_.reset();
            work_.reset(new boost::asio::io_service::work(ios_));
            v_thread_.clear();
        }

        if (!v_thread_.empty())
            return false;

        thread_number = std::max(thread_number, size_t(1));
        for (; thread_number-- > 0;)
        {
            v_thread_.push_back(boost::thread([this]()
                {
                    boost::system::error_code ec;
                    ios_.run(ec);
                }));
        }

        return  true;
    }

    void Stop(bool wait = true)
    {
        work_.reset();
        if (!ios_.stopped())
        {
            ios_.stop();
        }

        if (wait)
        {
            WaitStop();
        }
    }

    template<typename Function>
    void Post(Function&& function)
    {
        strand_.post(std::forward<Function>(function));
        //ios_.post(strand_.wrap(std::forward<Function>(function)));
    }
    template<class F, class... Args>
    void Post(F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        Post(std::move(func));
    }

    template<typename Function>
    void Dispatch(Function&& function)
    {
        strand_.dispatch(std::forward<Function>(function));
        //ios_.dispatch(strand_.wrap(std::forward<Function>(function)));
    }
    template<class F, class... Args>
    void Dispatch(F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        Dispatch(std::move(func));
    }


    template<class Rep, class Period, typename Function>
    std::weak_ptr<boost::asio::steady_timer> AddTimer(std::chrono::duration<Rep, Period>  duration, Function&& function)
    {
        return SetTimer(duration, std::move(function), nullptr);
    }

    template<class Rep, class Period, class F, class... Args>
    std::weak_ptr<boost::asio::steady_timer> AddTimer(std::chrono::duration<Rep, Period> duration, F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        return AddTimer(duration, std::move(func));
    }

    template<typename Function>
    void PostWithoutStrand(Function&& function)
    {
        ios_.post(std::forward<Function>(function));
    }
    template<class F, class... Args>
    void PostWithoutStrand(F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        PostWithoutStrand(std::move(func));
    }

    template<typename Function>
    void DispatchWithoutStrand(Function&& function)
    {
        ios_.dispatch(std::forward<Function>(function));
    }
    template<class F, class... Args>
    void DispatchWithoutStrand(F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        DispatchWithoutStrand(std::move(func));
    }


    template<class Rep, class Period, typename Function>
    std::weak_ptr<boost::asio::steady_timer> AddTimerWithoutStrand(std::chrono::duration<Rep, Period> duration, Function&& function)
    {
        return SetTimerWithoutStrand(duration, std::move(function), nullptr);
    }

    template<class Rep, class Period, class F, class... Args>
    std::weak_ptr<boost::asio::steady_timer> AddTimerWithoutStrand(std::chrono::duration<Rep, Period> duration, F&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        return AddTimerWithoutStrand(duration, std::move(func));
    }

    std::size_t CancelTimer(const std::weak_ptr<boost::asio::steady_timer>& weak_timer)
    {
        auto timer = weak_timer.lock();
        if (timer)
        {
            boost::system::error_code ec;
            auto cancel_count = timer->cancel(ec);
            return cancel_count;
        }

        return 0;
    }
protected:
    template<class Rep, class Period, typename Function>
    std::weak_ptr<boost::asio::steady_timer> SetTimer(std::chrono::duration<Rep, Period>  duration, Function&& function, std::shared_ptr<boost::asio::steady_timer> timer = nullptr)
    {
        if (timer == nullptr)
        {
            timer = std::make_shared<boost::asio::steady_timer>(ios_, duration);
        }
        else
        {
            timer->expires_from_now(duration);
        }

        timer->async_wait(strand_.wrap([this, duration, function = std::forward<Function>(function), timer](const boost::system::error_code& ec)
        {
            if (ec)return;

            if (function())
            {
                SetTimer(duration, std::move(function), timer);
            }

        }));

        return  timer;
    }


    template<class Rep, class Period, typename Function>
    std::weak_ptr<boost::asio::steady_timer> SetTimerWithoutStrand(std::chrono::duration<Rep, Period>  duration, Function&& function, std::shared_ptr<boost::asio::steady_timer> timer = nullptr)
    {
        if (timer == nullptr)
        {
            timer = std::make_shared<boost::asio::steady_timer>(ios_, duration);
        }
        else
        {
            timer->expires_from_now(duration);
        }

        timer->async_wait([this, duration, function = std::forward<Function>(function), timer](const boost::system::error_code& ec)
        {
            if (ec)return;

            if (function())
            {
                SetTimerWithoutStrand(duration, std::move(function), timer);
            }

        });

        return  timer;
    }

protected:
    void WaitStop()
    {
        for (auto& thd : v_thread_)
        {
            if (thd.joinable())
                thd.join();
        }
    }

private:
    boost::asio::io_service ios_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    boost::asio::io_service::strand strand_;
    std::vector<boost::thread> v_thread_;
};