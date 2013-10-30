#include "EncrypterApp.h"


bool EncrypterApp::OnInit()
{
	// Apelam metoda din clasa de baza
	if(!wxApp::OnInit())
        return false;
	
	// Modifica loggerul implicit
	delete wxLog::SetActiveTarget(new wxLogStderr(fopen("cryptogpu.log", "w")));

	// Creaza fereastra principala a aplicatiei
	// Nota: nu e nevoie sa facem delete la aceasta resursa, 
	// este dealocata de framework, la inchiderea ferestrei
	m_mainWindow = new EncrypterMainWindow;
	
	// Afiseaza fereastra principala
	m_mainWindow->CenterOnScreen();
	m_mainWindow->Show();
	
	// Creaza si afiseaza fereastra de log
	m_logWindow = new wxLogWindow(m_mainWindow, this->GetAppName());
	m_logWindow->GetFrame()->SetPosition
	(
		m_mainWindow->GetPosition() + 
		wxPoint(0, m_mainWindow->GetSize().GetHeight())
	);

	return true; // Porneste aplicatia
}

int EncrypterApp::OnExit()
{
	return 0;
}

EncrypterApp::~EncrypterApp()
{
}

/**
* Creeaza instanta aplicatiei definita de EncrypterApp
*/
IMPLEMENT_APP(EncrypterApp)
DECLARE_APP(EncrypterApp)

