#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAX -9999
#define DistanceMAX 99999
#define alpha 1
#define betta 2
#define p 0.9
#define antPheromone 100

double countDistance(int startPoint,int endPoint,int number);
void initial(int temp[],int Temp[],int count);
void clearArray(int array[],int count);
double countOneSolutionDistance(int array[],int number);
void copy(int finalData[],int startData[],int much);
int data[51][3] = {0};

int main()
{
	clock_t startclock,endclock;
	startclock = clock();
	FILE *fptr;
	char num[2]={},x[2]={},y[2]={},empty[] = "EOF";
	int ant = 51,count = 0,chose,co = 1,i,j,h;
	double bestDistance = DistanceMAX,totalDistance = 0,tempDistance = 0;
	
//	printf("How many ant = ");
//	scanf("%d",&ant);	
	fptr = fopen("eil51-data.txt","r");
	while(fscanf(fptr,"%s %s %s",&num,&x,&y) != EOF)
	{
		if(strcmp(num,empty) != 0)
		{
			data[count][0] = atoi(num);
			data[count][1] = atoi(x);
			data[count][2] = atoi(y);
			count++;
		}
		else
			break;
	}
	fclose(fptr);
	
	double pheromone[count+1][count+1]={0};
	int bestPath[count] = {0};
	
	//set pheromone start 0.000625
	for(i=1;i<=count-1;i++)// ↓ 
		for(j=(i+1);j<=count;j++)//-->
			pheromone[i][j] = 0.000167;
				
	srand(time(NULL));
	
	while(co != 50001)
	{
		int initialAntData[ant][count]={0},antData[ant][count]={0},Temp[count] = {0},temp[count] = {0},
			pathHaveAntDirection[count+1][count+1] = {0},pathHaveAntNoDirection[count+1][count+1] = {0},q = 0,c = 0,b;
		double ant_CountDistance[ant],change[count] = {0},r,distance[count+1][count+1]={0};
		if(bestDistance == 430)
			printf("co = %d\n",co);
		//clear array		
		for(i=0;i<ant;i++)		
		{
			clearArray(antData[i],count);
			clearArray(initialAntData[i],count);			
		}		
		for(i=0;i<=count;i++)		
		{			
			clearArray(pathHaveAntDirection[i],count);
			clearArray(pathHaveAntNoDirection[i],count);
		}				
		//set pheromone table x&y
		for(i=0;i<=count;i++)					
			pheromone[0][i] = distance[0][i] = pathHaveAntNoDirection[0][i] = pathHaveAntDirection[0][i] = i;
		for(i=0;i<=count;i++)					
			pheromone[i][0] = distance[i][0] = pathHaveAntNoDirection[i][0] = pathHaveAntDirection[i][0] = i;
			
		initial(temp,Temp,count);
		//reset every solution start point
		for(i=0;i<ant;i++)
		{	
			chose = rand()%count;
			initialAntData[i][0] = Temp[chose];
		}
		for(i=0;i<ant;i++)
			antData[i][0] = initialAntData[i][0];
		//要跑的點的次數 去頭
		while(c!=(count-1))
		{		
			c++;
			int di=0,tempNumber[count-c]={0};
			double dividingLine[count-c]={0};
			for(h=0;h<ant;h++)
			{
				int t;				
				for(j=0;j<count;j++)
					for(i=0;i<c;i++)	
						if(temp[j] == antData[h][i])//去除已走過的點
							temp[j] = MAX;
			
				for(j=0;j<count;j++)
				{
					if(temp[j] != MAX)//點未走過 	
						change[j] = countDistance(antData[h][q],temp[j],count);
					else
						change[j] = DistanceMAX;
				}
				for(j=0;j<count;j++)
				{	
					if(change[j] != DistanceMAX)
					{
						int temp = antData[h][c-1],jj = j+1;						
						if(j < temp)
							change[j] = pow((pheromone[jj][temp]),alpha) * pow((1/change[j]),betta);
						else
							change[j] = pow((pheromone[temp][jj]),alpha) * pow((1/change[j]),betta);
						totalDistance +=  change[j];//計算分母
					}
				}									
				//計算每個區塊的分割線
				for(j=0;j<count;j++)
				{
					if(change[j] != DistanceMAX)
					{
						dividingLine[di] = change[j]/totalDistance + tempDistance;						
						tempDistance += change[j]/totalDistance;
						tempNumber[di] = temp[j];//紀錄是多少
						di++;
					}
				}

				r = (double)rand()/(RAND_MAX+1);			
				for(j=0;j<di;j++)
				{
					if(totalDistance == 0)
					{
						int ra;
						ra = rand()%di;
						antData[h][c] = tempNumber[ra];
						break;
					}
					if(r <= dividingLine[j])//壓線往前算
					{
						antData[h][c] = tempNumber[j];
						break;
					}
				}
				//重設 
				for(j=0;j<count;j++)//temp
					temp[j] = data[j][0];
				for(j=0;j<di;j++)//tempNumber
					tempNumber[j] = 0;
				totalDistance = 0;
				tempDistance = 0;
				di = 0;				
			}
			q++;
		}
		//計算每路上有幾隻螞蟻 (有方向) 
		for(h=0;h<ant;h++)
		{
			for(i=0;i<(count-1);i++)
			{				
				int j = i + 1;
				pathHaveAntDirection[antData[h][i]][antData[h][j]]++;				
			}
		}		
		//無方向 合併 
		for(i=1;i<=count-1;i++)// ↓ 
			for(j=(i+1);j<=count;j++)//-->
				pathHaveAntNoDirection[i][j] = pathHaveAntDirection[i][j] + pathHaveAntDirection[j][i];		
		//update pheromone 
		for(i=1;i<=count-1;i++)// ↓ 
		{
			double total;
			total = countOneSolutionDistance(antData[i-1],count);
			for(j=(i+1);j<=count;j++)//-->
			{
//				distance[i][j] = countDistance(pathHaveAntNoDirection[i][0],pathHaveAntNoDirection[0][j],count);
				pheromone[i][j] = (antPheromone/total) * pathHaveAntNoDirection[i][j] + (1-p) * pheromone[i][j];
			}
		}		
		for(i=0;i<ant;i++)		
		{
			ant_CountDistance[i] = countOneSolutionDistance(antData[i],count);
			if(ant_CountDistance[i] < bestDistance)
			{
				copy(bestPath,antData[i],count);
				bestDistance = ant_CountDistance[i];
			}
		}
		co++;			
		printf("%f\n",bestDistance);
	}
	for(i=0;i<count;i++)
		printf("%2d %2d %2d\n",bestPath[i],data[bestPath[i] - 1][1],data[bestPath[i] - 1][2]);
	printf("%2d %2d %2d\n",bestPath[0],data[bestPath[0]-1][1],data[bestPath[0]-1][2]);				
	endclock = clock();		
//	printf("%1f\n",(endclock-startclock)/(double)(CLOCKS_PER_SEC));//total time	
}

