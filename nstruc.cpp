#include "jss.h"
//neighborhood strucutres

vector<Neighbor>
JIT_JSS::Swap(matriz sequence){

	vector<Neighbor>neighborhood;
	//int counter = 0;
	for(int i = 0;i<sequence.size();i++)
	{
		for(int j = 1;j<sequence[i].size();j++)
		{
			Neighbor neighbor;
			int op1 = sequence[i][j-1];
			int op2 = sequence[i][j];

			neighbor.first.first = sequence;
			neighbor.first.first[i][j-1] = op2;
			neighbor.first.first[i][j] = op1;
			neighbor.second = {op1,op2};

			neighbor.first.second = calcStartTimes(neighbor.first.first);
			if(isScheduleCorrect(neighbor.first.first,neighbor.first.second)
			&& isProcessingOrderKept(neighbor.first.first,neighbor.first.second)
			&& isStartTimesCorrect(neighbor.first.first,neighbor.first.second)
			)
			{
				neighborhood.push_back(neighbor);
			}
		}
	}

	return neighborhood;
}