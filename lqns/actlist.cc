/* actlist.cc   -- Greg Franks Thu Feb 20 1997
 * $HeadURL: http://rads-svn.sce.carleton.ca:8080/svn/lqn/trunk-V5/lqns/actlist.cc $
 *
 * Everything you wanted to know about connecting activities, but were afraid to ask.
 *
 * Copyright the Real-Time and Distributed Systems Group,
 * Department of Systems and Computer Engineering,
 * Carleton University, Ottawa, Ontario, Canada. K1S 5B6
 *
 * February 1997
 *
 * ------------------------------------------------------------------------
 * $Id: actlist.cc 13912 2020-10-02 12:46:57Z greg $
 * ------------------------------------------------------------------------
 */


#include "dim.h"
#include <map>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <lqio/input.h>
#include <lqio/error.h>
#if HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#include "errmsg.h"
#include "lqns.h"
#include "actlist.h"
#include "activity.h"
#include "entry.h"
#include "task.h"
#include "processor.h"
#include "call.h"
#include "submodel.h"
#include "model.h"
#include "entrythread.h"
#include "pragma.h"
#include "option.h"

/* -------------------------------------------------------------------- */
/*               Activity Lists -- Abstract Superclass                  */
/* -------------------------------------------------------------------- */


ActivityList::ActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : _task(owner), _dom(dom)
{
    owner->addPrecedence(this);
}



/*
 * Return the next link.
 */

ActivityList*
ActivityList::next() const
{
    throw should_not_implement( "ActivityList::next", __FILE__, __LINE__ );
    return 0;
}


/*
 * Return the previous link.
 */

ActivityList*
ActivityList::prev() const
{
    throw should_not_implement( "ActivityList::prev", __FILE__, __LINE__ );
    return 0;
}


/*
 * Set the next link.
 */

ActivityList&
ActivityList::next( ActivityList * )
{
    throw should_not_implement( "ActivityList::next", __FILE__, __LINE__ );
    return *this;
}


/*
 * Set the prev link.
 */

ActivityList&
ActivityList::prev( ActivityList * )
{
    throw should_not_implement( "ActivityList::addSubList", __FILE__, __LINE__ );
    return *this;
}

/* -------------------------------------------------------------------- */
/*                       Simple Activity Lists                          */
/* -------------------------------------------------------------------- */

bool
SequentialActivityList::operator==( const ActivityList& operand ) const
{
    const SequentialActivityList * anOperand = dynamic_cast<const SequentialActivityList *>(&operand);
    if ( anOperand ) {
        return anOperand->_activity == _activity;
    }
    return false;
}


SequentialActivityList&
SequentialActivityList::add( Activity * anActivity )
{
    _activity = anActivity;
    return *this;
}

/* -------------------------------------------------------------------- */
/*                       Simple Forks (rvalues)                         */
/* -------------------------------------------------------------------- */

ForkActivityList::ForkActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : SequentialActivityList( owner, dom ),
      _prev(nullptr)
{
}


/*
 * Recursively find all children and grand children from `this'.  As
 * we descend down, we bump the depth.  If our path's cross, we have a
 * loop and abort.
 */

unsigned
ForkActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    if ( getActivity() ) {
        return getActivity()->findChildren( callStack, directPath, activityStack, forkStack, rate );
    } else {
        return callStack.depth();
    }
}



/*
 * Generate interlocking table.
 */

unsigned
ForkActivityList::followInterlock( std::deque<const Entry *>& entryStack, const InterlockInfo& globalCalls, const unsigned callingPhase ) const
{
    if ( getActivity() ) {
        return getActivity()->followInterlock( entryStack, globalCalls, callingPhase );
    } else {
        return entryStack.size();
    }
}



/*
 * Follow the path backwards.  Used to set path lists for joins.
 */

Activity::Collect&
ForkActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    if ( getActivity() ) {
        return getActivity()->collect( activityStack, entryStack, data );
    } else {
	return data;
    }
}


/*
 * Return the sum of aFunc.
 */

const Activity::Count_If&
ForkActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    if ( getActivity() ) {
        return getActivity()->count_if( stack, data );
    } else {
        return data;
    }
}


/*
 * Recursively search from this entry to any entry on myServer.
 * When we pop back up the call stack we add all calling tasks
 * for each arc which calls myServer.  The task adder
 * will ignore duplicates.
 *
 * Note: we can't short circuit the search because there may be interlocking
 * on multiple branches.
 */

bool
ForkActivityList::getInterlockedTasks( std::deque<const Entry *>& entryStack, const Entity * myServer,
                                       std::set<const Entity *>& interlockedTasks, const unsigned last_phase ) const
{
    if ( getActivity() ) {
        return getActivity()->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase );
    } else {
        return false;
    }
}



/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait
 */

void
ForkActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    if ( getActivity() ) {
        getActivity()->callsPerform( exec );
    }
}



/*
 * Get the number of concurrent threads
 */

unsigned
ForkActivityList::concurrentThreads( unsigned n ) const
{
    if ( getActivity() ) {
        return getActivity()->concurrentThreads( n );
    } else {
        return n;
    }
}

/* -------------------------------------------------------------------- */
/*                      Simple Joins (lvalues)                          */
/* -------------------------------------------------------------------- */

JoinActivityList::JoinActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : SequentialActivityList( owner, dom ),
      _next(nullptr)
{
}


/*
 * Recursively find all children and grand children from `this'.  As
 * we descend down, we bump the depth.  If our path's cross, we have a
 * loop and abort.
 */

unsigned
JoinActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    if ( next() ) {
        return next()->findChildren( callStack, directPath, activityStack, forkStack, rate );
    } else {
        return callStack.depth();
    }
}



/*
 * Link activity.
 */

unsigned
JoinActivityList::followInterlock( std::deque<const Entry *>& entryStack, const InterlockInfo& globalCalls, const unsigned callingPhase ) const
{
    if ( next() ) {
        return next()->followInterlock( entryStack, globalCalls, callingPhase );
    } else {
        return entryStack.size();
    }
}


/*
 * Recursively search from this entry to any entry on myServer.
 * When we pop back up the call stack we add all calling tasks
 * for each arc which calls myServer.  The task adder
 * will ignore duplicates.
 *
 * Note: we can't short circuit the search because there may be interlocking
 * on multiple branches.
 */

bool
JoinActivityList::getInterlockedTasks( std::deque<const Entry *>& entryStack, const Entity * myServer,
                                       std::set<const Entity *>& interlockedTasks, const unsigned last_phase ) const
{
    if ( next() ) {
        return next()->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase );
    } else {
        return false;
    }
}


/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait
 */

void
JoinActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    if ( next() ) {
        next()->callsPerform( exec );
    }
}



/*
 * Get the number of concurrent threads
 */

unsigned
JoinActivityList::concurrentThreads( unsigned n ) const
{
    if ( next() ) {
        return next()->concurrentThreads( n );
    } else {
        return n;
    }
}




/*
 * Follow the path backwards.  Used to set path lists for joins.
 */

Activity::Collect&
JoinActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    if ( next() ) {
        return next()->collect( activityStack, entryStack, data );
    } else {
	return data;
    }
}


/*
 * Return the sum of aFunc.
 */

const Activity::Count_If&
JoinActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    if ( next() ) {
        return next()->count_if( stack, data );
    } else {
        return data;
    }
}

