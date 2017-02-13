// RSA Assignment for ECE4122/6122 Fall 2015

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;

// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// Fill in the remainder of the RSA_Algorithm methods
void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{
  mpz_class p;
  mpz_class q;
  mpz_class phi;

  while(1)
  {
    p = rng.get_z_bits(sz);
    if(mpz_probab_prime_p(p.get_mpz_t(),100)==1) break;
  }

  while(1)
  {
    q = rng.get_z_bits(sz);
    if(mpz_probab_prime_p(q.get_mpz_t(),100)==1) break;
  }

  n = p * q;
  phi = (p-1) * (q-1);


  while(1)
  {
    d = rng.get_z_bits(2*sz);
    mpz_class cmd;
    mpz_gcd(cmd.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
    if((d<phi)&&(cmd==1)) break;
  }
  
  mpz_class cmd;
  mpz_invert(cmd.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
  e = mpz_class(cmd);
}

  // Encrypt plaintext message M with key  pair n/e
  // By convention, we will make the encryption key e the public key
  // and the decryption key d the private key.
mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{
  mpz_class C;
  mpz_powm (C.get_mpz_t(), M.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
  return C;
}

  // Decrypt ciphertext message C with key pair n/d
mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{
  mpz_class M;
  mpz_powm (M.get_mpz_t(), C.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
  return M;
}


void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}




