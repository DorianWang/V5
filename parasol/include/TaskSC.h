#ifndef TASKSC_H
#define TASKSC_H

#include "systemc.h"
#include "para_types.h"

struct TaskSC : public sc_module
{
   /** Default constructor */
   TaskSC(sc_module_name name) : sc_module(name){
      // Set up here.
   };
   /** Default destructor */
   virtual ~TaskSC();
   std::string name;
	ps_task_state_t state;			/* task state		*/
	long	node;				/* node location	*/
	long	uhost;				/* user cpu index	*/
	long	host;				/* cpu index		*/
	ps_cpu_t	*hp;		/* host pointer		*/
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

#endif // TASKSC_H