/*----------------------------------------------------------------------*/
/*                  Activity lists that fork or join.                   */
/*----------------------------------------------------------------------*/

ForkJoinActivityList::~ForkJoinActivityList()
{
    _activityList.clear();
}


bool
ForkJoinActivityList::operator==( const ActivityList& operand ) const
{
    const ForkJoinActivityList * anOperand = dynamic_cast<const ForkJoinActivityList *>(&operand);
    if ( anOperand ) {
        // Check cltns for equivalence.
        return anOperand->_activityList == _activityList;
    }
    return false;
}

/*
 * Add an item to the activity list.
 */

ForkJoinActivityList&
ForkJoinActivityList::add( Activity * anActivity )
{
    _activityList.push_back( anActivity );
    return *this;
}



/*
 * Construct a list name.
 */

std::string
ForkJoinActivityList::getName() const
{
    std::string name;
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        if ( activity != activityList().begin() ) {
            name += " ";
            name += typeStr();
            name += " ";
        }
        name += (*activity)->name();
    }
    return name;
}

/* -------------------------------------------------------------------- */
/*          Activity Lists that fork -- abstract superclass             */
/* -------------------------------------------------------------------- */

AndOrForkActivityList::AndOrForkActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : ForkJoinActivityList( owner, dom ),
      _parentForkList(nullptr),
      _joinList(nullptr),
      _prev(nullptr)
{
}



/*
 * Free virtual entries.
 */

AndOrForkActivityList::~AndOrForkActivityList()
{
    for ( std::vector<Entry *>::const_iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry ) {
	delete *entry;
    }
}


/*
 * Configure descendents
 */

AndOrForkActivityList&
AndOrForkActivityList::configure( const unsigned n )
{
    for_each( _entryList.begin(), _entryList.end(), Exec1<Entry,const unsigned>( &Entry::configure, n ) );
    return *this;
}



ActivityList *
AndOrForkActivityList::getNextFork() const
{
    return joinList()->next();
}



bool
AndOrForkActivityList::hasNextFork() const
{
    return joinList() != nullptr && joinList()->next() != nullptr;
}


/*
 * Check that all items in the fork list for this join are the same.
 * If they are zero, tag this join as a synchronization point.
 */

bool
AndOrForkActivityList::check() const
{
    return true;
}



unsigned
AndOrForkActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    forkStack.push_back( this );

    /* Now search down lists */

    unsigned int max_depth = 0;
    try {
	for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
            if ( Options::Debug::forks() ) {
		if ( dynamic_cast<const AndForkActivityList *>(this) ) std::cerr << "And Fork: ";
		else if ( dynamic_cast<const OrForkActivityList *>(this) ) std::cerr << "Or Fork:  ";
		else abort();
		std::cerr << setw( activityStack.size() ) << " " << activityStack.back()->name()
			  << " -> " << (*activity)->name() << endl;
            }
            max_depth = max( (*activity)->findChildren( callStack, directPath, activityStack, forkStack, rate * prBranch( *activity ) ), max_depth );
        }
    }
    catch ( const bad_internal_join& error ) {
	LQIO::solution_error( LQIO::ERR_JOIN_PATH_MISMATCH, owner()->name().c_str(), error.what(), getName().c_str() );
    }
    forkStack.pop_back();
    return max_depth;
}



/*
 * Link activity.  Current path ends here.
 */

unsigned
AndOrForkActivityList::followInterlock( std::deque<const Entry *>& entryStack, const InterlockInfo& globalCalls, const unsigned callingPhase ) const
{
    unsigned max_depth = entryStack.size();
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        const InterlockInfo newCalls = globalCalls * prBranch(*activity);
        max_depth = max( (*activity)->followInterlock( entryStack, newCalls, callingPhase ), max_depth );
    }

    if ( hasNextFork() ) {
	max_depth = max( getNextFork()->followInterlock( entryStack, globalCalls, callingPhase ), max_depth );
    }

    return max_depth;
}



/*
 * Recursively search from this entry to any entry on myServer.
 * When we pop back up the call stack we add all calling tasks
 * for each arc which calls myServer.  The task adder
 * will ignore duplicates.
 *
 * Note: we can't short circuit the search because there may be interlocking
 * on multiple branches.
 */

bool
AndOrForkActivityList::getInterlockedTasks( std::deque<const Entry *>& entryStack, const Entity * myServer,
                                            std::set<const Entity *>& interlockedTasks, const unsigned last_phase ) const
{
    bool found = false;

    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        if ( (*activity)->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase ) ) {
            found = true;
        }
    }

    if ( hasNextFork() && getNextFork()->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase ) ) {
	found = true;
    }
    
    return found;
}



/*
 * Common code for aggregating the activities for a branch to its psuedo entry
 */

Entry *
AndOrForkActivityList::collectToEntry( const Activity * activity, Entry * entry, std::deque<const Activity *>& activityStack, std::deque<Entry *>&entryStack, Activity::Collect& branch )
{
    branch.setRate( branch.rate() * prBranch(activity) );

    entry->set( entryStack.back(), branch );

    entryStack.push_back( entry );
    activity->collect( activityStack, entryStack, branch );
    entryStack.pop_back();

    return entry;
}


std::ostream&
AndOrForkActivityList::printSubmodelWait( std::ostream& output, unsigned offset ) const
{
    for_each( _entryList.begin(), _entryList.end(), ConstPrint1<Entry,unsigned>( &Entry::printSubmodelWait, output, offset ) );
    return output;
}

/* -------------------------------------------------------------------- */
/*                      Or Fork Activity Lists                          */
/* -------------------------------------------------------------------- */

/*
 * Add an OR-fork.  Also add a "virtual" entry for each branch of the fork.
 * Aggregation along each branch will take place into the virtual entry.
 * This class will aggregate the results from each branch into the parent.
 * See Smith[] for the variance stuff.
 */

OrForkActivityList&
OrForkActivityList::add( Activity * anActivity )
{
    ForkJoinActivityList::add( anActivity );

    Entry * anEntry = new VirtualEntry( anActivity );
    assert( anEntry->entryTypeOk(ACTIVITY_ENTRY) );
    anEntry->setStartActivity( anActivity );
    _entryList.push_back( anEntry );
    assert( _entryList.size() == activityList().size() );

    return *this;
}



/*
 * Check that all items in the fork list add to one and they all have probabilities.
 */

bool
OrForkActivityList::check() const
{
    AndOrForkActivityList::check();

    double sum = 0.;
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	sum += prBranch(*activity);
    }
    if ( fabs( 1.0 - sum ) > EPSILON ) {
        LQIO::solution_error( LQIO::ERR_MISSING_OR_BRANCH, getName().c_str(), owner()->name().c_str(), sum );
	return false;
    }
    return true;
}


double
OrForkActivityList::prBranch( const Activity * activity ) const
{
    return getDOM()->getParameterValue(activity->getDOM());
}


/*
 * Recursively aggregate using 'aFunc' along all branches of the or, storing
 * the results of the aggregation in the virtual entry assigned to each branch.
 * (except if aFunc == setThroughput).  Mean and variance is needed to
 * find the variance of the aggregate.
 */

