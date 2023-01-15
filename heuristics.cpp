//implemnentation of meta heursticis local search and tabu search

#include "jss.h"

pair<Schedule,int>
JIT_JSS::LocalSearch(matriz instance)
{
    // Schedule solution = EarliestDeadlineFirst(instance);
    Schedule solution = GifflerThompson(instance);
    int iter = 0;

    //local search will only stop affer finding the local maximum
    while(true)
    {
        iter++;
        vector<Neighbor> neighborhood = Swap(solution.first);
        bool improvement = false;

        //using best improvement strategy for now
        for(Neighbor n : neighborhood)
        {
            if(CalculatePenalties(n.first) < CalculatePenalties(solution))
            {
                solution = n.first;
                improvement = true;
            }
        }

        //reach local maximum
        if(!improvement)break;

    }

    return {solution,iter};

}

