#ifndef __PROFILER_H_INCLUDED__
#define __PROFILER_H_INCLUDED__

//! Enable profiling information
#define _IRR_COMPILE_WITH_PROFILING_ 1

#include "IrrCompileConfig.h"
#include "rect.h"
#include "irrString.h"
#include "irrArray.h"
#include "ITimer.h"
#include "IGUIEnvironment.h"
#include "IGUIListBox.h"
#include <limits.h>

namespace irr
{
struct SProfileData
{
	friend class CProfiler;

    SProfileData()
	{
		GroupIndex = -1;
		reset();
	}

	bool operator<(const SProfileData& pd) const
	{
		return Id < pd.Id;
	}

	bool operator==(const SProfileData& pd) const
	{
		return Id == pd.Id;
	}

	s32 getGroupIndex() const
	{
		return GroupIndex;
	}

	const core::stringw& getName() const
	{
		return Name;
	}

	//! Each call to Profiler::stop for this data increases the counter by 1.
	u32 getCallsCounter() const
	{
		return CountCalls;
	}

	u32 getLongestTime() const
	{
		return LongestTime;
	}

	u32 getShortestTime() const
	{
		return ShortestTime;
	}

	s32 getTimeSum() const
	{
		return TimeSum;
	}

	//! Convert the whole data into a string
	core::stringw getAsString() const
	{
		if ( CountCalls > 0 )
		{
	#ifdef _MSC_VER
	#pragma warning(disable:4996)	// 'sprintf' was declared deprecated
	#endif
			// Can't use swprintf as it fails on some platforms (especially mobile platforms)
			char dummy[1023];
			sprintf(dummy, "%-15.15s%-12u%-12u%-12u%-12u%-12u",
				core::stringc(Name).c_str(), CountCalls, TimeSum,
				TimeSum / CountCalls, LongestTime,
				(ShortestTime <= LongestTime ? ShortestTime : 0)
				);
			dummy[1022] = 0;

			return core::stringw(dummy);
	#ifdef _MSC_VER
	#pragma warning(default :4996)	// 'sprintf' was declared deprecated
	#endif
		}
		else
		{
			return Name;
		}
	}

	//! Return a string which describes the columns returned by getAsString
	static core::stringw makeTitleString()
	{
		return core::stringw("name           calls       time(sum)   time(avg)   time(max)   time(min)");
	}

private:
	SProfileData(u32 id) : Id(id) {}	// just to be used for searching

	void reset()
	{
		CountCalls = 0;
		LongestTime = 0;
		ShortestTime = UINT_MAX;
		TimeSum = 0;
		LastTimeStarted = 0;
	}

	s32 Id;
    s32 GroupIndex;
	core::stringw Name;

    u32 CountCalls;
    u32 LongestTime;
    u32 ShortestTime;
    u32 TimeSum;

    u32 LastTimeStarted;
};

//! Code-profiler. Please check the example in the Irrlicht examples folder about how to use it.
// The design is all about using the central start/stop mechanism with minimal overhead.
// This is why this class works without a virtual functions interface contrary to the usual Irrlicht design.
// And also why we work with id's instead of strings in the start/stop functions even if it makes using
// the class slightly harder.
// Also it comes without reference-counting as the profiler-class is never released.
// But it's also _not_ a singleton (still to be discussed), but life-time is only guaranteed to be inside main.
class CProfiler
{
public:
    CProfiler()
	: IrrTimer(0)
	, NextAutoId(INT_MAX)
	{
		addGroup(L"overview");
	}

	~CProfiler()
	{
		if ( IrrTimer )
			IrrTimer->drop();
	}

	// TODO: can be automatic once this is in Irrlicht
    void init(irr::ITimer * irrlichtTimer)
	{
		IrrTimer = irrlichtTimer;
		IrrTimer->grab();
	}

	//! Add an id with given name and group which can be used for profiling with start/stop
	/** After calling this once you can start/stop profiling for the given id.
	\param id: Should be >= 0 as negative id's are reserved for Irrlicht. Also very large numbers (near INT_MAX) might
	have been added automatically by the other add function.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data. L"Irrlicht" is used by the Irrlicht engine itself. */
    void add(s32 id, const core::stringw &name, const core::stringw &groupName);

	//! Add an automatically generated for the given name and group which can be used for profiling with start/stop.
	/** After calling this once you can start/stop profiling with the returned id.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data. L"Irrlicht" is used by the Irrlicht engine itself.
	\return The id which was automatically generated for this name. If the name already has an id then that id will be returned.*/
    s32 add(const core::stringw &name, const core::stringw &groupName);