Activity::Collect&
OrForkActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    const unsigned int n = activityList().size();
    const unsigned int submodel = data.submodel();
    Entry * currEntry = entryStack.back();
    unsigned phase = data.phase();
    Activity::Function f = data.collect();

    /* Now search down lists */

    if ( f == &Activity::collectWait ) {
	std::vector<Vector<Exponential> > term( n );
        Vector<Exponential> sum( currEntry->maxPhase() );

        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch( data );
	    const Activity * activity = activityList().at(i);
            Entry * anEntry = collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );

            term[i].resize( currEntry->maxPhase() );
            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                if ( submodel == 0 ) {
		    const double s =  anEntry->_phase[p].elapsedTime();
		    if ( !isfinite( s ) ) continue;			/* Ignore bogus branches */
                    term[i][p].init( s, anEntry->_phase[p].variance() );
                    for ( unsigned j = 0; j < i; ++j ) {
                        sum[p] += varianceTerm( prBranch(activity), term[i][p], prBranch(activityList().at(j)), term[j][p] );
                    }
                } else {
                    term[i][p].mean( anEntry->_phase[p].myWait[submodel] );
                }
                sum[p] += prBranch(activity) * term[i][p];
            }
        }
        for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
            currEntry->aggregate( submodel, p, sum[p] );
        }

    } else if ( f == &Activity::collectReplication ) {
        Vector< VectorMath<double> > sum(currEntry->maxPhase());
        for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
            sum[p].resize( sum[p].size() + currEntry->_phase[p]._surrogateDelay.size() );
        }

        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch( data );
	    const Activity * activity = activityList().at(i);
            Entry * anEntry = collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );

            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                VectorMath<double>& term = anEntry->_phase[p]._surrogateDelay;
                term *= prBranch(activity);
                sum[p] += term;
            }
        }
        currEntry->aggregateReplication( sum );

    } else if ( f == &Activity::collectServiceTime ) {
        VectorMath<double> sum( currEntry->maxPhase() );

        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch( data );
	    const Activity * activity = activityList().at(i);
            Entry * anEntry = collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );

            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                sum[p] += prBranch(activity) * anEntry->_phase[p].serviceTime();
            }
        }
        for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
            currEntry->addServiceTime( p, sum[p] );
        }

    } else {
        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch( data );
	    const Activity * activity = activityList().at(i);
            collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );
        }
    }

    /* Now follow the activities after the join */

    assert( phase == data.phase() );		/* Phase change should not occur on branch */

    if ( hasNextFork() ) {
	data.setRate( data.rate() * joinList()->getNextRate() );	/* May not be 1. */
        getNextFork()->collect( activityStack, entryStack, data );
    }

    return data;
}



/*
 * Return the sum of aFunc.
 */

const Activity::Count_If&
OrForkActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    double sum = 0.0;
    unsigned phase = data.phase();
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	Activity::Count_If branch(data);
	branch.setReplyAllowed(false);
	branch.setRate( data.rate() * prBranch(*activity) );
	branch = (*activity)->count_if( stack, branch );		/* only want the last one. */
	sum += branch.sum();
	phase = std::max( phase, branch.phase() );
    }
    assert( phase == data.phase() );

    /* Now follow the activities after the join */

    data = sum;
    data.setPhase( phase );
    if ( hasNextFork() ) {
	data.setRate( data.rate() * joinList()->getNextRate() );	/* May not be 1. */
	getNextFork()->count_if( stack, data );
    }
    return data;
}



/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait
 */

void
OrForkActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	(*activity)->callsPerform( Phase::CallExec( exec, prBranch(*activity) ) );
    }

    if ( hasNextFork() ) {
        getNextFork()->callsPerform( exec );
    }
}



/*
 * Get the number of concurrent threads
 */

unsigned
OrForkActivityList::concurrentThreads( unsigned n ) const
{
    const unsigned m = n;

    /* Now search down lists */
    
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        n = max( (*activity)->concurrentThreads( m ), n );
    }

    if ( hasNextFork() ) {
        return getNextFork()->concurrentThreads( n );
    }
    return n;
}

/* -------------------------------------------------------------------- */
/*                      And Fork Activity Lists                         */
/* -------------------------------------------------------------------- */


AndForkActivityList::AndForkActivityList( Task * owner, LQIO::DOM::ActivityList * dom  )
    : AndOrForkActivityList( owner, dom ),
      _joinDelay(0.0),
      _joinVariance(0.0)
{
}




/*
 * Add an AND-fork.  Also add a "thread" entry for each branch of the fork.
 * Aggregation along each branch will take place into the thread.  The
 * threads are also used as chains in the MVA models.
 */

AndForkActivityList&
AndForkActivityList::add( Activity * anActivity )
{
    ForkJoinActivityList::add( anActivity );

    Thread * aThread = new Thread( anActivity, this );
    assert( aThread->entryTypeOk(ACTIVITY_ENTRY) );
    aThread->setStartActivity( anActivity );
    _entryList.push_back( aThread );
    assert( _entryList.size() == activityList().size() );

    Task * aTask = const_cast<Task *>(dynamic_cast<const Task *>(anActivity->owner()));    /* Downcase/unconst */

    aTask->addThread( aThread );

    return *this;
}


/*
 * Check that all items in the fork list for this join are the same.
 * If they are zero, tag this join as a synchronization point.
 */

bool
AndForkActivityList::check() const
{
    return AndOrForkActivityList::check();
}


/*
 * Determine if this fork is a descendent of aParent.
 */

bool
AndForkActivityList::isDescendentOf( const AndForkActivityList * aParent ) const
{
    return _parentForkList == aParent || (_parentForkList != nullptr && _parentForkList->isDescendentOf( aParent ));
}




/*
 * Recursively find all children and grand children from `this'.  As
 * we descend down, we bump the depth.  If our path's cross, we have a
 * loop and abort.
 */

unsigned
AndForkActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    _parentForkList = nullptr;
    for ( std::deque<const AndOrForkActivityList *>::const_reverse_iterator forkList = forkStack.rbegin(); forkList != forkStack.rend() && !_parentForkList; ++forkList ) {
	if ( dynamic_cast<const AndForkActivityList *>(*forkList) != nullptr ) _parentForkList = dynamic_cast<const AndForkActivityList *>(*forkList);
    }

    return AndOrForkActivityList::findChildren( callStack, directPath, activityStack, forkStack, rate );
}


/*
 * Check my list for a match on activity.  If so, insert it into the list and keep going up.
 */

void
AndOrForkActivityList::backtrack( const std::deque<const AndOrForkActivityList *>& forkStack, std::set<const AndOrForkActivityList *>& forkList ) const
{
    const std::deque<const AndOrForkActivityList *>::const_iterator offset = std::find( forkStack.begin(), forkStack.end(), this );
    if ( offset != forkStack.end() ) forkList.insert( this );
    prev()->backtrack( forkStack, forkList );
}


/*
 * Aggregate subchains into their psuedo entries.  When all branches
 * haved joined aggregation into this entry will continue by the join.
 * We need the mean and variance of all the branches to calculate the
 * mean and/or variance of the parent.  Note that this aggregate
 * function will sequence to the NEXT activity after the join once all
 * forks have been evaluated (which is different from the behaviour of
 * the OR-fork).
 *
 * Phases cause some grief... the join term is added to the next_p
 * (next_phase).  We have to do some trickery to figure out the phase
 * 1 and 2 waits if a reply occurs on an AND branch.  When the entity
 * is acting as a server, the time spent in the "other threads" shows
 * up as the synchronization delay, so this delay has to be stored by
 * phase.
 */

