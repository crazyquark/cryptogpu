#ifndef AESENCRYPTER_H_
#define AESENCRYPTER_H_

#include "../intf/Encrypter.h"
#include "EnumTypes.h"
#include <cstdlib>
using namespace std;

#define AES_MAXNR 14 // Numarul maxim de runde AES

// Operatii pe biti, din OpenSSL
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64))
# define SWAP(x) (x) /*(_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00) */ // In practica, nu avem nevoie sa facem swap-ul octetilor
# define GETU32(p) SWAP(*((uint32 *)(p)))
# define PUTU32(ct, st) { *((uint32 *)(ct)) = SWAP((st)); }
#else
# define GETU32(pt) (((uint32)(pt)[0] << 24) ^ ((uint32)(pt)[1] << 16) ^ ((uint32)(pt)[2] <<  8) ^ ((uint32)(pt)[3]))
# define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
#endif


/**
* Reprezinta cheia extinsa
*/
struct AesKey
{
	/**
	* Cheile de runda
	*/
	uint32 roundKeys[4 *(AES_MAXNR + 1)];

	/**
	* Numarul de runde
	*/
	int noRounds;
};

/**
* Implementeaza clasa de criptare folosind algoritmul AES
*/
class AESEncrypter : public Encrypter
{
protected:
	/**
	* Modul de rulare: pe CPU sau pe GPU
	*/
	eRunMode m_runMode;

	/**
	* T-Box-urile pentru criptare
	*/

	/**
	* Te0[x] = S [x].[02, 01, 01, 03];
	*/
	static const uint32 Te0[256];
	/**
	* Te1[x] = S [x].[03, 02, 01, 01];
	*/
	static const uint32 Te1[256];
	/**
	* Te2[x] = S [x].[01, 03, 02, 01];
	*/
	static const uint32 Te2[256];
	/**
	* Te3[x] = S [x].[01, 01, 03, 02];
	*/
	static const uint32 Te3[256];
	
	/**
	* Valori rcon pentru AES(puteri ale lui 2, 
	* avem nevoie de maxim 10)
	*/
	static const uint32 Rcon[10];

	/**
	* T-box-urile pentru decriptare
	*/

	/**
	* Td0[x] = Si[x].[0e, 09, 0d, 0b];
	*/
	static const uint32 Td0[256];
	
	/**
	* Td1[x] = Si[x].[0b, 0e, 09, 0d];
	*/
	static const uint32 Td1[256];

	/**
	* Td2[x] = Si[x].[0d, 0b, 0e, 09];
	*/
	static const uint32 Td2[256];

	/**
	* Td3[x] = Si[x].[09, 0d, 0b, 0e];
	*/
	static const uint32 Td3[256];

	/**
	* Td4[x] = Si[x].[01];
	*/
	static const ubyte Td4[256];

	/**
	* Cheia extinsa de criptare
	*/
	AesKey* m_encryptKey;
	
	/**
	* Cheia extinsa de decriptare
	*/
	AesKey* m_decryptKey;

	/**
	* Ruleaza criptarea pe CPU, un bloc de 128 de biti
	*/
	void CPUEncryptBlock(ubyte* input, ubyte* output);

	/**
	* Ruleaza criptarea pe GPU folosind CUDA
	*/
	void CUDAEncrypt(ubyte* input, ubyte* output, int noBlocks);

	/**
	* Ruleaza criptarea pe GPU folosind OpenGL
	*/
	void OGLEncrypt(ubyte* input, ubyte* output);
	
	/**
	* Expandeaza cheia de cifru in cheia de criptare extinsa
	*/
	void ExpandEncryptKey(const ubyte* cipherKey, int keySize);

	/**
	* Ruleaza decriptarea pe CPU, un bloc de 128 de biti
	*/
	void CPUDecryptBlock(ubyte* input, ubyte* output);

	/**
	* Ruleaza decriptarea pe GPU folosind CUDA
	*/
	void CUDADecrypt(ubyte* input, ubyte* output, int noBlocks);

	/**
	* Ruleaza decriptarea pe GPU folosind OpenGL
	*/
	void OGLDecrypt(ubyte* input, ubyte* output);
	
	/**
	* Expandeaza cheia de cifru in cheia de decriptare extinsa
	*/
	void ExpandDecryptKey(const ubyte* cipherKey, int keySize);

	/**
	* Daca dimensiunea bufferelor nu e multiplu de 128 biti, trebuie
	* adaugate 0-uri la final
	*/
	void PadData(ubyte*& input, ubyte*& output, int size, int blockSize, int noBlocks);

public:
	/**
	* Constructor
	*/
	AESEncrypter(eRunMode runMode = CPU);

	/**
	* Mostenit din Encrypter, cripteaza input, si scrie in output rezultatul
	*/
	void Encrypt(ubyte*& input, ubyte*& output, int size);
	
	/**
	* Mostenit din Encrypter, decripteaza input, si scrie in output rezultatul
	*/
	void Decrypt(ubyte*& input, ubyte*& output, int size);

	/**
	* Stabileste pe baza cheii de cifru, cheia extinsa si programul de chei
	* (key schedule)
	*/
	void SetKey(const ubyte* cipherKey, int keySize, eOperation operation = ENCRYPT_AND_DECRYPT);

	virtual ~AESEncrypter();
};

#endif // AESENCRYPTER_H_