	//! Return the number of profile data blocks. There is one for each id.
    u32 getProfileDataCount() const
    {
		return ProfileDatas.size();
    }

	//! Get the profile data
	/** \param index A value between 0 and getProfileDataCount()-1.	Indices can change when new id's are added.*/
    const SProfileData& getProfileDataByIndex(u32 index) const
    {
		return ProfileGroups[index];
    }

	//! Get the profile data
	/** \param id Same value as used in ::add
	\return Profile data for the given id or 0 when it does not exist.	*/
    const SProfileData* getProfileDataById(u32 id)
    {
		SProfileData data(id);
    	s32 idx = ProfileDatas.binary_search(data);
		if ( idx >= 0 )
			return &ProfileDatas[idx];
		return NULL;
    }

	//! Get the number of profile groups. Will be at least 1.
	/** NOTE: The first groups is always L"overview" which is an overview for all existing groups */
    u32 getGroupCount() const
    {
		return ProfileGroups.size();
    }

    //! Get profile data for a group.
    /** NOTE: The first groups is always L"overview" which is an overview for all existing groups */
    const SProfileData& getGroupData(u32 index) const
    {
		return ProfileGroups[index];
    }

	//! Start profile-timing for the given id
	/** NOTE: you have to add the id first with one of the ::add functions */
	inline void start(s32 id_);

	//! Stop profile-timing for the given id
	/** NOTE: timer must have been started first with the ::start function */
    inline void stop(s32 id_);

	//! Reset profile data for one id
    void reset(s32 id_);

    //! Reset all profile data
    /** NOTE: This is not deleting id's or groups, just resetting all timers to 0. */
    void resetAll();

	//! Write all profile-data into a string
    void printAll(core::stringw &ostream, bool includeOverview=false,bool suppressUncalled_=true) const;

	//! Write the profile data of one group into a string
    void printGroup(core::stringw &ostream, s32 groupIndex_, bool suppressUncalled_) const;

protected:

    s32 getDataIndex(const core::stringw &name) const
    {
		for ( u32 i=0; i < ProfileDatas.size(); ++i )
		{
			if ( ProfileDatas[i].Name == name )
			{
				return (s32)i;
			}
		}

		return -1;
    }

	s32 getGroupIndex(const core::stringw &name) const
	{
		for ( u32 i=0; i < ProfileGroups.size(); ++i )
		{
			if ( ProfileGroups[i].Name == name )
			{
				return (s32)i;
			}
		}

		return -1;
	}

