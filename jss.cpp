#include "jss.h"

int main(int argc,char **argv){

    if(argc == 0)
    {
        cout<<"NO ARGUMENTS PROVIDED\n";
        return -1;
    }
    
    JIT_JSS j;

    string file_path(argv[1]);
    string tabu_tenure(argv[2]);

    j.parseInstance(file_path);

    //j.printInstance(); 

    // Schedule s = j.EarliestDeadlineFirst(j.processingOrder); 
    Schedule s = j.GifflerThompson(j.processingOrder);

    j.PrintSchedule(s);

    vector<double>penalties = j.CalculatePenalties(s);
    cout<<"initial penalties:"<<penalties[0]<<endl;

    
    pair<Schedule,int> i = j.LocalSearch(j.processingOrder);
    s = i.first;
    int iter = i.second;

    j.PrintSchedule(s);

    penalties = j.CalculatePenalties(s);
    cout<<"initial penalties:"<<penalties[0]<<endl;
    cout<<"iter:"<<iter<<endl;

    return 0;
}