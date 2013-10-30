#include "EncrypterMainWindow.h"
#include "../../core/impl/Log.h"
#include <wx/file.h>

// Declaratie evenimente de meniu
BEGIN_EVENT_TABLE(EncrypterMainWindow, wxFrame)
	EVT_MENU(ID_ABOUT,				EncrypterMainWindow::OnAbout)
    EVT_MENU(ID_EXIT,				EncrypterMainWindow::OnQuit)
	EVT_MENU(ID_FILE,				EncrypterMainWindow::OnRunOnFile)
	EVT_COMBOBOX(ID_COMBO_MODE,		EncrypterMainWindow::OnEncryptModeSelect)
	EVT_COMBOBOX(ID_COMBO_KEYLEN,	EncrypterMainWindow::OnKeyLenSelect)
	EVT_COMBOBOX(ID_COMBO_RUNON,	EncrypterMainWindow::OnRunOnSelect)
	EVT_TEXT(ID_TEXT_KEY,			EncrypterMainWindow::OnKeyChanged)
END_EVENT_TABLE()

void EncrypterMainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // Forteaza fereastra sa se inchida
    Close(true);
}

void EncrypterMainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This is an AES encrypt/decrypt application\n")
                    _T("running under %s."),
					wxTheApp->GetAppName(),
                    wxGetOsDescription().c_str()
                 ),
                 _T("About CryptoGPU"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

void EncrypterMainWindow::OnEncryptModeSelect(wxCommandEvent& event)
{
	m_aesMode = event.GetSelection();
	LogMessage("Will run: %s", m_aesMode == 0 ? "Encrypt" : "Decrypt");
}

void EncrypterMainWindow::OnKeyLenSelect(wxCommandEvent& event)
{
	switch(event.GetSelection())
	{
		case 0:
			m_keySize = 128;
			break;
		case 1:
			m_keySize = 192;
			break;
		case 2:
			m_keySize = 256;
			break;
		default:
			break;
	}
	LogMessage("Required key size is now: %d bits", m_keySize);
}

void EncrypterMainWindow::OnRunOnSelect(wxCommandEvent& event)
{
	switch(event.GetSelection())
	{
		case 0:
			m_runMode = CPU;
			break;
		case 1:
			m_runMode = GPU_CUDA;
			break;
		default:
			break;
	}
	LogMessage("Will run on: %s", m_runMode == CPU ? "CPU" : "GPU");
}

void EncrypterMainWindow::OnKeyChanged(wxCommandEvent& event)
{
	wxString cipherKey = event.GetString();
	LogMessage("Key size is now: %d bits", cipherKey.size() * 4);

	// Calculam cheia, daca e posibil
	if (m_cipherKey)
	{
		delete m_cipherKey;
		m_cipherKey = NULL;
	}

	if (cipherKey.size() * 4 != m_keySize)
	{
		return; // Nu are lungimea necesara
	}
	
	// Alocam o noua cheie
	ubyte* tempCipherKey = m_cipherKey = new ubyte[m_keySize / 8];
	const char* buffer = cipherKey.c_str();
	for (int i = 0; i < m_keySize / 4; i += 2)
	{
		// Citim 2 cifre hexa
		wxString hexaGroup = wxString("0x") + wxString(buffer[i]) + wxString(buffer[i+1]);
		unsigned long value;
		hexaGroup.ToULong(&value, 16);
		
		// Trunchiem la octet
		(*tempCipherKey) = (ubyte)value;
		tempCipherKey++;
	}
}

void EncrypterMainWindow::OnRunOnFile(wxCommandEvent& event)
{
	wxFileDialog dialog(this, wxT("Open File"), wxT("c:\\"), wxEmptyString, wxT("*.*"), wxOPEN);
	wxString filename;
	if (dialog.ShowModal() == wxID_OK)
	{
		filename = dialog.GetPath();
	}
	else
	{
		LogMessage("No file selected");
		return;
	}

	if (!m_cipherKey)
	{
		LogError("No cipher key!");
		return;
	}

	wxFile fileToProcess;
	if (!fileToProcess.Open(filename, wxFile::read))
	{
		LogError("Couldn't read file %s", filename);
        return;
	}

	wxFile fileToWrite;
	wxString extension = filename.AfterLast('.');
	filename = filename.BeforeLast('.'); // Extragem fisierul fara extensie
	filename = filename + (m_aesMode == 0 ? wxString(".enc.") : wxString(".dec.")) + extension;
	if (fileToWrite.Exists(filename))
	{
		LogError("Output file already exists!");
		return;
	}
	if (!fileToWrite.Open(filename, wxFile::write))
	{
		LogError("Couldn't write file %s", filename);
		return;
	}

	// Restart
	m_progress->SetValue(0);

	// Citim datele din fisier
	LogMessage("Reading file...");
	wxFileOffset size = fileToProcess.Length();
	// TODO: nu citi tot fisierul in memorie
	ubyte* fileData = new ubyte[size];
	fileToProcess.Read(fileData, size);
	fileToProcess.Close();
	
	m_progress->SetValue(10);

	// Executam operatia
	AESEncrypter encrypter(m_runMode);

	// Setam cheia
	LogMessage("Expanding cipher key...");
	encrypter.SetKey(m_cipherKey, m_keySize, m_aesMode == 0 ? ENCRYPT : DECRYPT);
	
	m_progress->SetValue(20);

	// Criptam/decriptam
	LogMessage(m_aesMode == 0 ? "Encrypting..." : "Decrypting...");
	ubyte* output = new ubyte[size];
	if (m_aesMode == 0)
	{
		// Trebuie sa pastram padding-ul ca sa putem decripta corect
		int blockSize = m_runMode == CPU ? 16 : 1024;
		encrypter.Encrypt(fileData, output, size);
		if (size % blockSize)
		{
			int noBlocks = size / blockSize;
			size = (noBlocks + 1) * blockSize;
		}
	}
	else
		encrypter.Decrypt(fileData, output, size);
	delete[] fileData;

	m_progress->SetValue(90);

	// Salvam in fisier rezultatul
	LogMessage("Saving output to %s", filename);
	fileToWrite.Write(output, size);
	fileToWrite.Close();
	LogMessage("Done!");
	delete[] output;

	m_progress->SetValue(100);
}

EncrypterMainWindow::EncrypterMainWindow() : wxFrame(NULL, wxID_ANY, wxT("CryptoGPU"), wxDefaultPosition, wxSize(640, 300)), m_keySize(128), m_aesMode(0), m_runMode(CPU), m_cipherKey(NULL)
{
    // Creeaza meniu
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(ID_FILE, wxT("&Run On File..."), wxT("Encrypt/decrypt a file"));
	fileMenu->Append(ID_EXIT, wxT("E&xit"), wxT("Exit application"));

    // Adauga meniu de help
    wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(ID_ABOUT, wxT("&About...\tF1"), wxT("Show about dialog"));
	
	// Creeaza menu bar
	wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(helpMenu, wxT("&Help"));

	// Ataseaza meniu bar-ul
	this->SetMenuBar(menuBar);
	
	// Creaza status bar
	this->CreateStatusBar(1);
	this->SetStatusText(wxT("CryptoGPU"));

	// Populam fereastra cu elemente
	wxPanel* panel = new wxPanel(this, wxID_ANY); // Container pt toate elementele

	// Sizer-ul de layout top-level
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

	// Modul de criptare
	wxBoxSizer* upsizer = new wxBoxSizer(wxHORIZONTAL);
	upsizer->Add(new wxStaticText(panel, wxID_STATIC,
		wxT("AES Mode:"),
		wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER));
	wxArrayString stringsMode;
	stringsMode.Add(wxT("Encrypt"));
	stringsMode.Add(wxT("Decrypt"));
	upsizer->Add(new wxComboBox(panel, ID_COMBO_MODE,
		wxT("Encrypt"), wxDefaultPosition, wxSize(100, 20),
		stringsMode, wxCB_READONLY));
	topsizer->Add(upsizer);
	
	
	// Lungime cheie
	wxBoxSizer* middlesizer1 = new wxBoxSizer(wxHORIZONTAL); 
	middlesizer1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Key length:"), wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER));
	wxArrayString stringsKey;
	stringsKey.Add(wxT("128"));
	stringsKey.Add(wxT("192"));
	stringsKey.Add(wxT("256"));
	middlesizer1->Add(new wxComboBox(panel, ID_COMBO_KEYLEN,
		wxT("128"), wxDefaultPosition, wxSize(100, 20),
		stringsKey, wxCB_READONLY));
	topsizer->Add(middlesizer1);
	
	// Cheia in hex
	wxBoxSizer* middlesizer2 = new wxBoxSizer(wxHORIZONTAL);
	middlesizer2->Add(new wxStaticText(panel, wxID_STATIC, wxT("AES Key(hex):"), wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER));
	middlesizer2->Add(new wxTextCtrl(panel, ID_TEXT_KEY,
		wxEmptyString, wxDefaultPosition, wxSize(450, 20),
		wxTE_PROCESS_ENTER));
	topsizer->Add(middlesizer2);
	
	// Optiune rulare
	wxBoxSizer* middlesizer3 = new wxBoxSizer(wxHORIZONTAL);
	middlesizer3->Add(new wxStaticText(panel, wxID_STATIC, wxT("Run On:"), wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER));
	wxArrayString stringsRunOn;
	stringsRunOn.Add(wxT("CPU"));
	stringsRunOn.Add(wxT("GPU"));
	middlesizer3->Add(new wxComboBox(panel, ID_COMBO_RUNON,
		wxT("CPU"), wxDefaultPosition, wxSize(100, 20),
		stringsRunOn, wxCB_READONLY));
	topsizer->Add(middlesizer3);

	// Indicator progres
	wxBoxSizer* middlesizer4 = new wxBoxSizer(wxHORIZONTAL);
	middlesizer4->Add(new wxStaticText(panel, wxID_STATIC, wxT("Operation Progress:"), wxDefaultPosition, wxSize(100, 20), wxALIGN_CENTER));
	middlesizer4->Add(m_progress = new wxGauge(panel, wxID_STATIC,
		100, wxDefaultPosition, wxSize(450, 20), wxGA_HORIZONTAL));
	topsizer->Add(middlesizer4);

	// Info user
	wxBoxSizer* bottomsizer = new wxBoxSizer(wxHORIZONTAL);
	bottomsizer->Add(new wxStaticText(panel, wxID_STATIC, 
		wxT("Select File->Run On File...\n to start encrypting/decrypting a file.\nOutput will be stored as *filename*.enc[dec].[ext]"),
		wxDefaultPosition, wxSize(640,100), wxALIGN_CENTER));
	topsizer->Add(bottomsizer);

	panel->SetSizer(topsizer);
}

EncrypterMainWindow::~EncrypterMainWindow()
{
	if (m_cipherKey)
		delete m_cipherKey;
}