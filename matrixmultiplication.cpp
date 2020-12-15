#include<iostream>
#include<mpi.h>
#include<fstream>

using namespace std;

int main(int argc,char *args[])
{
	int index=0;
	int row1=500,column1=500,row2=500,column2=500;                    //Rows and Colums of matrices
	int arr1[row1][column1],arr2[row2][column2],arr3[row1][column2];
    
    
    int row_to_compute=1;          //ending index
	int number_of_row=0;        //Number of rows each process will get
	int pid,np;            //pid stores the rank whereas np stores number of process created



	MPI_Init(&argc,&args);             //Initializing MPI                              

	MPI_Comm_rank(MPI_COMM_WORLD,&pid);           //fetching rank      
	MPI_Comm_size(MPI_COMM_WORLD,&np);            //fetching size

	
	if(pid==0)
	{
        
        for(int i=0;i<row1;i++)
        for(int j=0;j<column1;j++)
            arr1[i][j]=1;               
                                             //initializing both array's with 1 inorder to keep things simple   
        for(int i=0;i<row2;i++)
        for(int j=0;j<column2;j++)
            arr2[i][j]=1;  
        
        
        double start_time=MPI_Wtime();    //Fetching the elapsed time/wall time (Beginning)

        number_of_row=row1/np; // Dividing rows of matrix A
		int i=1;
	if(np>1)
	{
			for(i=1;i<np-1;i++)  
		{
			index=i*number_of_row;
			row_to_compute=index+number_of_row-1;
			MPI_Send(&index,1,MPI_INT,i,0,MPI_COMM_WORLD);        //Send's the starting index of matrix A which is to be computed

			MPI_Send(&row_to_compute,1,MPI_INT,i,0,MPI_COMM_WORLD);  //Send's the ending index of matrix A 
			MPI_Send(&arr1, row1*column1, MPI_INT, i, 0, MPI_COMM_WORLD);     

			MPI_Send(&arr2,row2*column2,MPI_INT,i,0,MPI_COMM_WORLD);
            
            //Note that both matrices are sent fully but the iteration of outer for loop will be determined by 'index' and 'row_to_compute' variables
            //hence dividing the rows of matrix A in each processes. 		 
		}


		index=i*number_of_row;  //Remaining number of rows that are left will be computed by last process.
		row_to_compute=row1-1;
        
            

			MPI_Send(&index,1,MPI_INT,i,0,MPI_COMM_WORLD);

			MPI_Send(&row_to_compute,1,MPI_INT,i,0,MPI_COMM_WORLD);
			MPI_Send(&arr1, row1*column1, MPI_INT, i, 0, MPI_COMM_WORLD);

			MPI_Send(&arr2,row2*column2,MPI_INT,i,0,MPI_COMM_WORLD);
	}
		
        for(int i=0;i<number_of_row;i++)           //Master process does its work.
		{
          for(int j=0;j<column2;j++)
		{   arr3[i][j]=0;
        	for(int k=0;k<column1;k++)                 
				{	
                    arr3[i][j]+=arr1[i][k]*arr2[k][j];
                }
        }
        }    

		for(int i=1;i<np;i++)
		{
			int start=0,end=0;
			MPI_Recv(&start,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&end,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);              //Getting the base address of row that is being computed by any particular process.
			MPI_Recv(&arr3[start][0],((end-start)+1)*column2,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
		}

        double finish_time=MPI_Wtime();  //Fetching Finish Time

        ofstream output;
        output.open("output.txt",ios::out);

		for(int i=0;i<row1;i++){
			for(int j=0;j<column2;j++)                 
				{
					output<<arr3[i][j]<<" ";    //Writing Results in output.txt file.
				}
				output<<endl;
			}	
        output.close();    
        cout<<"Done Computing Check text file.."<<endl;
        cout<<"Time taken: "<<finish_time-start_time<<endl;

	}
	else
	{
		int start=0,end=0;
        MPI_Recv(&start,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);  //Starting index
		MPI_Recv(&end,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);                       //Ending Index.   
		MPI_Recv(&arr1,row1*column1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);      
		MPI_Recv(&arr2,row2*column2,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		for(int i=start;i<=end;i++)
		{	for(int j=0;j<column2;j++)
			{	
                arr3[i][j]=0;                               //Computation
                for(int k=0;k<column1;k++)                 
				{	
                    arr3[i][j]+=arr1[i][k]*arr2[k][j];
                }
            }
        }

		MPI_Send(&start,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Send(&end,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Send(&arr3[start][0],((end-start)+1)*column2,MPI_INT,0,0,MPI_COMM_WORLD); //Sending Base address of starting row that was computed
	}

	
	MPI_Finalize();  
	return 0;
}