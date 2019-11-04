/*



 PUT HEADER HERE



 */


#include "mpi.h"
#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <time.h>

using namespace std;
int ITERATIONS;
double F_MAX;
double mass = 10000;
double DATA[8][7];
#define MASTER 0
struct buzzyFleet

{

	int rankID, status;
	double x, y, z, Fx, Fy, Fz, v, dirX, dirY, dirZ;
};
struct buzzyFleet buzzy, yellowJacket1, yellowJacket2, yellowJacket3, yellowJacket4, yellowJacket5, yellowJacket6, yellowJacket7;

/*struct buzzyFleet getOrigin(double t)

{
}*/

//void moveBuzzy(buzzyFleet);

//void reportToBuzzy(buzzyFleet);

void processDAT(string fileName)

{
	buzzy.rankID = 0;
	yellowJacket1.rankID = 1;
	yellowJacket2.rankID = 2;
	yellowJacket3.rankID = 3;
	yellowJacket4.rankID = 4;
	yellowJacket5.rankID = 5;
	yellowJacket6.rankID = 6;
	yellowJacket7.rankID = 7;
	yellowJacket1.status = 1;
	yellowJacket2.status = 1;
	yellowJacket3.status = 1;
	yellowJacket4.status = 1;
	yellowJacket5.status = 1;
	yellowJacket6.status = 1;
	yellowJacket7.status = 1;

	ifstream file;

	file.open(fileName);
	file >> ITERATIONS;

	file >> F_MAX;

	//cout << "iterations are "<<ITERATIONS << "and fmax is " << F_MAX;

	for (int i = 0; i < 9; i++)

	{
		for (int j = 0; j < 7; j++)

		{

			file >> DATA[i][j];
		}
	}

	buzzy.x = DATA[0][0];
	buzzy.y = DATA[0][1];
	buzzy.z = DATA[0][2];
	buzzy.v = DATA[0][3];
	buzzy.dirX = DATA[0][4];
	buzzy.dirY = DATA[0][5];
	buzzy.dirZ = DATA[0][6];

	yellowJacket1.x = DATA[1][0];
	yellowJacket1.y = DATA[1][1];
	yellowJacket1.z = DATA[1][2];
	yellowJacket1.v = DATA[1][3];
	yellowJacket1.dirX = DATA[1][4];
	yellowJacket1.dirY = DATA[1][5];
	yellowJacket1.dirZ = DATA[1][6];

	yellowJacket2.x = DATA[2][0];
	yellowJacket2.y = DATA[2][1];
	yellowJacket2.z = DATA[2][2];
	yellowJacket2.v = DATA[2][3];
	yellowJacket2.dirX = DATA[2][4];
	yellowJacket2.dirY = DATA[2][5];
	yellowJacket2.dirZ = DATA[2][6];

	yellowJacket3.x = DATA[3][0];
	yellowJacket3.y = DATA[3][1];
	yellowJacket3.z = DATA[3][2];
	yellowJacket3.v = DATA[3][3];
	yellowJacket3.dirX = DATA[3][4];
	yellowJacket3.dirY = DATA[3][5];
	yellowJacket3.dirZ = DATA[3][6];

	yellowJacket4.x = DATA[4][0];
	yellowJacket4.y = DATA[4][1];
	yellowJacket4.z = DATA[4][2];
	yellowJacket4.v = DATA[4][3];
	yellowJacket4.dirX = DATA[4][4];
	yellowJacket4.dirY = DATA[4][5];
	yellowJacket4.dirZ = DATA[4][6];

	yellowJacket5.x = DATA[5][0];
	yellowJacket5.y = DATA[5][1];
	yellowJacket5.z = DATA[5][2];
	yellowJacket5.v = DATA[5][3];
	yellowJacket5.dirX = DATA[5][4];
	yellowJacket5.dirY = DATA[5][5];
	yellowJacket5.dirZ = DATA[5][6];

	yellowJacket6.x = DATA[6][0];
	yellowJacket6.y = DATA[6][1];
	yellowJacket6.z = DATA[6][2];
	yellowJacket6.v = DATA[6][3];
	yellowJacket6.dirX = DATA[6][4];
	yellowJacket6.dirY = DATA[6][5];
	yellowJacket6.dirZ = DATA[6][6];

	yellowJacket7.x = DATA[7][0];
	yellowJacket7.y = DATA[7][1];
	yellowJacket7.z = DATA[7][2];
	yellowJacket7.v = DATA[7][3];
	yellowJacket7.dirX = DATA[7][4];
	yellowJacket7.dirY = DATA[7][5];
	yellowJacket7.dirZ = DATA[7][6];


	//cout << "data 7 0 is: "<<DATA[7][0];



	//return data;

}

