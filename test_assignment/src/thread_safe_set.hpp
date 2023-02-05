#pragma once

#include <mutex>
#include <unordered_set>
#include <queue>
#include <type_traits>

template <typename T >
class ThreadSafeSet
{
	static_assert( std::is_pointer_v<T>, "This queue works only with pointers" );
	std::unordered_set<T> mSet;
	std::mutex mMutex;

public:
	void Insert( T value )
	{
		std::lock_guard<std::mutex> lock( mMutex );
		mSet.insert( value );
	}

	T Front()
	{
		std::lock_guard<std::mutex> lock( mMutex );
		T value = *mSet.begin();
		mSet.erase( mSet.begin() );
		return value;
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> lock( mMutex );
		return mSet.empty();
	}

	T Find( T value )
	{
		std::lock_guard<std::mutex> lock( mMutex );
		auto res = mSet.find( value );
		if( res != mSet.end() )
			return *res;
		return nullptr;
	}

};