Activity::Collect&
AndForkActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    const unsigned int submodel = data.submodel();
    const unsigned n = activityList().size();
    Entry * currEntry = entryStack.back();
    unsigned phase = data.phase();
    Activity::Function f = data.collect();

    if (flags.trace_quorum) {
        cout <<"\nAndForkActivityList::collect()...the start --------------- : submodel = " << submodel <<  endl;
    }

    // Start tomari: Quorum
    //The current code assumes that every AndFork corresponds to an AndJoin.
    //So the aggregation is done only in the ForkList.

    if ( f == &Activity::collectWait ) {
        DiscreteCDFs  quorumCDFs;
        DiscreteCDFs  localCDFs;
        DiscreteCDFs  remoteCDFs;
        bool isQuorumDelayedThreadsActive = false;
        double totalParallelLocal = 0;
        double totalSequentialLocal = 0;

        /* Calculate start time */

        double time = 0.0;
        if ( submodel != 0 ) {
            time = currEntry->getStartTime();
            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                time += currEntry->_phase[p].myWait[submodel]; /* Pick off time for this pass. - (since day 1!) */
            }
        } else {
            time = currEntry->getStartTimeVariance();
            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                time += currEntry->_phase[p].variance();       /* total isn't known until all activities scanned. */
            }
        }

        /* Now search down lists */

        Exponential phase_one;
	const AndJoinActivityList * joinList = dynamic_cast<const AndJoinActivityList *>(this->joinList());
        const bool isThereQuorumDelayedThreads = joinList && joinList->hasQuorum();

        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch( data );
	    const Activity * activity = activityList().at(i);
            Thread * anEntry = dynamic_cast<Thread *>(collectToEntry( activity, _entryList[i], activityStack, entryStack, branch ));
            phase = max( phase, branch.phase() );

            Vector<Exponential> term( currEntry->maxPhase() );

            anEntry->startTime( submodel, time );

            if ( submodel == 0 ) {

                /* Updating variance */

                DiscretePoints sumTotal;              /* BUG 583 -- we don't care about phase */
                DiscretePoints sumLocal;
                DiscretePoints sumRemote;

                anEntry->_total.myVariance = 0.0;
                for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                    anEntry->_total.myVariance += anEntry->_phase[p].variance();
                    if (flags.trace_quorum) {
                        cout <<"\nEntry " << anEntry->name() << ", anEntry->elapsedTime(p="<<p<<")=" << anEntry->_phase[p].elapsedTime() << endl;
                        cout << "anEntry->phase[p="<<p<<"].myWait[submodel=1]=" << anEntry->_phase[p].myWait[1] << endl;
                        cout << "anEntry->Entry::variance(p="<<p<<"]="<< anEntry->_phase[p].variance() << endl;
                    }

                    term[p].init( anEntry->_phase[p].elapsedTime(), anEntry->_phase[p].variance() );
                    sumTotal += term[p];            /* BUG 583 */
                }
                // tomari: first possible update for Quorum.

                const_cast<Activity *>(activity)->estimateQuorumJoinCDFs(sumTotal, quorumCDFs,
									 localCDFs, remoteCDFs,
									 isThereQuorumDelayedThreads,
									 isQuorumDelayedThreadsActive,
									 totalParallelLocal,
									 totalSequentialLocal);

            } else if ( submodel == Model::sync_submodel ) {

                /* Updating join delays */

                DiscretePoints sumTotal;              /* BUG 583 -- we don't care about phase */
                DiscretePoints sumLocal;
                DiscretePoints sumRemote;

                anEntry->_total.myWait[submodel] = 0.0;
                for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                    anEntry->_total.myWait[submodel] += anEntry->_phase[p].myWait[submodel];
                    if (flags.trace_quorum) {
                        cout <<"\nEntry " << anEntry->name() <<", anEntry->elapsedTime(p="<<p<<")=" <<anEntry->_phase[p].elapsedTime() << endl;
//                        cout << "anEntry->phase[curr_p="<<curr_p<<"].myWait[submodel="<<2<<"]=" << anEntry->_phase[curr_p].myWait[2] << endl;
                    }

                    term[p].init( anEntry->_phase[p].elapsedTime(), anEntry->_phase[p].variance() );
                    sumTotal += term[p];            /* BUG 583 */
                }

                // tomari: second possible update for Quorum
                const_cast<Activity *>(activity)->estimateQuorumJoinCDFs(sumTotal, quorumCDFs,
									 localCDFs, remoteCDFs,
									 isThereQuorumDelayedThreads,
									 isQuorumDelayedThreadsActive,
									 totalParallelLocal,
									 totalSequentialLocal);

            } else {

                /* Updating the waiting time for this submodel */

                anEntry->_total.myWait[submodel] = 0.0;
                for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                    anEntry->_total.myWait[submodel] += anEntry->_phase[p].myWait[submodel];
                    term[p].init( anEntry->_phase[p].myWait[submodel], anEntry->_phase[p].variance() );
                }
            }

            /* Phase change for this branch, so record the time it occurs. */

            if ( phase != data.phase() ) {
                phase_one = term[data.phase()];
            }
        }

        //to disable accounting for sequential execution in the quorum delayed threads,
        //set probQuorumDelaySeqExecution to zero.
        //0probQuorumDelaySeqExecution = 0;
        DiscretePoints * join = calcQuorumKofN( submodel, isQuorumDelayedThreadsActive, quorumCDFs );

        /* Need to compute p1 and p2 delay components */

        if ( submodel == 0 ) {
            _joinDelay = join->mean();
            /* Update Variance for parent. */
            if ( phase != data.phase() ) {
                currEntry->aggregate( submodel, data.phase(), phase_one );
                (*join) -= phase_one;
            }
            currEntry->aggregate( submodel, phase, *join );
        } else if ( submodel == Model::sync_submodel ) {
            if (flags.trace_quorum) {
                cout << "\n_joinDelay " << join->mean() << endl;
                cout << "_joinVariance " << join->variance() << endl;
            }

            _joinVariance = join->variance();
            if ( flags.trace_activities ) {
                cout << "Join delay aggregate to ";
                if ( dynamic_cast<VirtualEntry *>(currEntry) ) {
                    cout << " virtual entry ";
                } else {
                    cout << " actual entry ";
                }
                cout << currEntry->name() << ", submodel " << submodel << ", phase " << data.phase() << " wait: " << endl;
            }

            /* Update quorumJoin delay for parent.  Set variance for this fork/join.  */

            if ( phase != data.phase() ) {
                if ( flags.trace_activities ) {
                    cout << *join << ", phase " << phase << " wait: ";
                }

                /* we've encountered a phase change, so try to estimate the phases.  */

                currEntry->aggregate( submodel, data.phase(), phase_one );
                (*join) -= phase_one;
            }

            if ( flags.trace_activities ) {
                cout << *join << endl;
            }
            currEntry->aggregate( submodel, phase, *join );
        }