double fRand(double min, double max)

{

	rand();
	return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
	//return a_random_double;
}
double getAcceleration(double F, double m)

{
	double tol = fRand(0.8, 1.2);

	return tol * F / m;

}

double getLocation(double t, double initLocation, double initSpeed, double a)

{
	return (initLocation + (initSpeed * t) + 0.5 * a * t * t);

}

double getSpeed(double t, double initSpeed, double a)

{
	return ((initSpeed * t) + a * t);

}

void moveBuzzy(double t)

{

	buzzy.rankID = 0;
	buzzy.status = 1;
	buzzy.v = buzzy.v;
	buzzy.dirX = buzzy.dirX;
	buzzy.dirY = buzzy.dirY;
	buzzy.dirZ = buzzy.dirZ;
	buzzy.Fx = 0;
	buzzy.Fy = 0;
	buzzy.Fz = 0;
	buzzy.x = getLocation(t, buzzy.x, buzzy.v, 0);
	buzzy.y = getLocation(t, buzzy.y, buzzy.v, 0);
	buzzy.z = getLocation(t, buzzy.z, buzzy.v, 0);
}

void moveJacketInParallel(double t, struct buzzyFleet *jacket)

{
	jacket ->rankID = 0;

	if (jacket ->status == 1) // applying acceleartion of 0

	{

		jacket->x = jacket->x + (jacket->x * t);
		jacket->y = jacket->y + (jacket->y * t);
		jacket->z = jacket->z + (jacket->z * t);
	}



}
double euclideanDistance(struct buzzyFleet A, struct buzzyFleet B)

{

	return pow((pow(A.x - B.x, 2) + pow(A.y - B.y, 2) + pow(A.z - B.z, 2)), 0.5);

}

void getToOrigin(struct buzzyFleet* A, double t)

{
	//cout << A->x << endl;
	if (A->x != 0)//&& A.y !=0)
	{
		double nextX = A->x + (A->v * t) + (0.5 * (-F_MAX / mass) * t * t);
		double nextV = A->v + (-F_MAX / mass) * t;
		double nextX2 = A->x + A->v * t;
		if (nextX > 0)

		{
			A->x = nextX;
			A->v = nextV;
		}
		else if (nextX < 0 && nextX2 != 0)
		{
			A->Fx = 0;
			A->x = nextX2;
		}
		else
		{
			A->x = 0;
		}
	}
}
 //bool isFlyingInSameDirection(double buzzy_x, double buzzy_y, double buzzy_z, double, yj_x, double yj_y, double yz)

#define MSG_SIZE 1000

char buf[MSG_SIZE];  // Message contents

/*double* msg(struct buzzyFleet* A)
{
	double*send = new double[8];
	

	send[0] = A->rankID;
	send[1] = A->status;
	send[2] = A->x;
	send[3] = A->y;
	send[4] = A->z;
	send[5] = A->Fx;
	send[6] = A->Fy;
	send[7] = A->Fz;

	return send;

}*/

