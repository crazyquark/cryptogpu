#ifndef ENCRYPTERAPP_H_
#define ENCRYPTERAPP_H_

#include "EncrypterMainWindow.h"
#include <wx/wx.h>

/**
* Clasa de baza a interfetei grafice, conform cerintelor
* wxWidgets, derivata din wxApp
*/
class EncrypterApp : public wxApp
{
protected:
	/**
	* Fereastra principala
	*/
	EncrypterMainWindow* m_mainWindow;
	
	/**
	* Fereastra de log
	*/
	wxLogWindow* m_logWindow;

	/**
	* Echivalentul functiei main pt. aplicatii wxWdigets
	*/
	virtual bool OnInit();

	/**
	* Functie apelata la iesirea din aplicatie
	*/
	virtual int OnExit();

public:
	~EncrypterApp();
};

#endif /*ENCRYPTERAPP_H_*/