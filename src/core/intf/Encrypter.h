#ifndef ENCRYPTER_H_
#define ENCRYPTER_H_

#include "PrimitiveTypes.h"

/**
* Clasa abstacta ce defineste interfata unui obiect 
* ce ofera servicii de criptare 
*/
class Encrypter
{
public:
	/**
	* Cripteaza datele din blocul de intrare
	* si le scrie in blocul de iesire, ambele trebuie
	* alocate de cel care cheama functia.
	* Size e dimensiunea bucatii de criptat, in octeti
	*/
	virtual void Encrypt(ubyte*& input, ubyte*& output, int size) = 0;
	
	/**
	* Executa operatia inversa criptarii, decriptarea.
	* Intrarea si iesirea trebuie alocate de apelant.
	* Size e dimensiunea bucatii de decriptat, in octeti
	*/
	virtual void Decrypt(ubyte*& input, ubyte*& output, int size) = 0;

	/**
	* Pentru mecanismul de polimorfism, destructor gol
	*/
	virtual ~Encrypter() { };
};

#endif // ENCRYPTER_H_