#ifndef ENCRYPTERMAINWINDOW_H_
#define ENCRYPTERMAINWINDOW_H_

#include <wx/wx.h>
#include "../../core/impl/AESEncrypter.h"

/**
* IDuri evenimente
*/
enum eEventIDs
{
	ID_EXIT = wxID_EXIT,
	ID_ABOUT = wxID_ABOUT,
	ID_COMBO_MODE,
	ID_COMBO_KEYLEN,
	ID_TEXT_KEY,
	ID_COMBO_RUNON,
	ID_FILE
};

/**
* Fereastra principala a aplicatiei
*/
class EncrypterMainWindow : public wxFrame
{
protected:
	/**
	* Criptare/Decriptare
	*/
	int m_aesMode;

	/**
	* Modul de rulare CPU/GPU
	*/
	eRunMode m_runMode;

	/**
	* Cheia cifrului
	*/
	ubyte* m_cipherKey;
	
	/**
	* Dimensiunea cheii 128/192/256
	*/
	int m_keySize;

	/**
	* Indicator progress
	*/
	wxGauge* m_progress;

public:
	/**
	* Constructor
	*/
	EncrypterMainWindow();

	/**
	* Destructor
	*/
	~EncrypterMainWindow();
	
	/**
	* Trateaza evenimentul de quit
	*/
	void OnQuit(wxCommandEvent& event);
    
	/**
	* Trateaza evenimentul de help->about
	*/
	void OnAbout(wxCommandEvent& event);

	/**
	* Trateaza evenimentul de selectare a modului de functionare AES
	*/
	void OnEncryptModeSelect(wxCommandEvent& event);
	
	/**
	* Trateaza evenimentul de selectare a lungimii cheii
	*/
	void OnKeyLenSelect(wxCommandEvent& event);

	/**
	* Trateaza evenimentul de selectare rulari pe CPU/GPU
	*/
	void OnRunOnSelect(wxCommandEvent& event);

	/**
	* Trateaza evenimentul de introducere a cheii de criptare/decriptare
	*/
	void OnKeyChanged(wxCommandEvent& event);

	/**
	* Evenimentul de incarcare al unui fisier
	*/
	void OnRunOnFile(wxCommandEvent& event);

private:
	/**
	* Trateaza evenimente
	*/
	DECLARE_EVENT_TABLE()
};

#endif /*ENCRYPTERMAINWINDOW_H_*/