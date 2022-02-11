/* $Id: para_types.h 12547 2016-04-05 18:32:45Z greg $ */
/************************************************************************/
/*	para_types.h - PARASOL library typedef file			*/
/*									*/
/*	Copyright (C) 1993 School of Computer Science, 			*/
/*		Carleton University, Ottawa, Ont., Canada		*/
/*		Written by John Neilson					*/
/*									*/
/*  This library is free software; you can redistribute it and/or	*/
/*  modify it under the terms of the GNU Library General Public		*/
/*  License as published by the Free Software Foundation; either	*/
/*  version 2 of the License, or (at your option) any later version.	*/
/*									*/
/*  This library is distributed in the hope that it will be useful,	*/
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*/
/*  Library General Public License for more details.			*/
/*									*/
/*  You should have received a copy of the GNU Library General Public	*/
/*  License along with this library; if not, write to the Free		*/
/*  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.	*/
/*									*/
/*  The author may be reached at neilson@scs.carleton.ca or through	*/
/*  the School of Computer Science at Carleton University.		*/
/*									*/
/*	Created: 09/06/93 (JEN)						*/
/*	Revised: 21/12/93 (JEN)	Added code field to ps_task_t for run 	*/
/*				off fix.				*/
/*		 27/04/94 (JEN) Added typdefs for buffers and buffer	*/
/*				pools.					*/
/*		 26/07/94 (JEN) Removed psize from ps_bus_t and		*/
/*				ps_link_t.				*/
/*		 06/10/94 (JEN) Added stack_limit to ps_task_t.		*/
/*		 09/12/94 (JEN) Changed all typedef names to minimize 	*/
/*				compatibility problems.			*/
/*		 11/05/95 (JEN) Added org_port field to ps_mess_t to	*/
/*				fix a bug in ps_leave_port_set.		*/
/*		 18/05/95 (PRM)	Added mid field to ps_mess_t to support	*/
/*				unique message ids			*/
/*		 25/05/95 (PRM)	Added did field to ps_mess_t and 	*/
/*				ps_task_t to support angio tracing.	*/
/*		 08/06/95 (PRM)	Added rat struct to ps_stat_t union to	*/
/*				support rate statistics.		*/
/*		 07/07/95 (PRM)	Added uhost and upriority field to	*/
/*				ps_task_t for user defined scheduling	*/
/*									*/
/************************************************************************/

#ifndef	_PARA_TYPES
#define	_PARA_TYPES

#include <cstddef>
#include <setjmp.h>

// I'll replace these since it isn't too hard to do so and it cleans up the warnings.
// Still isn't really safe and standard C++ since it'll have lots of raw memory accesses.
#include <vector>  // Used for dynamic arrays
#include <string>  // Used for names and output
#include <list>    // Used for list of links in nodes
#include <queue>   // Used for message queue in link
#include <cstdint> // Used for fixed size ints.

// Note that classes and structs are equivalent except that struct members are default public.

/* 	Task states and flags						*/

enum ps_task_state_t {
    TASK_FREE,
    TASK_SUSPENDED,
    TASK_READY,
    TASK_HOT,
    TASK_RECEIVING,
    TASK_SLEEPING,
    TASK_SYNC,
    TASK_SYNC_SUSPEND,
    TASK_SYNC_FREE,
    TASK_SPINNING,
    TASK_COMPUTING,
    TASK_BLOCKED
};

// Forwards declaration since I don't want to reorganize that much.
struct rb_node; struct ps_cfs_rq_t; struct sched_info; struct ps_stat_t;


/************************************************************************/

typedef struct mctx_st {
	jmp_buf jb;
} mctx_t;

/************************************************************************/

struct	ps_buf_t {
	long	signature;			/* buffer signature	*/
	long	pool;				/* buffer pool id	*/
		ps_buf_t	*next;		/* next buffer pointer	*/
};

/************************************************************************/

