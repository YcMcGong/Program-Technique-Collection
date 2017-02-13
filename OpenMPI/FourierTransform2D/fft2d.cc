// Distributed two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;
void Transform1D(Complex* h, int w, Complex* H);
void InvTransform1D(Complex* h, int N, Complex* H);

void MatrixRotate(Complex* Out, Complex* In, int hig, int wid)
{
    for(int h=0; h<hig; h++)
    {
        for(int w=0; w<wid; w++)
        {
            Out[h*wid+w] = In[wid*w+h];
        }
    }  
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
  InputImage image(inputFN);  // Create the helper object for reading the image
  Complex* data = image.GetImageData();
  // Step (1) in the comments is the line above.
  // Your code here, steps 2-9
  int numtasks, rank;
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  printf ("Number of tasks= %d My rank= %d\n", numtasks,rank);
  
  int w = image.GetWidth();
  int Hig = image.GetHeight();
  Complex *Final = new Complex[Hig*w];

int total_count = Hig/numtasks;

for(int count=0; count<total_count; count++)
{
  Complex *h = new Complex[w];
  Complex *H = new Complex[w];
  h = data+rank*w+w*count*numtasks;
  Transform1D(h, w, H);
  int rc;
  
  if(rank==0)
  {
      for(int i=0; i<w; i++)
      {
      Final[i+w*count*numtasks]=H[i];
      }
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  //Send and Recive
  for(int r=1; r<numtasks; r++)
  {
       if(rank==r) rc = MPI_Send(H, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
       MPI_Status status;
       if(rank==0) rc = MPI_Recv(Final+w*r+w*count*numtasks, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD, &status);
  }
  MPI_Barrier(MPI_COMM_WORLD);
}

Complex* Image2 = new Complex[Hig*w];
if(rank==0)
{
    MatrixRotate(Image2, Final, Hig, w);
}




//2nd fft2-------------------------------------------------------------------------------------------------
total_count = Hig/numtasks;
Final = new Complex[Hig*w];
 
for(int count=0; count<total_count; count++)
{
  int rc;
  Complex *h = new Complex[w];
  Complex *H = new Complex[w];
  Complex *Send = new Complex[w];
  
  if(rank==0) h = Image2+w*count*numtasks;
  
  for(int r=1; r<numtasks; r++)
  {
       if(rank==0) 
       {
           Send = Image2+r*w+w*count*numtasks;
           rc = MPI_Send(Send, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD);
       }
       MPI_Status status;
       if(rank==r) rc = MPI_Recv(h, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, &status);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  Transform1D(h, w, H);
  
  if(rank==0)
  {
      for(int i=0; i<w; i++)
      {
      Final[i+w*count*numtasks]=H[i];
      }
  }
  
  //Send and Recive
  for(int r=1; r<numtasks; r++)
  {
       if(rank==r) rc = MPI_Send(H, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
       MPI_Status status;
       if(rank==0) rc = MPI_Recv(Final+w*r+w*count*numtasks, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD, &status);
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


Complex* Final_Out = new Complex[Hig*w];

//Print out 
 if(rank==0)
 {
     MatrixRotate(Final_Out, Final, Hig, w);
     image.SaveImageData("MyAfter2D.txt", Final_Out, w, Hig);
    //  for(int s=0; s<(w*Hig); s++)
    //   {
    //       //Final[s].Print();
    //       Final_Out[s].Print();
    //       //Image2[s].Print();
    //       //printf("\t");
    //   }
 } 
 
 //---------------------------------------------------------------------------------------------------------
 /*
 Inverse fft2d starts here
 */

Complex* Final2 = new Complex[Hig*w];

total_count = Hig/numtasks;

for(int count=0; count<total_count; count++)
{
  Complex *h = new Complex[w];
  Complex *H = new Complex[w];
  Complex *Send = new Complex[w];
  int rc;
  
  if(rank==0) h = Final_Out+w*count*numtasks;
  
  for(int r=1; r<numtasks; r++)
  {
       if(rank==0) 
       {
           Send = Final_Out+r*w+w*count*numtasks;
           rc = MPI_Send(Send, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD);
       }
       MPI_Status status;
       if(rank==r) rc = MPI_Recv(h, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, &status);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  InvTransform1D(h, w, H);
  
  if(rank==0)
  {
      for(int i=0; i<w; i++)
      {
      Final2[i+w*count*numtasks]=H[i];
      }
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  //Send and Recive
  for(int r=1; r<numtasks; r++)
  {
       if(rank==r) rc = MPI_Send(H, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
       MPI_Status status;
       if(rank==0) rc = MPI_Recv(Final2+w*r+w*count*numtasks, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD, &status);
  }
  MPI_Barrier(MPI_COMM_WORLD);
}

Complex* Image3 = new Complex[Hig*w];
if(rank==0)
{
    MatrixRotate(Image3, Final2, Hig, w);
}




//2nd fft2-------------------------------------------------------------------------------------------------
total_count = Hig/numtasks;
Final2 = new Complex[Hig*w];
 
for(int count=0; count<total_count; count++)
{
  int rc;
  Complex *h = new Complex[w];
  Complex *H = new Complex[w];
  Complex *Send = new Complex[w];
  
  if(rank==0) h = Image3+w*count*numtasks;
  
  for(int r=1; r<numtasks; r++)
  {
       if(rank==0) 
       {
           Send = Image3+r*w+w*count*numtasks;
           rc = MPI_Send(Send, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD);
       }
       MPI_Status status;
       if(rank==r) rc = MPI_Recv(h, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, &status);
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  InvTransform1D(h, w, H);
  
  if(rank==0)
  {
      for(int i=0; i<w; i++)
      {
      Final2[i+w*count*numtasks]=H[i];
      }
  }
  
  //Send and Recive
  for(int r=1; r<numtasks; r++)
  {
       if(rank==r) rc = MPI_Send(H, 2*w, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
       MPI_Status status;
       if(rank==0) rc = MPI_Recv(Final2+w*r+w*count*numtasks, 2*w, MPI_COMPLEX, r, 0, MPI_COMM_WORLD, &status);
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


Complex* Final_Out2 = new Complex[Hig*w];

//Print out 
 if(rank==0)
 {
     MatrixRotate(Final_Out2, Final2, Hig, w);
     image.SaveImageData("MyAfterInverse.txt", Final_Out2, w, Hig);
    //  for(int s=0; s<(w*Hig); s++)
    //   {
    //       Final_Out2[s].Print();
    //       printf("\t");
    //       //Final_Out[s].Print();
    //       //Image2[s].Print();
    //       //printf("\t");
    //   }
 } 
}

void Transform1D(Complex* h, int N, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.

  for(int n=0; n<N; ++n)
  {
        for(int k=0; k<N; ++k)
        {

            H[n] = H[n]+Complex(+cos(2.0*M_PI*n*k/N), -sin(2.0*M_PI*n*k/N))*h[k];
        }
        if(fabs(H[n].imag)<1e-9) H[n].imag = 0;
        if(fabs(H[n].real)<1e-9) H[n].real = 0;    
  }
}

void InvTransform1D(Complex* h, int N, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  double temp = (double) 1/N;
  for(int n=0; n<N; ++n)
  {
        for(int k=0; k<N; ++k)
        {

            H[n] = H[n]+Complex(temp*cos(2.0*M_PI*n*k/N), +temp*sin(2.0*M_PI*n*k/N))*h[k];
        }
        if(fabs(H[n].imag)<1e-9) H[n].imag = 0;
        if(fabs(H[n].real)<1e-9) H[n].real = 0;    
  }
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  int  rc; 
  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  Transform2D(fn.c_str()); // Perform the transform.
  // Finalize MPI here
  MPI_Finalize();
}  
  

  
