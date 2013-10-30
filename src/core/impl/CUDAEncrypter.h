#ifndef CUDAENCRYPTER_H_
#define CUDAENCRYPTER_H_

#include "AESEncrypter.h"

/**
* Interfata pentru functiile implementate in CUDA,
* trebuie exportate simbolurile in C strict
*/
extern "C"
{
	/**
	* Initializare CUDA
	*/
	void CUDAInit();

	/**
	* Functia de criptare pe CUDA
	*/
	void CUDAEncryptHost(ubyte* input, ubyte* output, AesKey* encryptKey, int noBlocks); 
	
	/**
	* Functia de decriptare pe CUDA
	*/
	void CUDADecryptHost(ubyte* input, ubyte* output, AesKey* decryptKey, int noBlocks); 
}

#endif /*CUDAENCRYPTER_H_*/