// This should total around 80 bytes, excluding the dynamic string data.
// I think it will still be best to use a pool for them,
// since there's probably a lot of them and they have short lifespans
struct	ps_mess_t {			/* message struct	*/
	//long	state;				/* message state, is this still used?	*/
	int32_t	sender;				/* sender index		*/
	int32_t	port;				/* port index		*/
	int32_t	ack_port;			/* ack port index	*/
	int32_t	org_port;			/* original port index	*/
	int32_t	c_code;				/* comm media code	*/
	int32_t	blid;				/* bus/link index	*/
	int32_t 	type;				/* message type code	*/
	uint32_t	size;				/* message size (bytes), used for bus_delay and link_delay	*/
	double	ts;				/* message time-stamp	*/
	std::string text;				/* message text pointer	*/
	int32_t	mid;				/* Unique message id, should this be 64 bit? Not sure exactly where this is set */
	int32_t	did;				/* dye id		*/
	int32_t	pri;				/* Message priority	*/
	//ps_mess_t	*next;		/* next message pointer, deprecated from use of queue	*/
};

// Might be too much overhead, but we'll be throwing a lot of this out later.
class	ps_message_pool {
public:
	ps_message_pool(){
		storage.resize(baseSize);
		for (unsigned int i = 0; i < baseSize; i++){
			unusedMessageIndex.push(i);
		}
	};
	static constexpr int baseSize = 400;
	static constexpr int growSize = 200;
	ps_mess_t* get_mp(size_t index){
		if (index < storage.size())
			return storage.data();
		return nullptr;
	}; // This pointer may be invalidated if the vector resizes.
	uint32_t get_mess(){
		uint32_t result;
		if (unusedMessageIndex.empty()){
			size_t oldSize = storage.size();
			storage.resize(oldSize + growSize);
			for (unsigned int i = oldSize; i < oldSize + growSize; i++){
				unusedMessageIndex.push(i);
			}
		}
		result = unusedMessageIndex.front();
		unusedMessageIndex.pop();
		return result;
	}
	void free_mess(size_t index){
		if (index < storage.size()){
			unusedMessageIndex.push(index);
		}
		else {
			printf("Trying to free message with invalid index %llu!\n", index);
			// Maybe throw an error I guess.
		}
	}
private:
	std::vector <ps_mess_t> storage;
	std::queue <uint32_t> unusedMessageIndex; // Only 4 billion messages, possible to need more.
};


/************************************************************************/

struct	ps_event_t {			/* PARASOL event struct	*/
	double	time;				/* event time		*/
	long	type;				/* event type code	*/
	long	*gp;				/* generic pointer	*/
	ps_event_t	*next;		/* next event pointer	*/
	ps_event_t	*prior;		/* prior event pointer	*/
};

/************************************************************************/

struct	ps_bus_t {			/* bus struct		*/
	std::string	name;				/* bus name		*/
	long	state;				/* bus state		*/
	long	nnodes;				/* # bus nodes		*/
	long	*bnode;				/* bus node array	*/
	double	trate;				/* transmission rate	*/
	long	discipline;			/* queueing discipline	*/
	std::list <uint32_t>	mq;		/* message list, not queue since random exists	*/
	ps_event_t	*ep;		/* bus event pointer	*/
	long	stat;				/* statistics index	*/
};

/************************************************************************/

