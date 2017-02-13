// ECE4122/6122 RSA Encryption/Decryption assignment
// Fall Semester 2015

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;


int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;
  
  // Loop from sz = 32 to 1024 inclusive
  // for each size choose 10 different key pairs
  // For each key pair choose 10 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For eacm message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.

  // your code here
  for (size_t sz=32; sz <= 1024; sz=sz*2)
  {
    for(int i=0; i<100; i++)
    {
      RSA.GenerateRandomKeyPair(sz);
      RSA.PrintNDE();
      for(int k=0; k<100; k++)
      {
          mpz_class M = RSA.rng.get_z_bits(sz);
          RSA.PrintM(M);
          mpz_class C = RSA.Encrypt(M);
          RSA.PrintC(C);
          mpz_class OUT = RSA.Decrypt(C);
          //RSA.PrintM(OUT);
      }
    }
  }
  
}
  
