#ifndef ENUMTYPES_H_
#define ENUMTYPES_H_

/**
* Specifica cum va rula un algoritm: 
* pe CPU, pe GPU/CUDA sau pe GPU/OPENGL 
*/
enum eRunMode
{
	CPU,
	GPU_CUDA,
	GPU_OPENGL
};

/**
* Specifica daca se executa criptare sau decriptare
*/
enum eOperation
{
	ENCRYPT,
	DECRYPT,
	ENCRYPT_AND_DECRYPT
};

#endif // ENUMTYPES_H_