// Next step should be to replace the table with a dynamic storage container of some sort.
// Each individual table seems to only hold one type which can be determined at compile time.
// It appears almost exactly like a std::vector<Type> in function, so I'll use a vector as base.
// There does not appear that a table for more than one type exists (like for C-style type punning),
// so it's time to take a dive and break everything.
// A map could also have been an acceptable choice, but I'll stick with vector for now.
template <class T> class ps_table_t {			/* dynamic table */
public:
	ps_table_t(){signature = 0;}; // An invalid signature; INVALID_TABLE
	ps_table_t(long initSize, long nSig = 29776723){
		table.resize(initSize); active_entries.resize(initSize); // vector<bool> initializes to false.
		rover = 0; used = 0; signature = nSig;
	}
	void init(long initSize, long nSig = 29776723){
		table.resize(initSize); active_entries.resize(initSize);
		rover = 0; used = 0; signature = nSig;
	}
	long signature;			/* table signature	*/
	long get_tab_size() const {return table.size();};			/* # of table entries (size of underlying vector)	*/
	long get_used() const {return used;};
	T* get_tab(){return table.data();}; // Yes they are the same.
	T* tab(){return table.data();};
	bool active(long id) const {
		if (id < active_entries.size() && id >= 0){
			return active_entries[id];
		}
		return false;
	};
	long add_entry(){
		if(this->signature != 29776723) // TABLE
			return(-1); //SYSERR
		if (used == table.size()){
			rover = table.size();
			table.resize(table.size() * 2);
			active_entries.resize(table.size());
		}
		size_t id = rover;
		for (size_t i = 0; i < table.size(); i++){
			if (active_entries[id] == false) break;
			id = id + 1 % table.size();
		}
		rover = id + 1 % table.size();
		used++;
		active_entries[id] = true;
		return id;
	};
	long remove_entry(long id){
		if(this->signature != 29776723) // TABLE
			return(-1); // SYSERR
		if (id < table.size() && id >= 0){
			active_entries[id] = false;
			used--;
			return 1; // OK
		}
		return -1; // SYSERR
	};

	//char	*tab;				/* table pointer	*/
	//char	*base;				/* table base		*/
	//long	entry_size; // Equal to tab_size + sizeof(double), which is generally 8 bytes more.
private:
	std::vector <T> table;
	std::vector <bool> active_entries;
	// There are some cool optimizations in bool vectors I won't benefit from here.
	size_t	rover;				/* rover index, equals to index after last get_tabl_entry()	*/
	size_t	used;				/* # of entries used (equal to number with bool set to true)	*/
};

/************************************************************************/

struct	ps_comm_t {			/* communication struct	*/
	long	blid;				/* link/bus index 	*/
	// ps_comm_t	*next;		/* next pointer, deprecated due to std::list	*/
};

/************************************************************************/

struct ps_cpu_t {			/* cpu processor struct	*/
	ps_cpu_t(){
		state = 0; //CPU_IDLE;
		run_task = -1; //NULL_TASK;
		ts_tab = nullptr;
		catcher = -1; //NULL_TASK;
		scheduler = -1; //NULL_TASK;
		last_task = -1; //NULL_TASK;
		stat = -1; //NULL_STAT;
		port_n = 0;
		group_rq = nullptr; //NULL_CFSRQ_PTR;  /* pointer of group rq */
	}; // Default constructor to avoid big for loops.
	long	state;				/* cpu state		*/
	size_t	run_task;			/* running task	index	*/
	long	stat;				/* statistics index	*/
	ps_table_t<long>	*ts_tab;	/* task stats table	*/
	long	scheduler;			/* scheduler task	*/
	long	catcher;			/* catcher task(if any)	*/
	size_t	last_task;			/* last task index	*/
	long 	port_n;				/* # of port		*/
	ps_cfs_rq_t *group_rq;		/* pointer of cfs_rq for each group */
};

/************************************************************************/

struct ps_dye_t {			/* Angio dye struct	*/
	char 	*base_name;			/* base name		*/
	long 	occurence;			/* occurence number	*/
	long	serialno;			/* serial number	*/
};

/************************************************************************/

struct ps_link_t {			/* one-way link struct	*/
	std::string name;				/* link name		*/
	long	state;				/* link state		*/
	long	snode;				/* source node index	*/
	long	dnode;				/* destinat'n node index*/
	double	trate;				/* net transmission rate*/
	std::queue <uint32_t> mq;		/* message queue, since only FIFO	*/
	long	stat;				/* statistics index	*/
	ps_event_t	*ep;		/* link event pointer	*/
} ;

/************************************************************************/

