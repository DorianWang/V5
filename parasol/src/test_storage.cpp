
#include "test_storage.h"

#include <algorithm> // std::sort


int TestStorage::add_stat(const char* name, long type)
{
   return add_stat(std::string(name), type);
}

int TestStorage::add_stat(const std::string& name, long type)
{
   ps_stat_t* nStat = new ps_stat_t;

   nStat->name = name; nStat->resid = 0.0; nStat->type = type;

   switch(type){
   case SAMPLE:
      nStat->values.sam.count = 0;
      nStat->values.sam.sum = 0.0;
      break;
   case VARIABLE:
      nStat->values.var.start = nStat->values.var.old_time = ps_now;
      nStat->values.var.old_value = 0.0;
      nStat->values.var.integral = 0.0;
      break;
   case RATE:
      nStat->values.rat.start = ps_now;
      nStat->values.rat.count = 0;
      break;
   default:
      // Panic and die?
      break;
   }
   stats.push_back(nStat);
   return stats.size() - 1;
}

std::vector <ps_stat_t*> TestStorage::sorted_stats()
{
   std::vector <ps_stat_t*> sorted = stats;
   std::sort(sorted.begin(), sorted.end(), [](ps_stat_t* lhs, ps_stat_t* rhs)
            { return lhs->name.compare(rhs->name);});
   return sorted;
}




int TestStorage::add_node(const char* name, long ncpu, double speed, double quantum, long discipline, long sf)
{
   // This is probably a lot slower that old parasol, especially from inefficient string manipulation stuff.
   // No out of memory checks, just let the std::bad_alloc exceptions go.
   ps_node_t* nNode = new ps_node_t;

   nNode->name = std::string(name);
   nNode->ncpu = ncpu; nNode->nfree = ncpu; nNode->sf = sf;
   nNode->speed = speed; nNode->rtrq = NULL_TASK; nNode->quantum = quantum;
   // NULL_COMM_PTR == ((ps_comm_t*) 0)
   nNode->sl_list = nullptr; nNode->rl_list = nullptr; nNode->bus_list = nullptr;
   nNode->ngroup = 0;


   nNode->build_time = ps_now; // Needs to get SystemC time somehow, but later.
   nNode->cpu = std::vector<ps_cpu_t>(ncpu); // vector with ncpu elements.
   if (sf & SF_PER_NODE){
      nNode->stat = add_stat(nNode->name + " Utilization", VARIABLE);
      for (size_t i = 0; i < nNode->cpu.size(); i++){
         nNode->cpu[i].stat = add_stat(nNode->name + " (cpu " + std::to_string(i) +") Utilization", VARIABLE);
      }
   }
   else {
      NOT_IMPLEMENTED_WAR;
   }

   if (discipline == CFS){
      NOT_IMPLEMENTED_WAR;
   }
   else {
      nNode->host_rq = nullptr; //NULL_CFSRQ_PTR;
   }
   nodes.push_back(nNode);
   return nodes.size() - 1;
}


void TestStorage::print_nodes()
{
   for (size_t i = 0; i < nodes.size(); i++){
      if (nodes[i] != nullptr){
         std::cout << i << " " << nodes[i]->name << " " << nodes[i]->ncpu << "\n";
      }
   }
   std::cout << "Total size of nodes vector: " << nodes.size() << std::endl;
}