void clearArray(int array[],int count)
{
	int j;
	for(j=0;j<=count;j++)
		array[j] = 0;	
}
 
void initial(int temp[],int Temp[],int count)
{
	int j;
	for(j=0;j<count;j++)
		temp[j] = Temp[j] = data[j][0];	
}

double countDistance(int startPoint,int endPoint,int number)
{	
	int i,j,countDistance_tempdata_startPoint[2] = {0},countDistance_tempdata_endPoint[2] = {0};
	double toCountDistance = 0;
	for(i=0;i<number;i++)//取xy軸 
	{
		if(startPoint == data[i][0])
		{
			countDistance_tempdata_startPoint[0] = data[i][1];
			countDistance_tempdata_startPoint[1] = data[i][2];
		}
		if(endPoint == data[i][0])
		{
			countDistance_tempdata_endPoint[0] = data[i][1];
			countDistance_tempdata_endPoint[1] = data[i][2];
		}
	}
	toCountDistance = sqrt(pow((countDistance_tempdata_startPoint[0] - countDistance_tempdata_endPoint[0]),2) + pow((countDistance_tempdata_startPoint[1] - countDistance_tempdata_endPoint[1]),2));
	return toCountDistance;
}

double countOneSolutionDistance(int array[],int number)
{	
	int i,j,countDistance_tempdata[number][2]={0};
	double toCountDistance = 0;	
	for(i=0;i<number;i++)
	{
		for(j=0;j<number;j++)
		{
			if(array[i] == data[j][0])
			{
				countDistance_tempdata[i][0] = data[j][1];//x
				countDistance_tempdata[i][1] = data[j][2];//y
			}
		}
	}
	for(j=0;j<number;j++)	
	{
		int temp;
		if(j!=(number-1))
		{
			temp = j + 1;		
			toCountDistance += sqrt(pow((countDistance_tempdata[j][0]-countDistance_tempdata[temp][0]),2)+pow((countDistance_tempdata[j][1]-countDistance_tempdata[temp][1]),2));
		}
		else
		{
			temp = 0;
			toCountDistance += sqrt(pow((countDistance_tempdata[j][0]-countDistance_tempdata[temp][0]),2)+pow((countDistance_tempdata[j][1]-countDistance_tempdata[temp][1]),2));
		}
	}
	return toCountDistance;
}

void copy(int finalData[],int startData[],int much)
{
	int ci,cj;
	for(ci=0;ci<much;ci++)
	{			
		finalData[ci] = startData[ci];
	}
}