double* getFleetInfo()
//struct buzzyFleet* A, struct buzzyFleet* B, struct buzzyFleet* C, struct buzzyFleet* D, struct buzzyFleet* E, struct buzzyFleet* F, struct buzzyFleet* G)
{
	double* send = new double[64];


	send[0] = buzzy.rankID;
	send[1] = yellowJacket1.rankID;
	send[2] = buzzy.x;
	send[3] = buzzy.y;
	send[4] = buzzy.z;
	send[5] = buzzy.Fx;
	send[6] = buzzy.Fy;
	send[7] = buzzy.Fz;

	send[8] = yellowJacket1.rankID;
	send[9] = yellowJacket1.status;
	send[10] = yellowJacket1.x;
	send[11] = yellowJacket1.y;
	send[12] = yellowJacket1.z;
	send[13] = yellowJacket1.Fx;
	send[14] = yellowJacket1.Fy;
	send[15] = yellowJacket1.Fz;

	send[16] = yellowJacket2.rankID;
	send[17] = yellowJacket2.status;
	send[18] = yellowJacket2.x;
	send[19] = yellowJacket2.y;
	send[20] = yellowJacket2.z;
	send[21] = yellowJacket2.Fx;
	send[22] = yellowJacket2.Fy;
	send[23] = yellowJacket2.Fz;

	send[24] = yellowJacket3.rankID;
	send[25] = yellowJacket3.status;
	send[26] = yellowJacket3.x;
	send[27] = yellowJacket3.y;
	send[28] = yellowJacket3.z;
	send[29] = yellowJacket3.Fx;
	send[30] = yellowJacket3.Fy;
	send[31] = yellowJacket3.Fz;

	send[32] = yellowJacket4.rankID;
	send[33] = yellowJacket4.status;
	send[34] = yellowJacket4.x;
	send[35] = yellowJacket4.y;
	send[36] = yellowJacket4.z;
	send[37] = yellowJacket4.Fx;
	send[38] = yellowJacket4.Fy;
	send[39] = yellowJacket4.Fz;

	send[40] = yellowJacket5.rankID;
	send[41] = yellowJacket5.status;
	send[42] = yellowJacket5.x;
	send[43] = yellowJacket5.y;
	send[44] = yellowJacket5.z;
	send[45] = yellowJacket5.Fx;
	send[46] = yellowJacket5.Fy;
	send[47] = yellowJacket5.Fz;

	send[48] = yellowJacket6.rankID;
	send[49] = yellowJacket6.status;
	send[50] = yellowJacket6.x;
	send[51] = yellowJacket6.y;
	send[52] = yellowJacket6.z;
	send[53] = yellowJacket6.Fx;
	send[54] = yellowJacket6.Fy;
	send[55] = yellowJacket6.Fz;

	send[56] = yellowJacket7.rankID;
	send[57] = yellowJacket7.status;
	send[58] = yellowJacket7.x;
	send[59] = yellowJacket7.y;
	send[60] = yellowJacket7.z;
	send[61] = yellowJacket7.Fx;
	send[62] = yellowJacket7.Fy;
	send[63] = yellowJacket7.Fz;

	return send;

}

void coutArray(double *arr, int size)
{

	for (int i = 0; i < size; size++)
	{
		if (i == size - 1)
		{
			cout <<arr[i] << endl;
		}
		
		else
		{
			cout << arr[i] << ", ";
		}
	}

}

