#pragma once

#include <thread>
#include <future>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>
#include <algorithm>

class ThreadPool
{
	size_t mThreadsNum;
	std::vector<std::thread> mPool;
	std::queue<std::function<void()>> mTasks;
	std::condition_variable mEvent;
	std::mutex mMutex;
	bool mStopping;

public:
	explicit ThreadPool( size_t threads )
		: mThreadsNum( std::min<size_t>( std::thread::hardware_concurrency(), threads ) ),
		  mStopping( false )
	{
		Start();
	}

	~ThreadPool()
	{
		Stop();
	}

	template <typename T>
	auto AddTask( T task ) -> std::future<decltype( task() )>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype( task() ) ( )>>( std::move( task ) );
		{
			std::unique_lock<std::mutex> lock( mMutex );
			mTasks.push( [=]() {
				( *wrapper )( );
			} );
		}
		mEvent.notify_one();
		return wrapper->get_future();
	}

private:

	void Start()
	{
		for( size_t i = 0; i < mThreadsNum; i++ )
		{
			mPool.push_back( std::thread( [&]() {
				while( true )
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock( mMutex );
						mEvent.wait( lock, [&]() {
							return mStopping || !mTasks.empty();
						} );
						if( mStopping && mTasks.empty() )
							break;
						task = std::move( mTasks.front() );
						mTasks.pop();
					}
					task();
				}
			} ) );
		}
	}

	void Stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock( mMutex );
			mStopping = true;
		}
		mEvent.notify_all();
		for( std::thread& thread : mPool )
			thread.join();
	}
};