#if HAVE_LIBGSL && HAVE_LIBGSLCBLAS
        if ( joinList && joinList->hasQuorum()
	     if (totalParallelLocal+ totalSequentialLocal > 0) {
		 double probQuorumDelaySeqExecution = totalParallelLocal/
		     (totalParallelLocal+ totalSequentialLocal);
	     }

             && submodel == Model::sync_submodel
             && !flags.disable_expanding_quorum_tree /*!pragmaQuorumDistribution.count_if(DISABLE_EXPANDING_QUORUM)*/
             && pragma.getQuorumDelayedCalls() == KEEP_ALL_QUORUM_DELAYED_CALLS ) {
            saveQuorumDelayedThreadsServiceTime(entryStack,*join,quorumCDFs,
                                                localCDFs,remoteCDFs,
                                                probQuorumDelaySeqExecution);
        }
#endif

        delete join;

    } else if ( f == &Activity::collectServiceTime ) {

        /* For service time, just sum everthing up. */

        VectorMath<double> sum( currEntry->maxPhase() );
        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch(data);
	    const Activity * activity = activityList().at(i);
            Entry * anEntry = collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );

            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                sum[p] += anEntry->_phase[p].serviceTime();
            }
        }
        for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
            currEntry->addServiceTime( p, sum[p] );
        }

    } else {

        for ( unsigned i = 0; i < n; ++i ) {
	    Activity::Collect branch(data);
	    const Activity * activity = activityList().at(i);
            collectToEntry( activity, _entryList[i], activityStack, entryStack, branch );
            phase = max( phase, branch.phase() );
        }
    }

    /* Now follow the activities after the join */

    data.setPhase( phase );
    if ( hasNextFork() ) {
        getNextFork()->collect( activityStack, entryStack, data );
    }

    if (flags.trace_quorum) {
        cout <<"AndForkActivityList::collect()...the end --------------- : submodel = " << submodel <<  endl;
    }
    return data;
}


//Get the CDF of the joint distribution for a K out of N quorum

DiscretePoints *
AndForkActivityList::calcQuorumKofN( const unsigned submodel,
                                     bool isQuorumDelayedThreadsActive,
                                     DiscreteCDFs & quorumCDFs ) const
{
    const unsigned n = activityList().size();
    DiscretePoints * join;

    if (flags.trace_quorum) {
        cout << "\nAndForkActivityList::calcQuorumKofN(): submodel=" <<submodel<< endl;
    }
    const AndJoinActivityList * joinList = dynamic_cast<const AndJoinActivityList *>(this->joinList());
    if ( joinList ) {
        if (joinList->quorumCount() == 0) {
            const_cast<AndJoinActivityList *>(joinList)->quorumCount(n);
        }

        if ( isQuorumDelayedThreadsActive) {
            join = quorumCDFs.quorumKofN( joinList->quorumCount() + 1, n + 1 );
            if (flags.trace_quorum) {
                cout << "quorum (AndJoin) of " <<joinList->quorumCount() + 1
                     << " out of " << n + 1 << endl;
            }
        } else {
            join = quorumCDFs.quorumKofN(joinList->quorumCount(),n );
            if (flags.trace_quorum) {
                cout <<"quorum of " <<joinList->quorumCount() << " out of " << n << endl;
            }
        }

        if (flags.trace_quorum) {
            cout <<"quorumJoin mean=" <<join->mean()
                 << ", Variance=" << join->variance() << endl;
        }
    } else {
        /* BUG 327 */
        join = quorumCDFs.quorumKofN( n, n );
    }

    if ( submodel == Model::sync_submodel ) {
        for ( std::vector<Entry *>::const_iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry ) {
            if ( dynamic_cast<Thread *>(*entry) ) {
                dynamic_cast<Thread *>(*entry)->joinDelay(join->mean());
            }
        }
    }

    return join;
}


#if HAVE_LIBGSL
bool
AndForkActivityList::saveQuorumDelayedThreadsServiceTime( Stack<Entry *>& entryStack,
                                                          DiscretePoints & quorumJoin,
                                                          DiscreteCDFs & quorumCDFs,
                                                          DiscreteCDFs & localCDFs,
                                                          DiscreteCDFs & remoteCDFs,
                                                          double probQuorumDelaySeqExecution )
{
    if (flags.trace_quorum) {
        cout <<"\n'''''''''''start saveQuorumDelayedThreadsServiceTime'''''''''''''''''''''''" << endl;
    }
    const unsigned n = activityList().size();
    bool anError= false;
    Activity * localQuorumDelayActivity = NULL;
    Entry * currEntry = entryStack.back();

    unsigned orgSubmodel = currEntry->owner()->submodel();
    orgSubmodel++;

    DiscretePoints * localQuorumJoin = localCDFs.quorumKofN(joinList->quorumCount(),n );
    DiscretePoints * localAndJoin = localCDFs.quorumKofN( n, n );
    DiscretePoints localDiffJoin;
    localDiffJoin.mean( abs(localAndJoin->mean() -localQuorumJoin->mean()));
    localDiffJoin.variance(abs(localAndJoin->variance() + localQuorumJoin->variance() ));

    if (flags.trace_quorum) {
        cout << "localAndJoin->mean() = " << localAndJoin->mean() <<
            ", Variance = " << localAndJoin->variance() << endl; ;
        cout << "localQuorumJoin->mean() = " << localQuorumJoin->mean() <<
            ", Variance = " << localQuorumJoin->variance() << endl;
        cout << "localDiffJoin.mean() = " << localDiffJoin.mean() <<
            ", Variance = " << localDiffJoin.variance() << endl;
    }
    delete localAndJoin;
    delete localQuorumJoin;

    DiscretePoints * remoteQuorumJoin = remoteCDFs.quorumKofN(joinList->quorumCount(),n );
    DiscretePoints * remoteAndJoin = remoteCDFs.quorumKofN(n,n );
    DiscretePoints remoteDiffJoin;
    remoteDiffJoin.mean(abs(remoteAndJoin->mean() -remoteQuorumJoin->mean()));
    remoteDiffJoin.variance(abs(remoteAndJoin->variance() + remoteQuorumJoin->variance()) );

    if (flags.trace_quorum) {
        cout << "\nremoteAndJoin->mean() = " << remoteAndJoin->mean() <<
            ", Variance = " << remoteAndJoin->variance() << endl; ;
        cout << "remoteQuorumJoin->mean() = " << remoteQuorumJoin->mean() <<
            ", Variance = " << remoteQuorumJoin->variance() << endl;
        cout << "remoteDiffJoin.mean() = " << remoteDiffJoin.mean() <<
            ", Variance = " << remoteDiffJoin.variance() << endl;
    }
    delete remoteAndJoin;
    delete remoteQuorumJoin;

    DiscretePoints * quorumAndJoin = quorumCDFs.quorumKofN(n,n );
    DiscretePoints quorumDiffJoin;
    quorumDiffJoin.mean(abs(quorumAndJoin->mean() -quorumJoin.mean()));
    quorumDiffJoin.variance(abs(quorumAndJoin->variance() + quorumJoin.variance() ));

    if (flags.trace_quorum) {
        cout << "\nquorumAndJoin->mean() = " << quorumAndJoin->mean() <<
            ", Variance = " << quorumAndJoin->variance() << endl;
        cout << "quorumJoin.mean() = " << quorumJoin.mean() <<
            ", Variance = " << quorumJoin.variance() << endl;
        cout << "quorumDiffJoin.mean() = " << quorumDiffJoin.mean() <<
            ", Variance = " << quorumDiffJoin.variance() << endl;
    }
    delete quorumAndJoin;

    char localQuorumDelayActivityName[32];
    sprintf( localQuorumDelayActivityName, "localQmDelay_%d", joinList->quorumListNum() );
    localQuorumDelayActivity = owner()->findActivity(localQuorumDelayActivityName);

    if (localQuorumDelayActivity != NULL) {

        DeviceEntry * procEntry = dynamic_cast<DeviceEntry *>(const_cast<Entry *>(localQuorumDelayActivity->processorCall()->dstEntry()));

        if (!flags.ignore_overhanging_threads) {
	    double service_time = (1-probQuorumDelaySeqExecution) *  localDiffJoin.mean();
            localQuorumDelayActivity->setServiceTime( service_time );
	    procEntry->setServiceTime( service_time  / localQuorumDelayActivity->numberOfSlices() )
		.setCV_sqr( ((1-probQuorumDelaySeqExecution) * localDiffJoin.variance())/square(service_time) );
            procEntry->_phase[1].myWait[orgSubmodel] = (1-probQuorumDelaySeqExecution) * localDiffJoin.mean() ;
        } else {
            localQuorumDelayActivity->setServiceTime(0);
	    procEntry->setServiceTime( 0. ).setCV_sqr( 1. );
            procEntry->_phase[1].myWait[orgSubmodel] = 0.;
        }

        if (flags.trace_quorum) {
            cout <<" procEntry->_phase[1].myWait[orgSubmodel="<<orgSubmodel<<"]="<< procEntry->_phase[1].myWait[orgSubmodel] << endl;
        }

        if (!flags.ignore_overhanging_threads) {
            localQuorumDelayActivity->remoteQuorumDelay.mean(
                remoteDiffJoin.mean() + localDiffJoin.mean() * probQuorumDelaySeqExecution );
            localQuorumDelayActivity->remoteQuorumDelay.variance(
                remoteDiffJoin.variance() + localDiffJoin.variance() * probQuorumDelaySeqExecution );
        } else {
            localQuorumDelayActivity->remoteQuorumDelay.mean(0);
            localQuorumDelayActivity->remoteQuorumDelay.variance(0);
        }
        if (flags.trace_quorum) {
            cout <<"orgSubmodel="<<orgSubmodel<< endl;
            cout << "0. localQuorumDelayActivity->remoteQuorumDelay: mean =" << localQuorumDelayActivity->remoteQuorumDelay.mean()
                 << ", Variance=" << localQuorumDelayActivity->remoteQuorumDelay.variance() << endl;
            cout <<"probQuorumDelaySeqExecution=" <<probQuorumDelaySeqExecution << endl;
        }

    } else {
	throw logic_error( "AndForkActivityList::saveQuorumDelayedThreadsServiceTime" );
        anError = true;
    }

    if (flags.trace_quorum) {
        cout <<"\n'''''''''''end saveQuorumDelayedThreadsServiceTime'''''''''''''''''''''''" << endl;
    }

    return !anError;
}
#endif

