#include "jss.h"


//initial solution generators

/////////////////////////////////////////////////////////////////////////////////////////

//EARLIEST DEAD LINE FIRST:  SCHEDULING OPERATIONS AS EARLY AS POSSIBLE, ALWAYS PICKING TO SCHEDULE THE ONE WITH SMALLEST DEADLINE

//receive processingOrder as param
Schedule
JIT_JSS::EarliestDeadlineFirst(matriz instance)
{
	matriz sequence;
	vector<int>startTime;

	sequence.resize(nMachines);
	startTime.resize(nOperations + 1);

	//ready has operations that are ready to be processed next
	vector<int>ready;

	//machineStart[i] = minimum start time for machine i
	//jobStart[i] = minimumn start time for job i
	//start time of op 'x' will be maximum between machinestart[machine[x]] and jobstart[job[x]]
	vector<int>machineStart(nMachines,0),jobStart(nJobs,0);

	//putting first operation of each job in the ready vector
	for(int i = 0;i<nJobs;i++)
	{
		ready.push_back(instance[i][0]);
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

		sequence[machine[selected]].push_back(selected);

		startTime[selected] = max(machineStart[machine[selected]],jobStart[job[selected]]);

		machineStart[machine[selected]] = startTime[selected] + processingTime[selected];
		jobStart[job[selected]] = startTime[selected] + processingTime[selected];



		int selectedJob = job[selected];
		for(int i = 0;i<jobOps[selectedJob].size() - 1;i++)
		{
			if(selected == jobOps[selectedJob][i])
			{
				int next = jobOps[selectedJob][i + 1];
				ready.push_back(next);
			}
		}

	}

	return {sequence,startTime};

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GIFFLER THOMPSON:
//IMPLEMENTING FROM A meta-heuristic to solve the just-in-time job-shop scheduling problem BY Mohammad Mahdi Ahmadian, Amir Salehipour, T.C.E. Cheng

Schedule
JIT_JSS::GifflerThompson(matriz sequence)
{
	Schedule s;
	s.first.resize(nMachines);
	//input: the processing order
	//output: a feasible sequence s (the start times will be calculated with 'CalcStartTimes' in this same function, so it will actually output a feasible schedule)

	vector<int>G; //all the schedulable operations
	vector<int> machineStartTime(nMachines,0),jobStartTime(nJobs,0);

	//putting first operation of each job in G
	for(int i = 0;i<sequence.size();i++)
	{
		G.push_back(sequence[i][0]);
	}

	while(!G.empty())
	{
		//cout<<"new iter:\n";
		//Find the earliest completion time C∗ and its associated machine M∗ among all the operations in G
		int C = INT_MAX;
		int M = 0;

		for(int i = 0;i<G.size();i++)
		{
			//C = min(C, max( machineStartTime[machine[G[i]]], jobStartTime[job[G[i]]] ) + processingTime[G[i]] );
			int completionTime = max( machineStartTime[machine[G[i]]], jobStartTime[job[G[i]]] ) + processingTime[G[i]];
			if(completionTime < C)
			{
				C = completionTime;
				M = machine[G[i]];
			}   
		}
		//cout<<"C*:"<<C<<",M*"<<M<<endl;

		//; let G0 ⊆ G denote the operations processed on machine M∗;
		vector<int>G0;

		for(int i = 0;i<G.size();i++)
		{
			if(machine[G[i]] == M)
			{
				G0.push_back(G[i]);
			}
		}

		//building the conflict set (all the operation in G0 such that the start time is less than C)
		vector<int>G1;

		for(int i = 0;i<G0.size();i++)
		{
			int startTime = max( machineStartTime[machine[G[i]]], jobStartTime[job[G[i]]] );
			if(startTime < C)
			{
				G1.push_back(G0[i]);
			}
		}
		
		//select op in G1 to be scheduled next using Earliest due date dispatching rule
		int op = G1[0];
		for(int i = 0;i<G1.size();i++)
		{
			if(dueDate[G1[i]] < dueDate[op])
			{
				op = G1[i];
			}
		}
		//cout<<"op:"<<op<<endl;

		//schedule op
		s.first[machine[op]].push_back(op);

		//remote op from G
		for(int i = 0;i<G.size();i++)
		{
			if(G[i] == op)
			{
				G[i] = G[G.size() - 1];
				break;
			}
		}
		G.pop_back();

		//add sucessor of op to G (if any)
		for(int i = 0;i<sequence.size();i++)
		{
			for(int j = 0;j<sequence[i].size() - 1;j++)
			{
				if(sequence[i][j] == op)
				{
					int next = sequence[i][j + 1];
					G.push_back(next);
					break;
				}
			}
		}

		//update earliest start times in machine and job
		int earliestStart =  max( machineStartTime[machine[op]], jobStartTime[job[op]] );
		machineStartTime[machine[op]] = earliestStart + processingTime[op];
		jobStartTime[job[op]] = earliestStart + processingTime[op];
	}

	
	//after fiding a feasible sequence with giffler thompson, find start times
	vector<int> times = calcStartTimes(s.first);
	s.second = times;

	return s;

}