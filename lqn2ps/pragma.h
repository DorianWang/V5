/* -*- c++ -*-
 * $HeadURL: http://rads-svn.sce.carleton.ca:8080/svn/lqn/trunk-V5/lqn2ps/pragma.h $
 *
 * Pragma processing.
 *
 * Copyright the Real-Time and Distributed Systems Group,
 * Department of Systems and Computer Engineering,
 * Carleton University, Ottawa, Ontario, Canada. K1S 5B6
 *
 * November, 1994
 *
 * $Id: pragma.h 14337 2021-01-05 11:32:10Z greg $
 *
 * ------------------------------------------------------------------------
 */

#if	!defined(PRAGMA_H)
#define	PRAGMA_H

#include <string>
#include <map>
#include <lqio/dom_pragma.h>
#include <lqio/input.h>
#include "lqn2ps.h"

/* -------------------------------------------------------------------- */

class Pragma {

public:
    typedef void (Pragma::*fptr)(const std::string&);

    typedef enum { BCMP_STANDARD, BCMP_EXTENDED, BCMP_LQN } pragma_bcmp;
    
private:
    Pragma();
    virtual ~Pragma() 
	{
	    __cache = nullptr;
	}

    static void set_pragma( const std::pair<std::string,std::string>& p );
    
public:
    static pragma_bcmp getBCMP();
    static layering_format layering();
    static bool defaultProcessorScheduling() { assert( __cache != nullptr ); return __cache->_default_processor_scheduling; }
    static scheduling_type processorScheduling() { assert( __cache != nullptr ); return __cache->_processor_scheduling; }
    static bool prune();
    static LQIO::severity_t severityLevel();
    static bool spexHeader();
    static bool defaultTaskScheduling() { assert( __cache != nullptr ); return __cache->_default_task_scheduling; }
    static scheduling_type taskScheduling() { assert( __cache != nullptr ); return __cache->_task_scheduling; }

private:
    void setBCMP(const std::string&);
    void setLayering(const std::string&);
    void setProcessorScheduling(const std::string&);
    void setPrune(const std::string&);
    void setSeverityLevel(const std::string&);
    void setSpexHeader(const std::string&);
    void setTaskScheduling(const std::string&);
    
public:
    static void set( const std::map<std::string,std::string>& );
    static std::ostream& usage( std::ostream&  );
    static const std::map<std::string,Pragma::fptr>& getPragmas() { return __set_pragma; }
    
private:
    static void initialize();

private:
    scheduling_type _processor_scheduling;
    scheduling_type _task_scheduling;
    /* bonus */
    bool _default_processor_scheduling;
    bool _default_task_scheduling;
    
    static Pragma * __cache;
    static std::map<std::string,Pragma::fptr> __set_pragma;
    
    static std::map<std::string,pragma_bcmp> __bcmp_pragma;
    static std::map<std::string,layering_format> __layering_pragma;
    static std::map<std::string,scheduling_type> __processor_scheduling_pragma;
    static std::map<std::string,LQIO::severity_t> __serverity_level_pragma;
    static std::map<std::string,scheduling_type> __task_scheduling_pragma;
};
#endif