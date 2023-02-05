#pragma once

#include "build_graph.hpp"
#include "thread_pool.hpp"
#include "thread_safe_set.hpp"

#include <queue>
#include <string>
#include <stdexcept>

bool HasCycles( std::unordered_set<BuildUnit*> used, BuildUnit* cur )
{
	if( !used.emplace( cur ).second )
		return true;
	for( auto* dep : cur->mDependencies )
	{
		if( HasCycles( used, dep ) )
			return true;
	}
	return false;
}

// return: <Build list, Leafs>
std::pair<std::unordered_set<BuildUnit*>, std::unordered_set<BuildUnit*>>
GetTargetsToBuild( const BuildGraph& build_graph, size_t target_id )
{
	auto target_to_build = build_graph.mUnits.find( target_id );
	if( target_to_build == build_graph.mUnits.end() )
		throw std::runtime_error( "Invalid build target: " + std::to_string( target_id ) );
	// Possible to get leafs in dfs
	if( HasCycles( {}, target_to_build->second ) )
		throw std::runtime_error( "Build graph contain cycles" );

	std::unordered_set<BuildUnit*> build_list;
	std::unordered_set<BuildUnit*> leafs;
	std::queue<BuildUnit*> to_process;

	to_process.push( target_to_build->second );
	while( !to_process.empty() )
	{
		auto* unit = to_process.front();
		to_process.pop();
		build_list.insert( unit );

		if( unit->mDependencies.empty() )
			leafs.insert( unit );
		for( auto* dep : unit->mDependencies )
			to_process.push( dep );
	}
	return { build_list, leafs };
}


class Builder
{
	ThreadPool mThreadPool;
	ThreadSafeSet<BuildUnit*> mComplited;
	std::condition_variable mEndOfBuild;
	std::mutex mEndOfBuildMutex;

	void Build( BuildUnit* unit )
	{
		mThreadPool.AddTask( [&, unit](){
			unit->mTarget.mTask();
			mComplited.Insert( unit );
			mEndOfBuild.notify_one();
		} );
	}

public:
	explicit Builder( size_t num_threads )
		: mThreadPool( num_threads )
	{}

	void Execute( BuildGraph build_graph, size_t target_id )
	{
		auto [build_list, leafs] = GetTargetsToBuild( build_graph, target_id );
		auto target = build_graph.mUnits.find( target_id )->second;

		for( auto* leaf : leafs )
			Build( leaf );

		while( true )
		{
			std::unique_lock<std::mutex> lock( mEndOfBuildMutex );
			mEndOfBuild.wait( lock, [&]() {
				return !mComplited.Empty();
			} );
			if( mComplited.Find( target ) )
				break;

			while( !mComplited.Empty() )
			{
				auto* unit = mComplited.Front();
				for( auto* inv_dep : unit->mInversedDependencies )
				{
					inv_dep->mDependencies.erase( unit );
					if( build_list.count( inv_dep ) && inv_dep->mDependencies.empty() )
						Build( inv_dep );
				}
			}
		}
	}
};
