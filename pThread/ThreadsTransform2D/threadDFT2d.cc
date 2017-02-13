// Threaded two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 2

#include <iostream>
#include <string>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.
int N;
int* Sort;
Complex* Wei;
Complex* InvWei;
Complex* data;
int task_per_thread;
pthread_mutex_t exitMutex;
pthread_cond_t  exitCond;
int P;
int count;
int FetchAndIncrementCount();
pthread_mutex_t countMutex;
pthread_mutex_t startCountMutex;
Complex* Temp_img;
Complex* Temp_img2;
int startCount;
using namespace std;
bool* localSense;
bool globalSense;

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
// 
// // Function to reverse bits in an unsigned integer
// // This assumes there is a global variable N that is the
// // number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

//Reorder H
void ReorderH(int* Sort, int N)
{
  //int num = log2(N);
  
  for(int i=0; i<N; i++)
  {
    //   int store=0;
    //   int temp = i;
    //   for(int k=0; k<num; k++)
    //   {
    //       if(temp % 2 == 0)
    //       {
    //           store=store<<1;
    //       }
    //       
    //       else
    //       {
    //           store=(store<<1)+1;
    //       }
    //       temp=temp/2;
    //   }
    //   Sort[i]=store;
    Sort[i]=ReverseBits(i);
    //cout<<Sort[i]<<endl;
  }
}

//PreCal Weight
void preCalW(Complex* Wei, int N)
{
    for(int i=0; i<N; i++)
    {
        Wei[i]=Complex(+cos(2*M_PI*i/N),-sin(2*M_PI*i/N));
    }
}

void preCalInvW(Complex* InvWei, int N)
{
    for(int i=0; i<N; i++)
    {
        InvWei[i]=Complex(+cos(2*M_PI*i/N),+sin(2*M_PI*i/N));
    }
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
void MyBarrier_Init(int threads)// you will likely need some parameters)
{
    P=threads;
    count=P;
}

int FetchAndIncrementCount()
{
    pthread_mutex_lock(&countMutex);
    int myCount = count;
    count--;
    pthread_mutex_unlock(&countMutex);
    return myCount;
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(int myId) // Again likely need parameters
{
    localSense[myId] = !localSense[myId];
    pthread_mutex_lock(&countMutex);
    int myCount = count;
    count--;
    pthread_mutex_unlock(&countMutex);
    if(myCount==1)
    {
        count = P;
        globalSense = localSense[myId];
    }
    else
    {
        while (globalSense != localSense[myId]){};
    }
    //printf("Done Here %d\n",myId);
}
                    
void Transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
  
  int num = log2(N);
  Complex H[N];
  //Replace
  for(int i=0; i<N; i++) {H[i] = h[Sort[i]];}
  for(int i=0; i<N; i++) {h[i] = H[i];}
  //Done Replace
  for(int i=1; i<=num; i++)
  {
      int ranking = pow(2,i);
      Complex Backup[ranking];
      int time = N/ranking;
      for (int k=0; k<time; k++)
      {
          for(int r=0; r<ranking; r++)
          {
              Backup[r] = h[r+k*ranking];
          }
          
          for(int r=0; r<ranking; r++)
          {
              int b = r%(ranking/2);
              int u = b + (ranking/2);
              int w = r*N/ranking;
              h[r+k*ranking] = Backup[b] + Wei[w]*Backup[u];
          }
      }
  }
  
}


void InvTransform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
  
  int num = log2(N);
  Complex H[N];
  //Replace
  for(int i=0; i<N; i++) {H[i] = h[Sort[i]];}
  for(int i=0; i<N; i++) {h[i] = H[i];}
  //Done Replace
  for(int i=1; i<=num; i++)
  {
      int ranking = pow(2,i);
      Complex Backup[ranking];
      int time = N/ranking;
      for (int k=0; k<time; k++)
      {
          for(int r=0; r<ranking; r++)
          {
              Backup[r] = h[r+k*ranking];
          }
          
          for(int r=0; r<ranking; r++)
          {
              int b = r%(ranking/2);
              int u = b + (ranking/2);
              int w = r*N/ranking;
              h[r+k*ranking] = Backup[b] + InvWei[w]*Backup[u];
              //h[r+k*ranking] = Complex(h[r+k*ranking].real/N, h[r+k*ranking].imag/N);
          }
      }
  }
  
  for(int i=0; i<N; i++)
  {
      h[i] = h[i] *(1.0/N);
  }
  
}


