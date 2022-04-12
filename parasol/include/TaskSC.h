#ifndef TASKSC_H
#define TASKSC_H

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "bumbershoot.h"
#include "parasol/para_types.h"

namespace bbs{

struct TaskThread;

struct TaskSC : public bbs_sc_module
{
   TaskSC(const std::string& name, size_t index) : bbs_sc_module(name.c_str(), index){
      // Set up here.
   };
   virtual ~TaskSC(){};

   void init(uint_fast32_t node, uint_fast32_t host, void (*code)(void*), TaskState state = BS_READY);

	TaskState state;			/* task state		*/
	size_t	node;				/* node location	*/
	size_t	host;				/* user cpu index	*/
	void	(* code)(void *);		/* task code pointer	*/
	long	upriority;			/* user task priority	*/
	long	priority;			/* task priority	*/
	long	next;				/* next task index	*/
	long	parent;				/* task parent index	*/
	long	son;				/* task son index	*/
	long	sibling;			/* task sibling index	*/
	std::vector <uint32_t>	port_list;	/* All associated ports, used to track ports for moves	*/
	uint32_t bport;			/* broadcast port index	*/
	long	blind_port;			/* blind port index	*/
	long	wport;				/* waiting port index	*/
	long	lock_list;			/* lock list 		*/
	double	rct;				/* remaining cpu time	*/
	long	qx;				/* quantum expired flag	*/
	long	did;				/* dye id		*/
	long	tsn;				/* Task serial number	*/
	double	sched_time;			/* Task schedule time	*/
 	double	end_compute_time;		/* Task end compute time */
	long	tbp;				/* break polong info	*/

	/*The following fields are used to calculate the preemption time of a task. Tao*/
	long	pt_tag;				/*preemption tag. 1: preempted, 0: not preempted.*/
	double	pt_last;			/*the latest time that is preempted*/
	double	pt_sum;				/*the total preemption time*/

	/* The followings are for cfs scheduler, which is not implemented*/
	// long 	group;				/* group index	*/
	// long 	group_id;			/* order of group */
	// sched_info	*si;		/* contain the cfs schedule information of the task */
};

struct TaskThread
{
   // Holds the information associated with a spawned thread. Very memory intensive, but it's the easiest way.
   sc_event wakeUp; // Tells the task to wake up from waiting and check current state.
   sc_event resume; // Tells the task to resume execution.
   std::string parentName; // Host name + Task name?
   TaskState state;
   size_t taskIndex; // Associated task index
   size_t hostIndex; // Associated host(CPU) index
   size_t nodeIndex; // Associated node index
   TaskThread(std::string parentName, size_t nodeIndex, size_t hostIndex, size_t taskIndex, TaskState state = BS_READY)
            : parentName(parentName), hostIndex(hostIndex), taskIndex(taskIndex), state(state), nodeIndex(nodeIndex){
      sc_spawn_options opt;
      TaskSC& task = bm_task_tab.at(taskIndex);
      if (state == BS_COMPUTING) resume.notify(SC_ZERO_TIME); // Do not immediately run unless it is actually set to run.
      // It is possible to keep the handle of the spawned thread if desired, but I don't need it for now.
      sc_spawn(sc_bind(&TaskThread::run_task, this), parentName.c_str(), &opt);
   };

   void run_task(){
      if (state == BS_READY || state == BS_SUSPENDED){
         this->resume.notify(SC_ZERO_TIME); // Just brute force this for now.
         wait(this->resume);
         state = BS_COMPUTING;
      }
      TaskSC& task = bm_task_tab.at(taskIndex);
      task.code(reinterpret_cast<void*>(this)); // Use the pointer in library functions to get parent node and ports, etc.
      state = BS_COMPLETE;
   }

};




}

#endif // TASKSC_H