/*
 * Return the sum of aFunc.
 */

const Activity::Count_If&
AndForkActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    double sum = 0.0;
    unsigned phase = data.phase();
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	Activity::Count_If branch( data );
	const AndJoinActivityList * joinList = dynamic_cast<const AndJoinActivityList *>(this->joinList());
	branch.setReplyAllowed(!joinList || !joinList->hasQuorum());		/* Disallow replies quorum on branches */
	branch = (*activity)->count_if( stack, branch );
        sum += branch.sum();
	phase = max( phase, branch.phase() );
    }

    /* Now follow the activities after the join */

    data = sum;
    data.setPhase( phase );
    if ( hasNextFork() ) {
	getNextFork()->count_if( stack, data );
    }
    return data;		/* Result of last branch */
}



/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait.
 * Only follow the root path.  Other paths in the AND-FORK are followed
 * by their thread.
 */

void
AndForkActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    if ( hasNextFork() ) {
        getNextFork()->callsPerform( exec );
    }
}



/*
 * Get the number of concurrent threads
 */

unsigned
AndForkActivityList::concurrentThreads( unsigned n ) const
{
    unsigned m = for_each( activityList().begin(), activityList().end(), Sum1<Activity,unsigned,unsigned>( &Activity::concurrentThreads, 1 ) ).sum();
    n = max( n, m - 1 );

    if ( hasNextFork() ) {
        return getNextFork()->concurrentThreads( n );
    } else {
        return n;
    }
}




/*
 * Locate all joins and print delays.
 */

const AndForkActivityList&
AndForkActivityList::insertDOMResults(void) const
{
    if ( joinList() == nullptr ) return *this;
    LQIO::DOM::AndJoinActivityList * dom = const_cast<LQIO::DOM::AndJoinActivityList *>(dynamic_cast<const LQIO::DOM::AndJoinActivityList *>(joinList()->getDOM()));
    if ( dom == nullptr ) return *this;
    dom->setResultJoinDelay(_joinDelay)
	.setResultVarianceJoinDelay(_joinVariance);
    return *this;
}


std::ostream&
AndForkActivityList::printJoinDelay( std::ostream& output ) const
{
    output << "   " << owner()->name() 
	   << ", Fork: " << getName();
    if ( joinList() != nullptr ) {
	output << " -> Join: " << joinList()->getName();
    } else {
	output << " -> unterminated";
    }
    output << ", join delay=" << _joinDelay << ", variance=" << _joinVariance << std::endl;
    return output;
}

/* -------------------------------------------------------------------- */
/*                    And Or Join Activity Lists                        */
/* -------------------------------------------------------------------- */

AndOrJoinActivityList::AndOrJoinActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : ForkJoinActivityList( owner, dom ),
      _forkList(nullptr),
      _next(nullptr)
{
}


/*
 * Recursively find all children and grand children from `this'.  As
 * we descend down, we bump the depth.  If our path's cross, we have a
 * loop and abort.  This version attempts to match the join with its
 * fork.
 */

