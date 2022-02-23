#include <stdio.h>
#include <cstdlib>
#include <ctime>

	#include "QPBO/QPBO.h"

	int main()
	{
		srand(time(NULL));
		typedef int REAL;
		QPBO<REAL>* q;

		q = new QPBO<REAL>(2, 1); // max number of nodes & edges
		q->AddNode(2); // add two nodes

		q->AddUnaryTerm(0, 0, 2); // add term 2*x
		q->AddUnaryTerm(1, 3, 6); // add term 3*(y+1)
		q->AddPairwiseTerm(0, 1, 2, 3, 4, 6); // add term (x+1)*(y+2)

		q->MergeParallelEdges();
		q->Solve();
		for (int i=0;i<100;++i)
		{
			q->Improve();
		}
		q->ComputeWeakPersistencies();

		int x = q->GetLabel(0);
		int y = q->GetLabel(1);
		printf("Solution: x=%d, y=%d\n", x, y);

		return 0;
	}
