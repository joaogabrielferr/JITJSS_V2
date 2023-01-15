#include "jss.h"


void
JIT_JSS::PrintSchedule(Schedule s)
{
    matriz sequence = s.first;
    vector<int> time = s.second;
	for(int i = 0;i<sequence.size();i++)
	{
		cout<<i<<":";
		for(int j = 0;j<sequence[i].size();j++)
		{
            //adding 1 to job so help visualization in terminal
            //printing Ojob,machine(start time, completion time)
			cout<<"O"<<job[sequence[i][j]]+1<<machine[sequence[i][j]]<<"( "<< time[sequence[i][j]] <<","<<time[sequence[i][j]] + processingTime[sequence[i][j]] << ") ";
		}
		cout<<endl;
	}
	cout<<endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//calculate the penalties for a given schedule
//return: {total penalties (earlines + tardiness), just earliness, just tardiness}

vector<double>
JIT_JSS::CalculatePenalties(Schedule s)
{
	matriz sequence = s.first;
    vector<int> time = s.second;
	double total = 0.0, totalearliness = 0.0,totaltardiness = 0;

	for(int i = 0;i<sequence.size();i++)
	{
		for(int j = 0;j<sequence[i].size();j++)
		{
			int opcompletionTime = time[sequence[i][j]] + processingTime[sequence[i][j]];
			double ea = (opcompletionTime<= dueDate[sequence[i][j]] ? (dueDate[sequence[i][j]] - opcompletionTime)*earliness[sequence[i][j]]  :  0);
			double ta = (opcompletionTime>= dueDate[sequence[i][j]] ? (opcompletionTime - dueDate[sequence[i][j]])*tardiness[sequence[i][j]]  :  0);
			totalearliness+=ea;
			totaltardiness+=ta;
			total += (ea + ta);
		}
	}

	return {total,totalearliness,totaltardiness};

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//calculate start times from a given sequence of operations in the machines

vector<int>
JIT_JSS::calcStartTimes(matriz sequence)
{
	vector<int>startTime;
	startTime.resize(nOperations + 1);

	//ready has operations that are ready to be processed next
	vector<int>ready;

	//machineStart[i] = minimum start time for machine i
	//jobStart[i] = minimumn start time for job i
	//start time of op 'x' will be maximum between machinestart[machine[x]] and jobstart[job[x]]
	vector<int>machineStart(nMachines,0),jobStart(nJobs,0);

	//putting first operation of each machine in the ready vector
	for(int i = 0;i<sequence.size();i++)
	{
		ready.push_back(sequence[i][0]);
	}

	//while ready is not empty, select the op with smallest due date from the vector and remove it
	//sequence this op
	//add this op's sucessor in the job
	while(!ready.empty())
	{
		int selected = ready[0];
		int selectedId = 0;
		for(int i = 1;i<ready.size();i++)
		{
			if(dueDate[ready[i]] < dueDate[selected])
			{
				selected = ready[i];
				selectedId = i;
			}
		}

		ready[selectedId] = ready[ready.size() - 1];
		ready.pop_back();

		//sequence[machine[selected]].push_back(selected);

		startTime[selected] = max(machineStart[machine[selected]],jobStart[job[selected]]);

		machineStart[machine[selected]] = startTime[selected] + processingTime[selected];
		jobStart[job[selected]] = startTime[selected] + processingTime[selected];



		int selectedJob = job[selected];
		for(int i = 0;i<sequence[machine[selected]].size() - 1;i++)
		{
			if(selected == sequence[machine[selected]][i])
			{
				int next = sequence[machine[selected]][i + 1];
				ready.push_back(next);
			}
		}

	}

	return startTime;
}


/////////////////////////////////////////////////////////////////////////////////////////

bool checkIntersection(int start1,int end1,int start2,int end2)
{
	if (start1 == start2) {
		return true;
	}
	else if (start1 < start2) {
		if (end1 > start2) {
			return true;
		}
	}
	else {//start2 < start1
		if (end2 > start1) {
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool
JIT_JSS::isScheduleCorrect(matriz s,vector<int> neighborStartTime)
{
	for(int op1 = 1;op1<= nOperations;op1++)
	{
		for(int op2 = 1;op2<=nOperations;op2++)
		{
			//|| machine[op1] == machine[op2]
			if(op1 != op2 && (job[op1] == job[op2]))
			{
				if(checkIntersection(neighborStartTime[op1],neighborStartTime[op1] + processingTime[op1],neighborStartTime[op2],neighborStartTime[op2] + processingTime[op2]))
				{
					return false;
				}
			}
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool
JIT_JSS::isStartTimesCorrect(matriz schedule,vector<int>scheduleStartTime)
{
	for(int i = 0;i<schedule.size();i++)
	{
		for(int j = 1;j<schedule[i].size();j++)
		{
			int op1starttime = scheduleStartTime[schedule[i][j - 1]];
			int op1proctime = processingTime[schedule[i][j - 1]];
			int op2starttime = scheduleStartTime[schedule[i][j]];
			if(op2starttime < op1starttime + op1proctime)return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool
JIT_JSS::isProcessingOrderKept(matriz schedule,vector<int>scheduleStartTime)
{
	matriz scheduleOrder(nJobs);
	for(int i = 0;i<schedule.size();i++)
	{
		for(int j = 0;j<schedule[i].size();j++)
		{
			scheduleOrder[job[schedule[i][j]]].push_back(schedule[i][j]);
		}
	}

	for(int i = 0;i<scheduleOrder.size();i++)
	{
		sort(scheduleOrder[i].begin(),scheduleOrder[i].end(),[&scheduleStartTime](int &op1, int &op2){
			return scheduleStartTime[op1] < scheduleStartTime[op2];
		});
	}


	for(int i = 0;i<scheduleOrder.size();i++)
	{
		for(int j = 0;j<scheduleOrder[i].size();j++)
		{
			if(scheduleOrder[i][j] != processingOrder[i][j])return false;
		}
	}

	return true;

}