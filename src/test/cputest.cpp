#include "../core/impl/AESEncrypter.h"
#include <iostream>
using namespace std;

int cputest()
{
	ubyte test1key[] = { 0xE8, 0xE9, 0xEA, 0xEB, 0xED, 0xEE, 0xEF, 0xF0, 0xF2, 0xF3, 0xF4, 0xF5, 0xF7, 0xF8, 0xF9, 0xFA };
	ubyte test1pt[] = { 0x01, 0x4B, 0xAF, 0x22, 0x78, 0xA6, 0x9D, 0x33, 0x1D, 0x51, 0x80, 0x10, 0x36, 0x43, 0xE9, 0x9A };
	
	ubyte test1ct[16];
	memset (&test1ct, 0, 16);
	AESEncrypter encrypter(CPU);
	encrypter.SetKey((const ubyte*)&test1key, 128, ENCRYPT_AND_DECRYPT);
	//encrypter.Encrypt((ubyte*)&test1pt, (ubyte*)&test1ct, 16);
	
	for (int i = 0; i < 16; i++)
		cout<<hex<<(int)test1ct[i];
	cout<<endl;
	
	//encrypter.Decrypt((ubyte*)test1ct, (ubyte*)test1pt, 16);
	
	cin.ignore();

	return 0;
}