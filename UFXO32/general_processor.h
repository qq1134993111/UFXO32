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


	template<typename Function>
	void AddTimer(std::chrono::microseconds  micro_seconds, Function&& function)
	{
		auto timer = std::make_shared<boost::asio::steady_timer>(ios_, micro_seconds);
		//timer->expires_from_now(micro_seconds);
		timer->async_wait(strand_.wrap([this, micro_seconds, function = std::forward<Function>(function), timer](const boost::system::error_code& ec)
		{
			if (ec)return;

			if (function())
			{
				AddTimer(micro_seconds, std::move(function));
			}
		}));


	}

	template<class F, class... Args>
	void AddTimer(std::chrono::microseconds micro_seconds, F&& f, Args&&... args)
	{
		auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		AddTimer(micro_seconds, std::move(func));
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

	template<typename Function>
	void AddTimerWithoutStrand(std::chrono::microseconds micro_seconds, Function&& function)
	{
		auto timer = std::make_shared<boost::asio::steady_timer>(ios_, micro_seconds);
		//timer->expires_from_now(micro_seconds);
		timer->async_wait([this, micro_seconds, function = std::forward<Function>(function), timer](const boost::system::error_code& ec)
		{
			if (ec)return;

			if (function())
			{
				AddTimer(micro_seconds, std::move(function));
			}
		});
	}

	template<class F, class... Args>
	void AddTimerWithoutStrand(std::chrono::microseconds micro_seconds, F&& f, Args&&... args)
	{
		auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		AddTimerWithoutStrand(micro_seconds, std::move(func));
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