    s32 addGroup(const core::stringw &name_);

private:
    irr::ITimer * IrrTimer;
    irr::s32 NextAutoId;	// for giving out id's automatically
	core::array<SProfileData> ProfileDatas;
    core::array<SProfileData> ProfileGroups;
};

s32 CProfiler::add(const core::stringw &name, const core::stringw &groupName)
{
	s32 index = getDataIndex(name);
	if ( index >= 0 )
	{
		add( ProfileDatas[index].Id, name, groupName );
		return ProfileDatas[index].Id;
	}
	else
	{
		s32 id = NextAutoId;
		--NextAutoId;
		add( id, name, groupName );
		return id;
	}
}

void CProfiler::add(s32 id, const core::stringw &name, const core::stringw &groupName)
{
	s32 groupIdx = getGroupIndex(groupName);
	if ( groupIdx < 0 )
	{
		groupIdx = addGroup(groupName);
	}

	SProfileData data;
	data.Id = id;
    data.GroupIndex = groupIdx;
    data.Name = name;

	s32 idx = ProfileDatas.binary_search(data);
	if ( idx < 0 )
	{
		ProfileDatas.push_back(data);
		ProfileDatas.sort();
	}
	else
	{
		ProfileDatas[idx] = data;
	}
}

s32 CProfiler::addGroup(const core::stringw &name)
{
    SProfileData group;
	group.Id = -1;
    group.Name = name;
    ProfileGroups.push_back(group);
    return (s32)(ProfileGroups.size()-1);
}

void CProfiler::start(s32 id)
{
	s32 idx = ProfileDatas.binary_search(SProfileData(id));
	if ( idx >= 0 && IrrTimer )
	{
		ProfileDatas[idx].LastTimeStarted = IrrTimer->getRealTime();
	}
}

void CProfiler::stop(s32 id)
{
	u32 timeNow = IrrTimer->getRealTime();
	s32 idx = ProfileDatas.binary_search(SProfileData(id));
	if ( idx >= 0 && IrrTimer )
	{
		SProfileData &data = ProfileDatas[idx];
		if ( data.LastTimeStarted == 0 )
			return;

		++data.CountCalls;
		u32 diffTime = timeNow - data.LastTimeStarted;
		data.TimeSum += diffTime;
		if ( diffTime > data.LongestTime )
			data.LongestTime = diffTime;
		if ( diffTime < data.ShortestTime )
			data.ShortestTime = diffTime;
		data.LastTimeStarted = 0;


		if ( data.GroupIndex >= 0 )
		{
			SProfileData & group = ProfileGroups[data.GroupIndex];
			++group.CountCalls;
			group.TimeSum += diffTime;
			if ( diffTime > group.LongestTime )
				group.LongestTime = diffTime;
			if ( diffTime < group.ShortestTime )
				group.ShortestTime = diffTime;
			group.LastTimeStarted = 0;
		}
	}
}

void CProfiler::reset(s32 id)
{
	s32 idx = ProfileDatas.binary_search(SProfileData(id));
    if ( idx >= 0 )
    {
		SProfileData &data = ProfileDatas[idx];

		if ( data.GroupIndex >= 0 )
		{
			SProfileData & group = ProfileGroups[data.GroupIndex];
            group.CountCalls -= data.CountCalls;
            group.TimeSum -= data.TimeSum;
        }

        data.reset();
    }
}

void CProfiler::resetAll()
{
	for ( u32 i=0; i<ProfileDatas.size(); ++i )
    {
		ProfileDatas[i].reset();
    }

	for ( u32 i=0; i<ProfileGroups.size(); ++i )
    {
		ProfileGroups[i].reset();
    }
}

void CProfiler::printAll(core::stringw &ostream, bool includeOverview, bool suppressUncalled) const
{
	if (!IrrTimer)
	{
		ostream += L"WARNING: Profiler is not initialized\n";
	}

    ostream += SProfileData::makeTitleString();
    ostream += L"\n";
	for ( u32 i=includeOverview ?0:1; i<ProfileGroups.size(); ++i )
    {
        printGroup( ostream, ProfileGroups[i].Id, suppressUncalled );
    }
}

void CProfiler::printGroup(core::stringw &ostream, s32 idxGroup, bool suppressUncalled) const
{
    ostream += ProfileGroups[idxGroup].getAsString();
    ostream += L"\n";

	// print overview for groups
    if ( idxGroup == 0 )
    {
		for ( u32 i=0; i<ProfileGroups.size(); ++i )
        {
            if ( !suppressUncalled || ProfileGroups[i].CountCalls > 0)
            {
                ostream += ProfileGroups[i].getAsString();
                ostream += L"\n";
            }
        }
    }
	// print all data in a group
    else
    {
		for ( u32 i=0; i<ProfileDatas.size(); ++i )
        {
            if ( (!suppressUncalled || ProfileDatas[i].CountCalls > 0)
				&& ProfileDatas[i].GroupIndex == idxGroup )
            {
                ostream += ProfileDatas[i].getAsString();
                ostream += L"\n";
            }
        }
    }
}

//! Class where the objects profile their own life-time.
class CProfileScope
{
public:
	CProfileScope(CProfiler& profiler, s32 id, const core::stringw &name, const core::stringw &groupName)
	: Profiler(profiler), Id(id)
	{
		Profiler.add(Id, name, groupName);
		Profiler.start(Id);
	}

	CProfileScope(CProfiler& profiler, const core::stringw &name, const core::stringw &groupName)
	: Profiler(profiler)
	{
		Id = Profiler.add(name, groupName);
		Profiler.start(Id);
	}

	~CProfileScope()
	{
		Profiler.stop(Id);
	}

protected:
	CProfiler& Profiler;
	s32 Id;
};

//! Access the Irrlicht profiler object.
/** Existence is guaranteed from the start of main - but _not_ before the main() call
(this is still up for discussion and might be extended at some point). Behavior before
start of main is so far undefined and might eat your neighbors dog.

If you want to get internal profiling information about the engine itself
you will have to re-compile the engine with _IRR_COMPILE_WITH_PROFILING_ enabled.
But you can use the profiler for profiling your own projects without that. */
IRRLICHT_API CProfiler* IRRCALLCONV getProfiler();

} // namespace irr

//! Code inside IRR_PROFILE is only executed when _IRR_COMPILE_WITH_PROFILING_ is set
//! This allows disabling all profiler code used inside Irrlicht.
//! It might be useful to wrap all your profiler-calls in your own code with a similar macro.
#ifdef _IRR_COMPILE_WITH_PROFILING_
	#define IRR_PROFILE(X) X
#else
	#define IRR_PROFILE(X)
#endif // IRR_PROFILE

#endif // __PROFILER_H_INCLUDED__