void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  unsigned long myId = (unsigned long)v;
  int ID = (int)myId;
  
  for(int i=0; i<task_per_thread; i++)
  {
        Complex *h = new Complex[N];
        h = data + N*i+N*task_per_thread*ID;
        Transform1D(h, N);
  }
  MyBarrier(ID);
  
  if(ID==0)
  {
      MatrixRotate(Temp_img, data, N, N);
  }
  
  MyBarrier(ID);
  for(int i=0; i<task_per_thread; i++)
  {
        Complex *h = new Complex[N];
        h = Temp_img + N*i+N*task_per_thread*ID;
        Transform1D(h, N);
  }
  MyBarrier(ID);
  if(ID==0)
  {
      MatrixRotate(data, Temp_img, N, N);
  }
  MyBarrier(ID);
  
  //-------------------------------------------------------------------------------------------------
  //Inverse Transform1D
//   if(ID==0)
//   {
//       MatrixRotate(Temp_img, data, N, N);
//   }
//   MyBarrier(ID);
  
  for(int i=0; i<task_per_thread; i++)
  {
        Complex *h = new Complex[N];
        h = Temp_img + N*i+N*task_per_thread*ID;
        InvTransform1D(h, N);
  }
  MyBarrier(ID);
  
  if(ID==0)
  {
      MatrixRotate(Temp_img2, Temp_img, N, N);
  }
  
  MyBarrier(ID);
  for(int i=0; i<task_per_thread; i++)
  {
        Complex *h = new Complex[N];
        h = Temp_img2 + N*i+N*task_per_thread*ID;
        InvTransform1D(h, N);
  }
  MyBarrier(ID);
//   if(ID==0)
//   {
//       MatrixRotate(Temp_img, Temp_img2, N, N);
//   }
//   MyBarrier(ID);
  
  
  
  
  //Ending and send signal
  pthread_mutex_lock(&startCountMutex);
  startCount--;
  if (startCount == 0)
  { // Last to exit, notify main
  pthread_mutex_unlock(&startCountMutex);
  pthread_mutex_lock(&exitMutex);
  pthread_cond_signal(&exitCond);
  pthread_mutex_unlock(&exitMutex);
  }
  else
  {
      pthread_mutex_unlock(&startCountMutex);
  }
  return 0;
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  InputImage image(inputFN);  // Create the helper object for reading the image
  // Create the global pointer to the image array data
  data = image.GetImageData();
  int Wid = image.GetWidth();
  int Hig = image.GetHeight();
  Temp_img = new Complex[Hig*Wid];
  Temp_img2 = new Complex[Hig*Wid];
  N =Wid;
  Sort = new int[N];
  Wei = new Complex[N];
  InvWei = new Complex[N];
  ReorderH(Sort, Wid);
  preCalW(Wei, Wid);
  preCalInvW(InvWei, Wid);

  // Create 16 threads
  int nThreads = 16;
  startCount = nThreads;
  MyBarrier_Init(nThreads);
  localSense = new bool[nThreads];
  for (int i = 0; i < nThreads; ++i) localSense[i] = true;
  globalSense = true;
  pthread_mutex_init(&countMutex, 0);
  pthread_mutex_init(&exitMutex, 0);
  pthread_cond_init(&exitCond, 0);
  pthread_mutex_lock(&exitMutex);
  task_per_thread = Hig/nThreads;
  for(int i=0; i<nThreads; ++i)
  {
      pthread_t pt;
      pthread_create(&pt, 0, Transform2DTHread, (void*)i);
  }
  pthread_cond_wait(&exitCond, &exitMutex);//Or maybe do a 2nd Barrier at the end of each thread
  // Wait for all threads complete
  // Write the transformed data
  image.SaveImageData("MyAfter2D.txt", data, N, N);
  image.SaveImageDataReal("MyAfterInverse.txt", Temp_img2, N, N);
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  Transform2D(fn.c_str()); // Perform the transform.
  return 0;
} 
  

  
