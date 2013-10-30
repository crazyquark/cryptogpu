#include <cutil_inline.h>		  // Utilitare CUDA
#include "CUDAEncrypter.h"
#include "Log.h"
#include "Exceptions.h"
#include "CUDAEncrypterKernel.cu" // Cod ce ruleaza pe GPU
/**
* Contine functiile de criptare folosind CUDA,
* partea de cod ce ruleaza pe gazda
*/

extern "C"
{
static unsigned int timer = 0;

/**
* Initializare CUDA
*/
void CUDAInit()
{
	int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    if(deviceCount == 0)
	{
		LogError("No CUDA device available!");
		return;
	}

	// Setam device-ul CUDA(GPU-ul cel mai rapid din sistem)
	int cudaDevice = cutGetMaxGflopsDeviceId();
	cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, cudaDevice);
	LogMessage("Using device: %s", deviceProp.name);
	cudaSetDevice(cudaDevice);
}

void CUDAEncryptHost(ubyte* input, ubyte* output, AesKey* encryptKey, int noBlocks)
{
	// Un bloc de fire de executie cripteaza 256 x 4 octeti = 1024 de octeti = 1K
	dim3 dimBlock(256);
	dim3 dimGrid(noBlocks); // Numarul de blocuri CUDA = numarul de blocuri de 1K din input

	// Alocam memorie pe device
	ubyte* deviceInput;
	cutilSafeCall(cudaMalloc((void**)&deviceInput, noBlocks * 1024));

	ubyte* deviceOutput;
	cutilSafeCall(cudaMalloc((void**)&deviceOutput, noBlocks * 1024));
	
	uint32* deviceRoundKeys;
	cutilSafeCall(cudaMalloc((void**)&deviceRoundKeys, 4 * 4 * (AES_MAXNR + 1)));

	// Copiem input-ul pe device
	cutilSafeCall(cudaMemcpy(deviceInput, input, noBlocks * 1024, cudaMemcpyHostToDevice));

	// Copiem cheile
	cutilSafeCall(cudaMemcpy(deviceRoundKeys, encryptKey->roundKeys,  
		4 * 4 * (AES_MAXNR + 1), cudaMemcpyHostToDevice));

	// Cream un timer
    cutilCheckError(cutCreateTimer(&timer));
    cutilCheckError(cutStartTimer(timer));

	// Lansam kernelul
	CUDAEncryptKernel<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, 
		deviceRoundKeys, encryptKey->noRounds);

	// Verificam ca nu au aparut erori
	cutilCheckMsg("Kernel execution failed");

	// Copiem rezultatul de pe device
	cutilSafeCall(cudaMemcpy(output, deviceOutput, noBlocks * 1024, cudaMemcpyDeviceToHost));

	// Oprim timer-ul si afisam tipul de executie pe GPU
    cutilCheckError(cutStopTimer(timer));
    LogMessage("Processing time: %f (ms)", cutGetTimerValue(timer));
    cutilCheckError(cutDeleteTimer(timer));

	// Dealocam memoria pe device
	cutilSafeCall(cudaFree(deviceInput));
    cutilSafeCall(cudaFree(deviceOutput));
    cutilSafeCall(cudaFree(deviceRoundKeys));

	// Oprim threaduri
	cudaThreadExit();
}

void CUDADecryptHost(ubyte* input, ubyte* output, AesKey* decryptKey, int noBlocks)
{
	// Un bloc de fire de executie cripteaza (16 x 16) x 4 octeti = 1024 de octeti = 1K
	dim3 dimBlock(256);	// 16 x 16 = un bloc de 4 x 4 octeti (o stare de 128 de biti) x 16 = 16 blocuri de 128
	dim3 dimGrid(noBlocks); // Numarul de blocuri CUDA = numarul de blocuri de 1K din input

	// Alocam memorie pe device
	ubyte* deviceInput;
	cutilSafeCall(cudaMalloc((void**)&deviceInput, noBlocks * 1024));

	ubyte* deviceOutput;
	cutilSafeCall(cudaMalloc((void**)&deviceOutput, noBlocks * 1024));
	
	uint32* deviceRoundKeys;
	cutilSafeCall(cudaMalloc((void**)&deviceRoundKeys, 4 * 4 * (AES_MAXNR + 1)));

	// Copiem input-ul pe device
	cutilSafeCall(cudaMemcpy(deviceInput, input, noBlocks * 1024, cudaMemcpyHostToDevice));

	// Copiem cheile
	cutilSafeCall(cudaMemcpy(deviceRoundKeys, decryptKey->roundKeys,  
		4 * 4 * (AES_MAXNR + 1), cudaMemcpyHostToDevice));

	// Cream un timer
    cutilCheckError(cutCreateTimer(&timer));
    cutilCheckError(cutStartTimer(timer));

	// Lansam kernelul
	CUDADecryptKernel<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, 
		deviceRoundKeys, decryptKey->noRounds);

	// Verificam ca nu au aparut erori
	cutilCheckMsg("Kernel execution failed");

	// Copiem rezultatul de pe device
	cutilSafeCall(cudaMemcpy(output, deviceOutput, noBlocks * 1024, cudaMemcpyDeviceToHost));

	// Oprim timer-ul si afisam tipul de executie pe GPU
    cutilCheckError(cutStopTimer(timer));
    LogMessage("Processing time: %f (ms)", cutGetTimerValue(timer));
    cutilCheckError(cutDeleteTimer(timer));

	// Dealocam memoria pe device
	cutilSafeCall(cudaFree(deviceInput));
    cutilSafeCall(cudaFree(deviceOutput));
    cutilSafeCall(cudaFree(deviceRoundKeys));

	// Oprim threaduri
	cudaThreadExit();
}

}