struct	ps_lock_t {			/* lock struct		*/
	long	state;				/* lock state		*/
	size_t	owner;				/* lock owner index	*/
	long	queue;				/* lock queue           */
	long	count;				/* lock queue count	*/
	long	next;				/* next lock index	*/
};

/************************************************************************/

class ps_node_t {			/* processor node struct*/
public:
	std::string name;				/* node name		*/
	long	ncpu;				/* # of cpu's		*/
	long	nfree;				/* # of free cpu's	*/
	std::vector <ps_cpu_t> cpu;		/* array of cpus, owned by node so not pointers	*/
	long	rtrq;				/* ready to run queue	*/
	double	speed;				/* cpu speed factor	*/
	double	quantum;			/* time slice quantum	*/
	long	discipline;			/* queueing discipline	*/
	std::list <ps_comm_t>	sl_list;		/* send link list	*/
	std::list <ps_comm_t>	rl_list;		/* receive link list	*/
	std::list <ps_comm_t>	bus_list;		/* bus list		*/
	long		stat;			/* usage stat		*/
	ps_table_t<long>	*ts_tab;	/* per task usage stats	*/
	long		sf;			/* stats flags		*/
	double	build_time;			/* construction time	*/

	/* the following for cfs scheduler*/
	long  ngroup;				/* # of groups	*/
	ps_cfs_rq_t *host_rq;		/* the pointer of cfs_rq */
	ps_cfs_rq_t *group_rq;		/* pointer of cfs_rq for each group */
};

/************************************************************************/

struct	ps_pool_t {
	long	size;				/* size of user buffers	*/
	ps_buf_t	*flist;		/* free list of buffers	*/
};

/************************************************************************/

struct	ps_tp_pair_t {			/* task port pair struct*/
	long	task;				/* task index		*/
	long	port;				/* port	index		*/
	ps_tp_pair_t *next;		/* next pointer		*/
};

/************************************************************************/

struct  ps_port_t {			/* port struct		*/
	std::string name;				/* port name		*/
	long	state;				/* port state		*/
	//long	nmess;				/* # of queued messages, deprecated by std::queue::size()	*/
	std::list <uint32_t> mq;		/* first message pointer*/
	long	owner;				/* port owner index	*/
	ps_tp_pair_t *tplist;		/* list of tp pairs	*/
	long	next;				/* next port index	*/
};

/************************************************************************/

struct	ps_sema_t {			/* semaphore struct	*/
	long	count;				/* semaphore counter	*/
	ps_tp_pair_t 	*queue;			/* semaphore queue	*/
};

/************************************************************************/

struct	ps_stat_t {			/* statistics struct	*/
	std::string	name;				/* statistic name	*/
	double	resid;				/* non-rounding resid	*/
	union {
	    struct {
		long	count;			/* sample count		*/
		double	sum;			/* sample sum		*/
	    } sam;
	    struct {
		double	start;			/* start time		*/
		double	old_value;		/* previous value	*/
		double	old_time;		/* previous time	*/
		double	integral;		/* variable integral	*/
	    } var;
	    struct {
		double	start;			/* start time		*/
		long	count;			/* # observations	*/
	    } rat;
	} values;
	long	type;				/* statistic type	*/
};

/************************************************************************/

struct  ps_task_t {			/* task struct		*/
	std::string	name;				/* task name		*/
	ps_task_state_t state;			/* task state		*/
	long	node;				/* node location	*/
	long	uhost;				/* user cpu index	*/
	long	host;				/* cpu index		*/
	ps_cpu_t	*hp;		/* host pointer		*/
	double	*stack_base;			/* stack base		*/
	double	*stack_limit;			/* stack limit		*/
	mctx_t	context;			/* task context		*/
	void	(* code)(void *);		/* task code pointer	*/
	long	upriority;			/* user task priority	*/
	long	priority;			/* task priority	*/
	long	next;				/* next task index	*/
	long	parent;				/* task parent index	*/
	long	son;				/* task son index	*/
	long	sibling;			/* task sibling index	*/
	long	port_list;			/* port list 		*/
	long	bport;				/* broadcast port index	*/
	long	blind_port;			/* blind port index	*/
	long	wport;				/* waiting port index	*/
	long	lock_list;			/* lock list 		*/
	long	spin_lock;			/* spin lock index	*/
	ps_event_t	*tep;		/* task event pointer	*/
	double	rct;				/* remaining cpu time	*/
	ps_event_t	*qep;		/* quantum event pointer*/
	long	qx;				/* quantum expired flag	*/
	ps_event_t	*rtoep;		/* receive t/o event ptr*/
	long	did;				/* dye id		*/
	long	tsn;				/* Task serial number	*/
	double	sched_time;			/* Task schedule time	*/
 	double	end_compute_time;		/* Task end compute time */
	long	tbp;				/* break polong info	*/