unsigned
AndOrJoinActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    const_cast<AndOrJoinActivityList *>(this)->updateRate( activityStack.back(), rate );

    if ( forkList() == nullptr ) {
	std::set<const AndOrForkActivityList *> resultSet(forkStack.begin(),forkStack.end());

	/* Go up all of the branches looking for forks found on forkStack */

	for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	    if ( *activity == activityStack.back() ) continue;		/* No need -- this is resultSet */
	
	    /* Find all forks from this activity that match anything in forkStack */
	
	    std::set<const AndOrForkActivityList *> branchSet;
	    (*activity)->backtrack( forkStack, branchSet );			/* find fork lists on this branch */

	    /* Find intersection of branches */
	
	    std::set<const AndOrForkActivityList *> intersection;
	    std::set_intersection( branchSet.begin(), branchSet.end(),
				   resultSet.begin(), resultSet.end(),
				   std::inserter( intersection, intersection.end() ) );
	    resultSet = intersection;
	}

	/* Result should be all forks that match on all branches.  Take the one closest to top-of-stack */

	const AndJoinActivityList * and_join_list = dynamic_cast<const AndJoinActivityList *>(this);
	if ( resultSet.size() > 0 ) {

	    for ( std::deque<const AndOrForkActivityList *>::const_reverse_iterator fork_list = forkStack.rbegin(); fork_list != forkStack.rend() && forkList() == nullptr; ++fork_list ) {
		/* See if we can find a match on the fork stack in the result set of the right type */
		if ( (resultSet.find( *fork_list ) == resultSet.end())
		     || (dynamic_cast<const AndForkActivityList *>(*fork_list) && dynamic_cast<const AndJoinActivityList *>(this) == nullptr )
		     || (dynamic_cast<const OrForkActivityList *>(*fork_list) && dynamic_cast<const OrJoinActivityList *>(this) == nullptr ) ) continue;
	    
		/* Set type for join */
	    
		if ( and_join_list != nullptr && !const_cast<AndJoinActivityList *>(and_join_list)->joinType( AndJoinActivityList::INTERNAL_FORK_JOIN )) {
		    throw bad_internal_join( *this );
		}

		/* Set the links */

		if ( Options::Debug::forks() ) {
		    if ( dynamic_cast<const AndJoinActivityList *>(this) ) std::cerr << "And ";
		    else if ( dynamic_cast<const OrJoinActivityList *>(this) ) std::cerr << "Or ";
		    else abort();
		    std::cerr << setw( activityStack.size() ) << "Join: " << getName() << " -> Fork: " << (*fork_list)->getName() << std::endl;
		}
		const_cast<AndOrForkActivityList *>(*fork_list)->setJoinList( this );
		const_cast<AndOrJoinActivityList *>(this)->setForkList( *fork_list );	       	/* Will break loop */
	    }
	
	} else if ( (and_join_list != nullptr && !const_cast<AndJoinActivityList *>(and_join_list)->joinType( AndJoinActivityList::SYNCHRONIZATION_POINT ))
		    || dynamic_cast<const OrJoinActivityList *>(this) ) {
	    throw bad_external_join( *this );
	}
    }
    
    /* Carry on */
    
    if ( next() ) {
        return next()->findChildren( callStack, directPath, activityStack, forkStack, rate );
    }
    return callStack.depth();
}



/*
 * return the fork list closest to the bottom of the stack.
 */

void
AndOrJoinActivityList::backtrack( const std::deque<const AndOrForkActivityList *>& forkStack, std::set<const AndOrForkActivityList *>& forkList ) const
{
    for ( std::vector<const Activity *>::const_iterator a = activityList().begin(); a != activityList().end(); ++a ) {
	(*a)->backtrack( forkStack, forkList );		// Will create a union of all reachable matches
    }
}

/* -------------------------------------------------------------------- */
/*                      Or Join Activity Lists                          */
/* -------------------------------------------------------------------- */

bool
OrJoinActivityList::updateRate( const Activity * activity, double rate )
{
    _rateList.insert( std::pair<const Activity *,double>( activity, rate ) );
    _rate = 0;
    for ( std::map<const Activity *,double>::const_iterator i = _rateList.begin(); i != _rateList.end(); ++i ) _rate += i->second;
//    _rate = std::accumulate( _rateList.begin(), _rateList.end(), 0. );
    return true;
}

/* -------------------------------------------------------------------- */
/*                     And Join Activity Lists                          */
/* -------------------------------------------------------------------- */

AndJoinActivityList::AndJoinActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : AndOrJoinActivityList( owner, dom ),
      _joinType(JOIN_NOT_DEFINED),
      myQuorumCount(dom ? dynamic_cast<LQIO::DOM::AndJoinActivityList*>(dom)->getQuorumCountValue() : 0),
      myQuorumListNum(0)
{
}



bool
AndJoinActivityList::joinType( const join_type aType )
{
    if ( _joinType == JOIN_NOT_DEFINED ) {
        _joinType = aType;
        return true;
    } else {
        return aType == _joinType;
    }
}


bool
AndJoinActivityList::check() const
{
    return AndOrJoinActivityList::check();
}
    

/*
 * Follow the path.  We don't care about other paths.
 */

unsigned
AndJoinActivityList::followInterlock( std::deque<const Entry *>& entryStack, const InterlockInfo& globalCalls, const unsigned callingPhase ) const
{
    if ( next() && isSync() ) {
	return next()->followInterlock( entryStack, globalCalls, callingPhase );
    }
    return entryStack.size();
}



/*
 * Recursively search from this entry to any entry on myServer.
 * When we pop back up the call stack we add all calling tasks
 * for each arc which calls myServer.  The task adder
 * will ignore duplicates.
 *
 * Note: we can't short circuit the search because there may be interlocking
 * on multiple branches.
 */

bool
AndJoinActivityList::getInterlockedTasks( std::deque<const Entry *>& entryStack, const Entity * myServer,
                                          std::set<const Entity *>& interlockedTasks, const unsigned last_phase ) const
{
    if ( next() && isSync() ) {
	return next()->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase );
    }
    return false;
}



/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait
 */

void
AndJoinActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    if ( isSync() && next() ) {
        next()->callsPerform( exec );
    }
}



/*
 * If this join is the match for forkList, then stop aggregating as this thread is now done.
 * Otherwise, press on.
 */

Activity::Collect&
AndJoinActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    if ( isSync() && next() ) {
        return next()->collect( activityStack, entryStack, data );
    } else {
	return data;
    }
}



/*
 * If this join is the match for forkList, then stop aggregating as this thread is now done.
 * Otherwise, press on.
 */

const Activity::Count_If&
AndJoinActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    if ( isSync() && next() ) {
        return next()->count_if( stack, data );
    } else {
        return data;
    }
}



/*
 * Get the number of concurrent threads
 */

unsigned
AndJoinActivityList::concurrentThreads( unsigned n ) const
{
    if ( isSync() ) {
        return next()->concurrentThreads( n );
    } else {
        return n;
    }
}

/*----------------------------------------------------------------------*/
/*                           Repetition node.                           */
/*----------------------------------------------------------------------*/

RepeatActivityList::RepeatActivityList( Task * owner, LQIO::DOM::ActivityList * dom )
    : ForkActivityList( owner, dom ), _prev(nullptr)
{
}



RepeatActivityList::~RepeatActivityList()
{
    _activityList.clear();
    for ( std::vector<Entry *>::const_iterator entry = _entryList.begin(); entry != _entryList.end(); ++entry ) {
	delete *entry;
    }
    _entryList.clear();
}




/*
 * Configure descendents
 */

RepeatActivityList&
RepeatActivityList::configure( const unsigned n )
{
    for_each( _entryList.begin(), _entryList.end(), Exec1<Entry,unsigned>( &Entry::configure, n ) );
    return *this;
}



/*
 * Add a sublist.
 */

RepeatActivityList&
RepeatActivityList::add( Activity * anActivity )
{
    const LQIO::DOM::ExternalVariable * arg = const_cast<LQIO::DOM::ActivityList *>(getDOM())->getParameter(const_cast<LQIO::DOM::Activity *>(anActivity->getDOM()));
    if ( arg ) {

        _activityList.push_back(anActivity);

	Entry * anEntry = new VirtualEntry( anActivity );
	_entryList.push_back(anEntry);
        assert( anEntry->entryTypeOk(ACTIVITY_ENTRY) );
        anEntry->setStartActivity( anActivity );

    } else {

        /* End of list */
        ForkActivityList::add( anActivity );

    }

    return *this;
}


