#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
using namespace std;

/**
* Clasa de baza pentru exceptii
*/
class Exception : public exception
{
protected:
	/**
	* Mesajul exceptiei
	*/
	const char* m_message;

public:
	/**
	* Constructor
	*/
	Exception() : m_message("An unknown exception has occured")
	{
	}
	
	/**
	* Constructor ce initializeaza si mesajul
	*/
	Exception(const char* message) : m_message(message)
	{
	}

	/**
	* Mostenita din std::exception
	*/
	virtual const char* what() const throw()
	{
		return m_message;
	}

};

#endif /*EXCEPTIONS_H_*/