	/*The following fields are used to calculate the preemption time of a task. Tao*/
	long	pt_tag;				/*preemption tag. 1: preempted, 0: not preempted.*/
	double	pt_last;			/*the latest time that is preempted*/
	double	pt_sum;				/*the total preemption time*/

	/* The followings are for cfs scheduler*/
	long 	group;				/* group index	*/
	long 	group_id;			/* order of group */
	sched_info	*si;		/* contain the cfs schedule information of the task */

};

/************************************************************************/

struct	ps_var_t {
	long	used;				/* is entry used?	*/
	long	width;				/* Width of each elem	*/
	void	*data;				/* Array of elements	*/
};

/************************************************************************/

struct	ps_env_t {
	long	used;				/* is entry used?	*/
	long	ntasks;				/* size of indices array*/
	long	nsubscribers;			/* # of subscribers	*/
	long	nallocated;			/* Allocated vars	*/
	long	*indices;			/* indices array	*/
	ps_table_t<ps_var_t>	var_tab;	/* vars table		*/
};

/************************************************************************/

struct  ps_cfs_rq_t {		/* cfs run queue struct		*/
	long	node;				/* node location	*/
	long 	nready;			/* # of ready to run task */
	rb_node * root;
	rb_node * leftmost;

	/* time parameters */
	double fair;			/* fair key*/
	double exec_time;		/*the exec_time*/
	double wait_to_run;		/* the wait time */
	double sched_time;		/* Task schedule time	*/
    /* double underrun; */
    /* double overrun; */

	sched_info *si;		/* contain the cfs schedule information of the cfs-rq
						if the rq is owned by a group	*/
	long 	load;
	sched_info * curr;
};
/************************************************************************/

struct  ps_group_t {			/* group struct		*/
	char	*name;				/* group name		*/
	long	node;				/* node location	*/
	long	group_id2;		/* the order of group in the node */
	double	share;			/* share value of the group	*/
	long 	cap;			/* cap flag*/

	long	ntask;			/*  # of task in this group	*/
	long 	*task;				/* array of task	*/
	long	stat;			/* usage stat		*/
	ps_table_t<long>	*ts_tab;	/* per task usage stats	*/
	long		sf;			/* stats flags		*/

    /* sched_info *curr; */
    /* ps_cfs_rq_t * rq; */

};
/************************************************************************/

struct sched_info {

	long on_rq ; 	/*  1 means this si in the cfs_rq,otherwise is 0. */
	long task;
	/*struct rb_node * n; */
	rb_node *rbnode;		/* pointer of the rbnode  */
	ps_cfs_rq_t * own_rq ; 	/* the cfs_rq owned by this group ; */
	ps_cfs_rq_t * rq; 	/*  */
	sched_info * parent;	/* a pointer to the si in upper rq  */

	/* time parameters */
	double fair;
	double key;
	double q;
	double exec_time;
	double sched_time;
	double sleep_time;
	double weight;


};
/************************************************************************/

struct  rb_node {			/* red black tree node struct		*/

	long 	color;
	rb_node *rb_left;
	rb_node *rb_right;
	rb_node *rb_parent;
	double key;
	sched_info	*si;
};

/************************************************************************/


#endif	/* _PARA_TYPES */