double
RepeatActivityList::rateBranch( const Activity * anActivity ) const
{
    return getDOM()->getParameterValue(anActivity->getDOM());
}


Entry *
RepeatActivityList::collectToEntry( const Activity * activity, Entry * entry, std::deque<const Activity *>& activityStack, std::deque<Entry *>&entryStack, Activity::Collect& branch )
{
    branch.setRate( branch.rate() * rateBranch(activity) );

    entry->set( entryStack.back(), branch );

    entryStack.push_back( entry );
    activity->collect( activityStack, entryStack, branch );
    entryStack.pop_back();
    return entry;
}


/*
 * Recursively find all children and grand children from `this'.  As
 * we descend down, we bump the depth.  If our path's cross, we have a
 * loop and abort.
 */

unsigned
RepeatActivityList::findChildren( Call::stack& callStack, bool directPath, std::deque<const Activity *>& activityStack, std::deque<const AndOrForkActivityList *>& forkStack, double rate ) const
{
    unsigned max_depth = ForkActivityList::findChildren( callStack, directPath, activityStack, forkStack, rate );

    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	std::deque<const AndOrForkActivityList *> branchForkStack;    // For matching forks/joins.
        max_depth = max( (*activity)->findChildren( callStack, directPath, activityStack, branchForkStack, rate * rateBranch( *activity ) ), max_depth );
    }

    return max_depth;
}



/*
 * Link activity.
 */

unsigned
RepeatActivityList::followInterlock( std::deque<const Entry *>& entryStack, const InterlockInfo& globalCalls, const unsigned callingPhase ) const
{
    unsigned max_depth = entryStack.size();

    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        const InterlockInfo newCalls = globalCalls * rateBranch(*activity);
        max_depth = max( (*activity)->followInterlock( entryStack, newCalls, callingPhase ), max_depth );
    }

    return max( ForkActivityList::followInterlock( entryStack, globalCalls, callingPhase ), max_depth );
}



/*
 * Recursively search from this entry to any entry on myServer.
 * When we pop back up the call stack we add all calling tasks
 * for each arc which calls myServer.  The task adder
 * will ignore duplicates.
 *
 * Note: we can't short circuit the search because there may be interlocking
 * on multiple branches.
 */

bool
RepeatActivityList::getInterlockedTasks( std::deque<const Entry *>& entryStack, const Entity * myServer,
                                         std::set<const Entity *>& interlockedTasks, const unsigned last_phase ) const
{
    bool found = false;
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        if ( (*activity)->getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase ) ) {
            found = true;
        }
    }

    if ( ForkActivityList::getInterlockedTasks( entryStack, myServer, interlockedTasks, last_phase ) ) {
	found = true;
    }

    return found;
}


/*
 * For all calls to aServer perform aFunc over the chains in nextChain.
 * See Call::setVisits and Call::saveWait
 */

void
RepeatActivityList::callsPerform( const Phase::CallExec& exec ) const
{
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        (*activity)->callsPerform( Phase::CallExec( exec, rateBranch(*activity) ) );
    }

    ForkActivityList::callsPerform( exec );
}


/*
 * Recursively aggregate using 'aFunc' along all branches of the or, storing
 * the results of the aggregation in the virtual entry assigned to each branch.
 */

Activity::Collect&
RepeatActivityList::collect( std::deque<const Activity *>& activityStack, std::deque<Entry *>& entryStack, Activity::Collect& data )
{
    const unsigned int submodel = data.submodel();
    const unsigned int n = activityList().size();
    Entry * currEntry = entryStack.back();
    Activity::Function f = data.collect();

    for ( unsigned i = 0; i < n; ++i ) {
	const Activity * anActivity = activityList().at(i);
	Activity::Collect branch(data);
	Entry * anEntry = collectToEntry( anActivity, _entryList[i], activityStack, entryStack, branch );

        if ( f == &Activity::collectWait ) {
            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                Exponential term;
                Exponential sum;
                if ( submodel == 0 ) {
                    term.init( anEntry->_phase[p].elapsedTime(), anEntry->_phase[p].variance() );
                } else {
                    term.mean( anEntry->_phase[p].myWait[submodel] );
                }
                sum = rateBranch(anActivity) * term + varianceTerm( term );
                currEntry->aggregate( submodel, p, sum );
            }

        } else if ( f == &Activity::collectReplication ) {
            Vector< VectorMath<double> > sum(currEntry->maxPhase());
            for ( unsigned p = 1; p <= currEntry->maxPhase(); ++p ) {
                sum[p] = anEntry->_phase[p]._surrogateDelay;
                sum[p] *= rateBranch(anActivity);
            }
            currEntry->aggregateReplication( sum );

        } else if ( f == &Activity::collectServiceTime ) {
	    currEntry->addServiceTime( data.phase(), rateBranch(anActivity) * anEntry->_phase[data.phase()].serviceTime() );
        }
    }

    /* Do after as there may be a phase change. */

    ForkActivityList::collect( activityStack, entryStack, data );

    return data;
}


/*
 * Return the sum of aFunc.
 * Rate is set to zero for branches so that we don't count replies.
 */

const Activity::Count_If&
RepeatActivityList::count_if( std::deque<const Activity *>& stack, Activity::Count_If& data ) const
{
    data = ForkActivityList::count_if( stack, data );

    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
	Activity::Count_If branch = data;
	data.setReplyAllowed(false);
	branch.setRate( data.rate() * rateBranch(*activity) );
	branch = (*activity)->count_if( stack, branch );
	data += branch.sum();
    }
    return data;
}




/*
 * Get the number of concurrent threads
 */

unsigned
RepeatActivityList::concurrentThreads( unsigned n ) const
{
    for ( std::vector<const Activity *>::const_iterator activity = activityList().begin(); activity != activityList().end(); ++activity ) {
        n = max( n, (*activity)->concurrentThreads( n ) );
    }

    return ForkActivityList::concurrentThreads( n );
}



std::ostream& 
RepeatActivityList::printSubmodelWait( std::ostream& output, unsigned offset ) const
{
    for_each( _entryList.begin(), _entryList.end(), ConstPrint1<Entry,unsigned>( &Entry::printSubmodelWait, output, offset ) );
    return output;
}

/* ------------------------ Exception Handling ------------------------ */

bad_internal_join::bad_internal_join( const ForkJoinActivityList& list )
    : std::runtime_error( list.getName().c_str() )
{
}



bad_external_join::bad_external_join( const ForkJoinActivityList& list )
    : std::runtime_error( list.getName().c_str() )
{
}

/* ---------------------------------------------------------------------- */

/*
 * Connect the src and dst lists together.
 */

void
act_connect ( ActivityList * src, ActivityList * dst )
{
    if ( src ) {
        src->next( dst  );
    }
    if ( dst ) {
        dst->prev( src );
    }
}

void complete_activity_connections ()
{
    /* We stored all the necessary connections and resolved the list identifiers so finalize */
    std::map<LQIO::DOM::ActivityList*, LQIO::DOM::ActivityList*>::iterator iter;
    for (iter = Activity::actConnections.begin(); iter != Activity::actConnections.end(); ++iter) {
	ActivityList* src = Activity::domToNative[iter->first];
	ActivityList* dst = Activity::domToNative[iter->second];
	assert(src != NULL && dst != NULL);
	act_connect(src, dst);
    }
}
