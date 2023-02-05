#pragma once

#include "target.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>

struct BuildUnit
{
	Target mTarget;
	std::unordered_set<BuildUnit*> mDependencies;
	std::unordered_set<BuildUnit*> mInversedDependencies;

	explicit BuildUnit( size_t id )
		: mTarget( LoadTestTarget( id ) )
	{}
};

class BuildGraph
{
public:
	std::vector<std::shared_ptr<BuildUnit>> mData;
	std::unordered_map<size_t, BuildUnit*> mUnits;

	explicit BuildGraph( const std::vector<std::pair<size_t, size_t>>& input )
	{
		auto get_unit = [&]( size_t id )
		{
			if( auto iter = mUnits.find( id ); iter != mUnits.end() )
				return iter->second;
			auto& unit = mData.emplace_back( std::make_shared<BuildUnit>( id ) );
			mUnits.emplace( id, unit.get() );
			return unit.get();
		};

		for( auto [unit_id, dep_id] : input )
		{
			auto* unit = get_unit( unit_id );
			auto* dep = get_unit( dep_id );
			unit->mDependencies.insert( dep );
			dep->mInversedDependencies.insert( unit );
		}
	}
};