int main(int argc, char** argv)
{
	
	srand(time(NULL));
	processDAT("in.dat");
	//ofstream outFile;
	//outFile.open("out.dat");
	int  numtasks, rank, rc;

	rc = MPI_Init(&argc, &argv); //

	struct buzzyFleet recv;
	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}


	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Seed the random number generator to get different results each time
	srand(rank);
	if (rank == 0)
	{
		processDAT("in.dat");
	}
	//int* pArray = new int[sz];
	double* sendArray = new double[8*8]; // = { buzzy.x, buzzy.y, buzzy.z, buzzy.Fx, buzzy.Fy, buzzy.Fz };
	double* receiveArray = new double[8*8*8];
	//sendArray = msg(&buzzy);
	//sendArray[0] = buzzy.z;// { buzzy.x, buzzy.y, buzzy.z, buzzy.Fx, buzzy.Fy, buzzy.Fz };
	// Broadcast to yellowjackets
	sendArray = getFleetInfo();// &yellowJacket1, & yellowJacket2, & yellowJacket3, & yellowJacket4,
		//&yellowJacket5, &yellowJacket6, &yellowJacket7);
	MPI_Bcast(sendArray, 64, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	//cout << sendArray[0] << sendArray[1] << sendArray[2] << sendArray[3] << sendArray[4] << sendArray[5] <<endl;
	// Loop through the number of time steps
	for (int round = 0; round < ITERATIONS; round++)
	{
		
		if (rank == 0)
		{
			// Calculate Buzzy new location
			moveBuzzy(round);
			// Gather info from  fleet by sending fleet info
			MPI_Allgather(sendArray, 64, MPI_DOUBLE, receiveArray, 64, MPI_DOUBLE, MPI_COMM_WORLD);
			// Updating the fleet information
			sendArray = getFleetInfo();

			// printing the receive array


			cout << receiveArray[0] << ", " << receiveArray[1] << ", " << receiveArray[2] <<
				", " << receiveArray[3] << ", " << receiveArray[4] << ", " << receiveArray[5] <<
				", " << receiveArray[6] << ", " << receiveArray[7] << "\n"
				<< receiveArray[8] << ", " << receiveArray[9] << ", " << receiveArray[10] << ", " <<
				receiveArray[11] << ", " << receiveArray[12] << ", " << receiveArray[13] << ", " << receiveArray[14]
				<< ", " << receiveArray[15] << "\n" << receiveArray[16] << ", " << receiveArray[17] << ", " << receiveArray[18] <<
				", " << receiveArray[19] << ", " << receiveArray[20] << ", " << receiveArray[21] <<
				", " << receiveArray[22] << ", " << receiveArray[23] << "\n" << receiveArray[24] << ", " << receiveArray[25] << ", " << receiveArray[26] <<
				", " << receiveArray[27] << ", " << receiveArray[28] << ", " << receiveArray[29] <<
				", " << receiveArray[30] << ", " << receiveArray[31] << "\n" << receiveArray[32] << ", " << receiveArray[33] << ", " << receiveArray[34] <<
				", " << receiveArray[35] << ", " << receiveArray[36] << ", " << receiveArray[37] <<
				", " << receiveArray[38] << ", " << receiveArray[39] << "\n" << receiveArray[40] << ", " << receiveArray[41] << ", " << receiveArray[42] <<
				", " << receiveArray[43] << ", " << receiveArray[44] << ", " << receiveArray[45] <<
				", " << receiveArray[46] << ", " << receiveArray[47] << "\n" << receiveArray[48] << ", " << receiveArray[49] << ", " << receiveArray[50] <<
				", " << receiveArray[51] << ", " << receiveArray[52] << ", " << receiveArray[53] <<
				", " << receiveArray[54] << ", " << receiveArray[55] << "\n" << receiveArray[56] << ", " << receiveArray[57] << ", " << receiveArray[58] <<
				", " << receiveArray[59] << ", " << receiveArray[60] << ", " << receiveArray[61] <<
				", " << receiveArray[62] << ", " << receiveArray[63] << "\n";
			
		}
		else
		{
			// Calculate yellow jacket new location
			//CalculateYellowJacketXYZ();
			//moveJacketInParallel(round, &yellowJacket1);
			MPI_Allgather(sendArray, 64, MPI_DOUBLE,receiveArray, 64, MPI_DOUBLE,MPI_COMM_WORLD);
			
		}
	}
	MPI_Finalize();
}

