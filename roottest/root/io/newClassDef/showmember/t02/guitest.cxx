// @(#)root/test:$Id$
// Author: Fons Rademakers   07/03/98

// guitest.cxx: test program for ROOT native GUI classes.
// To run it do: make guitest; guitest
// Another version with identical functionality but using the new signals
// and slots communication mechanism can be found in $ROOTSYS/tutorials.
// That version can be run entirely in the interpreter.

#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>

#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TEnv.h>


enum ETestCommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_PRINT,
   M_FILE_PRINTSETUP,
   M_FILE_EXIT,

   M_TEST_DLG,
   M_TEST_MSGBOX,
   M_TEST_SLIDER,
   M_TEST_SHUTTER,
   M_TEST_PROGRESS,
   M_TEST_NUMBERENTRY,
   M_TEST_NEWMENU,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT,

   M_CASCADE_1,
   M_CASCADE_2,
   M_CASCADE_3,

   M_NEW_REMOVEMENU,

   VId1,
   HId1,
   VId2,
   HId2,

   VSId1,
   HSId1,
   VSId2,
   HSId2
};


Int_t mb_button_id[9] = { kMBYes, kMBNo, kMBOk, kMBApply,
                          kMBRetry, kMBIgnore, kMBCancel,
                          kMBClose, kMBDismiss };

EMsgBoxIcon mb_icon[4] = { kMBIconStop, kMBIconQuestion,
                           kMBIconExclamation, kMBIconAsterisk };

const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            0,               0 };

struct shutterData_t {
   const char *pixmap_name;
   const char *tip_text;
   Int_t       id;
   TGButton   *button;
};

shutterData_t histo_data[] = {
   { "h1_s.xpm",        "TH1",      1001,  0 },
   { "h2_s.xpm",        "TH2",      1002,  0 },
   { "h3_s.xpm",        "TH3",      1003,  0 },
   { "profile_s.xpm",   "TProfile", 1004,  0 },
   { 0,                 0,          0,     0 }
};

shutterData_t function_data[] = {
   { "f1_s.xpm",        "TF1",      2001,  0 },
   { "f2_s.xpm",        "TF2",      2002,  0 },
   { 0,                 0,          0,     0 }
};

shutterData_t tree_data[] = {
   { "ntuple_s.xpm",    "TNtuple",  3001,  0 },
   { "tree_s.xpm",      "TTree",    3002,  0 },
   { "chain_s.xpm",     "TChain",   3003,  0 },
   { 0,                 0,          0,     0 }
};


const char *editortxt =
"This is the ROOT text edit widget TGTextEdit. It is not intended as\n"
"a full developers editor, but it is relatively complete and can ideally\n"
"be used to edit scripts or to present users editable config files, etc.\n\n"
"The text edit widget supports standard emacs style ctrl-key navigation\n"
"in addition to the arrow keys. By default the widget has under the right\n"
"mouse button a popup menu giving access to several built-in functions.\n\n"
"Cut, copy and paste between different editor windows and any other\n"
"standard X11 text handling application is supported.\n\n"
"Text can be selected with the mouse while holding the left button\n"
"or with the arrow keys while holding the shift key pressed. Use the\n"
"middle mouse button to paste text at the current mouse location.\n"
"Mice with scroll-ball are properly supported.\n\n"
"This are the currently defined key bindings:\n"
"Left Arrow\n"
"    Move the cursor one character leftwards.\n"
"    Scroll when cursor is out of frame.\n"
"Right Arrow\n"
"    Move the cursor one character rightwards.\n"
"    Scroll when cursor is out of frame.\n"
"Backspace\n"
"    Deletes the character on the left side of the text cursor and moves the\n"
"    cursor one position to the left. If a text has been marked by the user\n"
"    (e.g. by clicking and dragging) the cursor will be put at the beginning\n"
"    of the marked text and the marked text will be removed.\n"
"Home\n"
"    Moves the text cursor to the left end of the line. If mark is TRUE text\n"
"    will be marked towards the first position, if not any marked text will\n"
"    be unmarked if the cursor is moved.\n"
"End\n"
"    Moves the text cursor to the right end of the line. If mark is TRUE text\n"
"    will be marked towards the last position, if not any marked text will\n"
"    be unmarked if the cursor is moved.\n"
"Delete\n"
"    Deletes the character on the right side of the text cursor. If a text\n"
"    has been marked by the user (e.g. by clicking and dragging) the cursor\n"
"    will be put at the beginning of the marked text and the marked text will\n"
"    be removed.\n"
"Shift - Left Arrow\n"
"    Mark text one character leftwards.\n"
"Shift - Right Arrow\n"
"    Mark text one character rightwards.\n"
"Control-A\n"
"    Move the cursor to the beginning of the line.\n"
"Control-B\n"
"    Move the cursor one character leftwards.\n"
"Control-C\n"
"    Copy the marked text to the clipboard.\n"
"Control-D\n"
"    Delete the character to the right of the cursor.\n"
"Control-E\n"
"    Move the cursor to the end of the line.\n"
"Control-F\n"
"    Move the cursor one character rightwards.\n"
"Control-H\n"
"    Delete the character to the left of the cursor.\n"
"Control-K\n"
"    Delete marked text if any or delete all\n"
"    characters to the right of the cursor.\n"
"Control-U\n"
"    Delete all characters on the line.\n"
"Control-V\n"
"    Paste the clipboard text into line edit.\n"
"Control-X\n"
"    Cut the marked text, copy to clipboard.\n"
"Control-Y\n"
"    Paste the clipboard text into line edit.\n"
"\n"
"All other keys with valid ASCII codes insert themselves into the line.";

class TileFrame;


class TestMainFrame : public TGMainFrame {

private:
   TGCompositeFrame   *fStatusFrame;
   TGCanvas           *fCanvasWindow;
   TileFrame          *fContainer;
   TGTextEntry        *fTestText;
   TGButton           *fTestButton;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuFile, *fMenuTest, *fMenuHelp;
   TGPopupMenu        *fCascadeMenu, *fCascade1Menu, *fCascade2Menu;
   TGPopupMenu        *fMenuNew1, *fMenuNew2;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

public:
   TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~TestMainFrame();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
};


class TestDialog : public TGTransientFrame {

private:
   TGCompositeFrame    *fFrame1, *fF1, *fF2, *fF3, *fF4, *fF5, *fF6, *fF7;
   TGButton            *fOkButton, *fCancelButton, *fStartB, *fStopB;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2;
   TGPictureButton     *fPicBut1;
   TGRadioButton       *fRadio1, *fRadio2;
   TGCheckButton       *fCheck1;
   TGCheckButton       *fCheckMulti;
   TGListBox           *fListBox;
   TGComboBox          *fCombo;
   TGTab               *fTab;
   TGTextEntry         *fTxt1, *fTxt2;
   TGLayoutHints       *fL1, *fL2, *fL3, *fL4;
   TRootEmbeddedCanvas *fEc1, *fEc2;
   Int_t                fFirstEntry;
   Int_t                fLastEntry;
   Bool_t               fFillHistos;
   TH1F                *fHpx;
   TH2F                *fHpxpy;
   TList               *fCleanup;

   void FillHistos();

public:
   TestDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
               UInt_t options = kVerticalFrame);
   virtual ~TestDialog();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestMsgBox : public TGTransientFrame {

private:
   TGCompositeFrame     *f1, *f2, *f3, *f4, *f5;
   TGButton             *fTestButton, *fCloseButton;
   TGPictureButton      *fPictButton;
   TGRadioButton        *fR[4];
   TGCheckButton        *fC[9];
   TGGroupFrame         *fG1, *fG2;
   TGLayoutHints        *fL1, *fL2, *fL3, *fL4, *fL5, *fL6, *fL21;
   TGTextEntry          *fTitle, *fMsg;
   TGTextBuffer         *fTbtitle, *fTbmsg;
   TGLabel              *fLtitle, *fLmsg;
   TGGC                  fRedTextGC;

public:
   TestMsgBox(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
              UInt_t options = kVerticalFrame);
   virtual ~TestMsgBox();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestSliders : public TGTransientFrame {

private:
   TGVerticalFrame   *fVframe1, *fVframe2;
   TGLayoutHints     *fBly, *fBfly1;
   TGHSlider         *fHslider1, *fHslider2;
   TGVSlider         *fVslider1;
   TGDoubleVSlider   *fVslider2;
   TGTextEntry       *fTeh1, *fTev1, *fTeh2, *fTev2;
   TGTextBuffer      *fTbh1, *fTbv1, *fTbh2, *fTbv2;

public:
   TestSliders(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~TestSliders();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestShutter : public TGTransientFrame {

private:
   TGShutter       *fShutter;
   TGLayoutHints   *fLayout;
   const TGPicture *fDefaultPic;
   TList           *fTrash;

public:
   TestShutter(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   ~TestShutter();

   void AddShutterItem(const char *name, shutterData_t data[]);
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TestProgress : public TGTransientFrame {

private:
   TGHorizontalFrame *fHframe1;
   TGVerticalFrame   *fVframe1;
   TGLayoutHints     *fHint1, *fHint2, *fHint3, *fHint4, *fHint5;
   TGHProgressBar    *fHProg1, *fHProg2, *fHProg3;
   TGVProgressBar    *fVProg1, *fVProg2;
   TGTextButton      *fGO;

public:
   TestProgress(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~TestProgress();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class EntryTestDlg : public TGTransientFrame {

private:
   TGVerticalFrame      *fF1;
   TGVerticalFrame      *fF2;
   TGHorizontalFrame    *fF[13];
   TGLayoutHints        *fL1;
   TGLayoutHints        *fL2;
   TGLayoutHints        *fL3;
   TGLabel              *fLabel[13];
   TGNumberEntry        *fNumericEntries[13];
   TGCheckButton        *fLowerLimit;
   TGCheckButton        *fUpperLimit;
   TGNumberEntry        *fLimits[2];
   TGCheckButton        *fPositive;
   TGCheckButton        *fNonNegative;
   TGButton             *fSetButton;
   TGButton             *fExitButton;

   static const char *const numlabel[13];
   static const Double_t numinit[13];

public:
   EntryTestDlg(const TGWindow *p, const TGWindow *main);
   virtual ~EntryTestDlg();
   virtual void CloseWindow();

   void SetLimits();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
};


class Editor : public TGTransientFrame {

private:
   TGTextEdit       *fEdit;   // text edit widget
   TGTextButton     *fOK;     // OK button
   TGLayoutHints    *fL1;     // layout of TGTextEdit
   TGLayoutHints    *fL2;     // layout of OK button

public:
   Editor(const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~Editor();

   void   LoadBuffer(const char *buffer);
   void   LoadFile(const char *file);

   TGTextEdit *GetEditor() const { return fEdit; }

   void   SetTitle();
   void   Popup();
   void   CloseWindow();
   Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};


class TileFrame : public TGCompositeFrame {

private:
   TGCanvas *fCanvas;

public:
   TileFrame(const TGWindow *p);
   virtual ~TileFrame() { }

   void SetCanvas(TGCanvas *canvas) { fCanvas = canvas; }
   Bool_t HandleButton(Event_t *event);
};

TileFrame::TileFrame(const TGWindow *p) :
   TGCompositeFrame(p, 10, 10, kHorizontalFrame, GetWhitePixel())
{
   // Create tile view container. Used to show colormap.

   fCanvas = 0;
   SetLayoutManager(new TGTileLayout(this, 8));

   // Handle only buttons 4 and 5 used by the wheel mouse to scroll
   gVirtualX->GrabButton(fId, kButton4, kAnyModifier,
                         kButtonPressMask | kButtonReleaseMask,
                         kNone, kNone);
   gVirtualX->GrabButton(fId, kButton5, kAnyModifier,
                         kButtonPressMask | kButtonReleaseMask,
                         kNone, kNone);
}

Bool_t TileFrame::HandleButton(Event_t *event)
{
   // Handle wheel mouse to scroll.

   Int_t page = 0;
   if (event->fCode == kButton4 || event->fCode == kButton5) {
      if (!fCanvas) return kTRUE;
      if (fCanvas->GetContainer()->GetHeight())
         page = Int_t(Float_t(fCanvas->GetViewPort()->GetHeight() *
                              fCanvas->GetViewPort()->GetHeight()) /
                              fCanvas->GetContainer()->GetHeight());
   }

   if (event->fCode == kButton4) {
      //scroll up
      Int_t newpos = fCanvas->GetVsbPosition() - page;
      if (newpos < 0) newpos = 0;
      fCanvas->SetVsbPosition(newpos);
      return kTRUE;
   }
   if (event->fCode == kButton5) {
      // scroll down
      Int_t newpos = fCanvas->GetVsbPosition() + page;
      fCanvas->SetVsbPosition(newpos);
      return kTRUE;
   }
   return kTRUE;
}


TestMainFrame::TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{
   // Create test main frame. A TGMainFrame is a top level window.

   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                                      0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Save", M_FILE_SAVE);
   fMenuFile->AddEntry("S&ave as...", M_FILE_SAVEAS);
   fMenuFile->AddEntry("&Close", -1);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Print", M_FILE_PRINT);
   fMenuFile->AddEntry("P&rint setup...", M_FILE_PRINTSETUP);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

   fMenuFile->DisableEntry(M_FILE_SAVEAS);
   fMenuFile->HideEntry(M_FILE_PRINT);

   fCascade2Menu = new TGPopupMenu(fClient->GetRoot());
   fCascade2Menu->AddEntry("ID = 2&1", M_CASCADE_1);
   fCascade2Menu->AddEntry("ID = 2&2", M_CASCADE_2);
   fCascade2Menu->AddEntry("ID = 2&3", M_CASCADE_3);

   fCascade1Menu = new TGPopupMenu(fClient->GetRoot());
   fCascade1Menu->AddEntry("ID = 4&1", 41);
   fCascade1Menu->AddEntry("ID = 4&2", 42);
   fCascade1Menu->AddEntry("ID = 4&3", 43);
   fCascade1Menu->AddSeparator();
   fCascade1Menu->AddPopup("Cascade&d 2", fCascade2Menu);

   fCascadeMenu = new TGPopupMenu(fClient->GetRoot());
   fCascadeMenu->AddEntry("ID = 5&1", 51);
   fCascadeMenu->AddEntry("ID = 5&2", 52);
   fCascadeMenu->AddEntry("ID = 5&3", 53);
   fCascadeMenu->AddSeparator();
   fCascadeMenu->AddPopup("&Cascaded 1", fCascade1Menu);
   fCascadeMenu->AddPopup("C&ascaded 2", fCascade2Menu);

   fMenuTest = new TGPopupMenu(fClient->GetRoot());
   fMenuTest->AddLabel("Test different features...");
   fMenuTest->AddSeparator();
   fMenuTest->AddEntry("&Dialog...", M_TEST_DLG);
   fMenuTest->AddEntry("&Message Box...", M_TEST_MSGBOX);
   fMenuTest->AddEntry("&Sliders...", M_TEST_SLIDER);
   fMenuTest->AddEntry("Sh&utter...", M_TEST_SHUTTER);
   fMenuTest->AddEntry("&Progress...", M_TEST_PROGRESS);
   fMenuTest->AddEntry("&Number Entry...", M_TEST_NUMBERENTRY);
   fMenuTest->AddSeparator();
   fMenuTest->AddEntry("Add New Menus", M_TEST_NEWMENU);
   fMenuTest->AddSeparator();
   fMenuTest->AddPopup("&Cascaded menus", fCascadeMenu);

   fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp->AddEntry("&Contents", M_HELP_CONTENTS);
   fMenuHelp->AddEntry("&Search...", M_HELP_SEARCH);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   fMenuNew1 = new TGPopupMenu();
   fMenuNew1->AddEntry("Remove New Menus", M_NEW_REMOVEMENU);

   fMenuNew2 = new TGPopupMenu();
   fMenuNew2->AddEntry("Remove New Menus", M_NEW_REMOVEMENU);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuTest->Associate(this);
   fMenuHelp->Associate(this);
   fCascadeMenu->Associate(this);
   fCascade1Menu->Associate(this);
   fCascade2Menu->Associate(this);
   fMenuNew1->Associate(this);
   fMenuNew2->Associate(this);

   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Test", fMenuTest, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

   // Create TGCanvas and a canvas container which uses a tile layout manager
   fCanvasWindow = new TGCanvas(this, 400, 240);
   fContainer = new TileFrame(fCanvasWindow->GetViewPort());
   fContainer->SetCanvas(fCanvasWindow);
   fCanvasWindow->SetContainer(fContainer);

   // Fill canvas with 256 colored frames (notice that doing it this way
   // causes a memory leak when deleting the canvas (i.e. the TGFrame's and
   // TGLayoutHints are NOT adopted by the canvas).
   // Best is to create a TList and store all frame and layout pointers
   // in it. When deleting the canvas just delete contents of list.
   for (int i=0; i < 256; ++i)
      fCanvasWindow->AddFrame(new TGFrame(fCanvasWindow->GetContainer(),
                                          32, 32, 0, (i+1)&255),
                              new TGLayoutHints(kLHintsExpandY | kLHintsRight));

   AddFrame(fCanvasWindow, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
                                             0, 0, 2, 2));

   // Create status frame containing a button and a text entry widget
   fStatusFrame = new TGCompositeFrame(this, 60, 20, kHorizontalFrame |
                                                     kSunkenFrame);

   fTestButton = new TGTextButton(fStatusFrame, "&Open editor...", 150);
   fTestButton->Associate(this);
   fTestButton->SetToolTipText("Pops up editor");
   fStatusFrame->AddFrame(fTestButton, new TGLayoutHints(kLHintsTop |
                          kLHintsLeft, 2, 0, 2, 2));
   fTestText = new TGTextEntry(fStatusFrame, new TGTextBuffer(100));
   fTestText->SetToolTipText("This is a text entry widget");
   fTestText->Resize(300, fTestText->GetDefaultHeight());
   fStatusFrame->AddFrame(fTestText, new TGLayoutHints(kLHintsTop | kLHintsLeft,
                                                       10, 2, 2, 2));
   AddFrame(fStatusFrame, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
            0, 0, 1, 0));

   SetWindowName("GuiTest");

   MapSubwindows();

   // we need to use GetDefault...() to initialize the layout algorithm...
   Resize(GetDefaultSize());
   //Resize(400, 200);

   MapWindow();
}

TestMainFrame::~TestMainFrame()
{
   // Delete all created widgets.

   delete fTestButton;
   delete fTestText;

   delete fStatusFrame;
   delete fContainer;
   delete fCanvasWindow;

   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;

   delete fMenuBar;
   delete fMenuFile;
   delete fMenuTest;
   delete fMenuHelp;
   delete fCascadeMenu;
   delete fCascade1Menu;
   delete fCascade2Menu;
   delete fMenuNew1;
   delete fMenuNew2;
}

void TestMainFrame::CloseWindow()
{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).

   gApplication->Terminate(0);
}

Bool_t TestMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   switch (GET_MSG(msg)) {

      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
               //printf("Button was pressed, id = %ld\n", parm1);
               if (parm1 == 150) {
                  Editor *ed = new Editor(this, 600, 400);
                  ed->LoadBuffer(editortxt);
                  ed->Popup();
               }
              break;

            case kCM_MENUSELECT:
               //printf("Pointer over menu entry, id=%ld\n", parm1);
               break;

            case kCM_MENU:
               switch (parm1) {

                  case M_FILE_OPEN:
                     {
                        static TString dir(".");
                        TGFileInfo fi;
                        fi.fFileTypes = filetypes;
                        fi.fIniDir    = StrDup(dir);
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
                        printf("Open file: %s (dir: %s)\n", fi.fFilename,
                               fi.fIniDir);
                        dir = fi.fIniDir;
                     }
                     break;

                  case M_FILE_SAVE:
                     printf("M_FILE_SAVE\n");
                     break;

                  case M_FILE_PRINT:
                     printf("M_FILE_PRINT\n");
                     printf("Hiding itself, select \"Print Setup...\" to enable again\n");
                     fMenuFile->HideEntry(M_FILE_PRINT);
                     break;

                  case M_FILE_PRINTSETUP:
                     printf("M_FILE_PRINTSETUP\n");
                     printf("Enabling \"Print\"\n");
                     fMenuFile->EnableEntry(M_FILE_PRINT);
                     break;

                  case M_FILE_EXIT:
                     CloseWindow();   // this also terminates theApp
                     break;

                  case M_TEST_DLG:
                     new TestDialog(fClient->GetRoot(), this, 400, 200);
                     break;

                  case M_TEST_MSGBOX:
                     new TestMsgBox(fClient->GetRoot(), this, 400, 200);
                     break;

                  case M_TEST_SLIDER:
                     new TestSliders(fClient->GetRoot(), this, 400, 200);
                     break;

                  case M_TEST_SHUTTER:
                     new TestShutter(fClient->GetRoot(), this, 400, 200);
                     break;

                  case M_TEST_PROGRESS:
                     new TestProgress(fClient->GetRoot(), this, 600, 300);
                     break;

                  case M_TEST_NUMBERENTRY:
                     new EntryTestDlg(fClient->GetRoot(), this);
                     break;

                  case M_TEST_NEWMENU:
                     {
                        if (fMenuTest->IsEntryChecked(M_TEST_NEWMENU)) {
                           ProcessMessage(MK_MSG(kC_COMMAND, kCM_MENU),
                                          M_NEW_REMOVEMENU, 0);
                           return kTRUE;
                        }
                        fMenuTest->CheckEntry(M_TEST_NEWMENU);
                        TGPopupMenu *p = fMenuBar->GetPopup("Test");
                        fMenuBar->AddPopup("New 1", fMenuNew1, fMenuBarItemLayout, p);
                        p = fMenuBar->GetPopup("Help");
                        fMenuBar->AddPopup("New 2", fMenuNew2, fMenuBarItemLayout, p);
                        fMenuBar->MapSubwindows();
                        fMenuBar->Layout();

                        TGMenuEntry *e = fMenuTest->GetEntry("Add New Menus");
                        fMenuTest->AddEntry("Remove New Menus", M_NEW_REMOVEMENU, 0, 0, e);
                     }
                     break;

                  case M_NEW_REMOVEMENU:
                     {
                        fMenuBar->RemovePopup("New 1");
                        fMenuBar->RemovePopup("New 2");
                        fMenuBar->Layout();
                        fMenuTest->DeleteEntry(M_NEW_REMOVEMENU);
                        fMenuTest->UnCheckEntry(M_TEST_NEWMENU);
                     }
                     break;

                  default:
                     break;
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}

TestDialog::TestDialog(const TGWindow *p, const TGWindow *main, UInt_t w,
                       UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   // Used to store GUI elements that need to be deleted in the ctor.
   fCleanup = new TList;

   fFrame1 = new TGHorizontalFrame(this, 60, 20, kFixedWidth);

   fOkButton = new TGTextButton(fFrame1, "&Ok", 1);
   fOkButton->Associate(this);
   fCancelButton = new TGTextButton(fFrame1, "&Cancel", 2);
   fCancelButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           2, 2, 2, 2);
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   fFrame1->AddFrame(fOkButton, fL1);
   fFrame1->AddFrame(fCancelButton, fL1);

   fFrame1->Resize(150, fOkButton->GetDefaultHeight());
   AddFrame(fFrame1, fL2);

   //--------- create Tab widget and some composite frames for Tab testing

   fTab = new TGTab(this, 300, 300);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);

   TGCompositeFrame *tf = fTab->AddTab("Tab 1");
   fF1 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF1->AddFrame(new TGTextButton(fF1, "&Test button", 0), fL3);
   fF1->AddFrame(fTxt1 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);
   fF1->AddFrame(fTxt2 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);
   tf->AddFrame(fF1, fL3);
   fTxt1->Resize(150, fTxt1->GetDefaultHeight());
   fTxt2->Resize(150, fTxt2->GetDefaultHeight());

   tf = fTab->AddTab("Tab 2");
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           200, 2, 2, 2);
   fF2 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF2->AddFrame(fBtn1 = new TGTextButton(fF2, "&Button 1", 0), fL1);
   fF2->AddFrame(fBtn2 = new TGTextButton(fF2, "B&utton 2", 0), fL1);
   fF2->AddFrame(fChk1 = new TGCheckButton(fF2, "C&heck 1", 0), fL1);
   fF2->AddFrame(fChk2 = new TGCheckButton(fF2, "Chec&k 2", 0), fL1);
   fF2->AddFrame(fRad1 = new TGRadioButton(fF2, "&Radio 1", 81), fL1);
   fF2->AddFrame(fRad2 = new TGRadioButton(fF2, "R&adio 2", 82), fL1);
   fCombo = new TGComboBox(fF2, 88);
   fF2->AddFrame(fCombo, fL3);

   tf->AddFrame(fF2, fL3);

   int i;
   for (i = 0; i < 20; i++) {
      char tmp[20];

      sprintf(tmp, "Entry %i", i+1);
      fCombo->AddEntry(tmp, i+1);
   }

   fCombo->Resize(150, 20);

   fBtn1->Associate(this);
   fBtn2->Associate(this);
   fChk1->Associate(this);
   fChk2->Associate(this);
   fRad1->Associate(this);
   fRad2->Associate(this);

   //-------------- embedded canvas demo
   fFillHistos = kFALSE;
   fHpx   = 0;
   fHpxpy = 0;

   tf = fTab->AddTab("Tab 3");
   fF3 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fStartB = new TGTextButton(fF3, "Start &Filling Hists", 40);
   fStopB  = new TGTextButton(fF3, "&Stop Filling Hists", 41);
   fStartB->Associate(this);
   fStopB->Associate(this);
   fF3->AddFrame(fStartB, fL3);
   fF3->AddFrame(fStopB, fL3);

   fF5 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);

   fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
                           kLHintsExpandY, 5, 5, 5, 5);
   fEc1 = new TRootEmbeddedCanvas("ec1", fF5, 100, 100);
   fF5->AddFrame(fEc1, fL4);
   fEc2 = new TRootEmbeddedCanvas("ec2", fF5, 100, 100);
   fF5->AddFrame(fEc2, fL4);

   tf->AddFrame(fF3, fL3);
   tf->AddFrame(fF5, fL4);

   fEc1->GetCanvas()->SetBorderMode(0);
   fEc2->GetCanvas()->SetBorderMode(0);

   // make tab yellow
   ULong_t yellow;
   fClient->GetColorByName("yellow", yellow);
   TGTabElement *tabel = fTab->GetTabTab(2);
   tabel->ChangeBackground(yellow);

   //-------------- end embedded canvas demo

   TGTextButton *bt;
   tf = fTab->AddTab("Tab 4");
   fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF4->AddFrame(bt = new TGTextButton(fF4, "A&dd Entry", 90), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);
   fF4->AddFrame(bt = new TGTextButton(fF4, "Remove &Entry", 91), fL3);
   bt->Associate(this);
   fCleanup->Add(bt);
   fF4->AddFrame(fListBox = new TGListBox(fF4, 89), fL3);
   fF4->AddFrame(fCheckMulti = new TGCheckButton(fF4, "&Mutli Selectable", 92), fL3);
   fCheckMulti->Associate(this);
   tf->AddFrame(fF4, fL3);

   for (i=0; i < 20; ++i) {
      char tmp[20];

      sprintf(tmp, "Entry %i", i+1);
      fListBox->AddEntry(tmp, i+1);
   }
   fFirstEntry = 1;
   fLastEntry  = 20;

   fListBox->Resize(150, 80);

   //--- tab 5
   tf = fTab->AddTab("Tab 5");
   tf->SetLayoutManager(new TGHorizontalLayout(tf));

   fF6 = new TGGroupFrame(tf, "Options", kVerticalFrame);
   tf->AddFrame(fF6, fL3);

   // 2 column, n rows
   fF6->SetLayoutManager(new TGMatrixLayout(fF6, 0, 2, 10));
   char buff[100];
   int j;
   for (j = 0; j < 4; j++) {
      sprintf(buff, "Module %i", j+1);
      fF6->AddFrame(new TGLabel(fF6, new TGHotString(buff)));

      TGTextBuffer *tbuf = new TGTextBuffer(10);
      tbuf->AddText(0, "0.0");

      TGTextEntry  *tent = new TGTextEntry(fF6, tbuf);
      tent->Resize(50, tent->GetDefaultHeight());
      tent->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
      fCleanup->Add(tent);
      fF6->AddFrame(tent);
   }
   fF6->Resize(fF6->GetDefaultSize());

   // another matrix with text and buttons
   fF7 = new TGGroupFrame(tf, "Tab Handling", kVerticalFrame);
   tf->AddFrame(fF7, fL3);

   fF7->SetLayoutManager(new TGMatrixLayout(fF7, 0, 1, 10));

   fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab", 101));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->AddFrame(bt = new TGTextButton(fF7, "Add Tab", 103));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab 5", 102));
   bt->Associate(this);
   bt->Resize(90, bt->GetDefaultHeight());
   fCleanup->Add(bt);

   fF7->Resize(fF6->GetDefaultSize());

   //--- end of last tab

   TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL5);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Dialog");

   MapWindow();
   //fClient->WaitFor(this);    // otherwise canvas contextmenu does not work
}

TestDialog::~TestDialog()
{
   // Delete test dialog widgets.

   fCleanup->Delete();
   delete fCleanup;
   delete fOkButton;
   delete fCancelButton;
   delete fStartB; delete fStopB;
   delete fFrame1;
   delete fBtn1; delete fBtn2;
   delete fChk1; delete fChk2;
   delete fRad1; delete fRad2;
   delete fTxt1; delete fTxt2;
   delete fEc1; delete fEc2;
   delete fListBox; delete fCheckMulti;
   delete fF1; delete fF2; delete fF3; delete fF4; delete fF5;
   delete fF6; delete fF7;
   delete fCombo;
   delete fTab;
   delete fL3; delete fL4;
   delete fL1; delete fL2;
   delete fHpx; delete fHpxpy;
}

void TestDialog::FillHistos()
{
   // Fill histograms till user clicks "Stop Filling" button.

   static int cnt;

   if (!fHpx) {
      fHpx   = new TH1F("hpx","This is the px distribution",100,-4,4);
      fHpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
      fHpx->SetFillColor(kRed);
      cnt = 0;
   }

   const int kUPDATE = 1000;
   float px, py;
   TCanvas *c1 = fEc1->GetCanvas();
   TCanvas *c2 = fEc2->GetCanvas();

   while (fFillHistos) {
      gRandom->Rannor(px,py); //px and py will be two gaussian random numbers
      fHpx->Fill(px);
      fHpxpy->Fill(px,py);
      cnt++;
      if (!(cnt % kUPDATE)) {
         if (cnt == kUPDATE) {
            c1->cd();
            fHpx->Draw();
            c2->cd();
            fHpxpy->Draw("cont");
         }
         c1->Modified();
         c1->Update();
         c2->Modified();
         c2->Update();
         gSystem->ProcessEvents();  // handle GUI events
      }
   }
}

void TestDialog::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t TestDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages coming from widgets associated with the dialog.

   char tmp[20];
   static int newtab = 0;

   switch (GET_MSG(msg)) {
      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch(parm1) {
                  case 1:
                  case 2:
                     printf("\nTerminating dialog: %s pressed\n",
                            (parm1 == 1) ? "OK" : "Cancel");
                     CloseWindow();
                     break;
                  case 40:  // start histogram filling
                     fFillHistos = kTRUE;
                     FillHistos();
                     break;
                  case 41:  // stop histogram filling
                     fFillHistos = kFALSE;
                     break;
                  case 90:  // add one entry in list box
                     fLastEntry++;
                     sprintf(tmp, "Entry %i", fLastEntry);
                     fListBox->AddEntry(tmp, fLastEntry);
                     fListBox->MapSubwindows();
                     fListBox->Layout();
                     break;
                  case 91:  // remove one entry in list box
                     if (fFirstEntry < fLastEntry) {
                        fListBox->RemoveEntry(fFirstEntry);
                        fListBox->Layout();
                        fFirstEntry++;
                     }
                     break;
                  case 101:  // remove tabs
                     {
                        TString s = fTab->GetTabTab(0)->GetString();
                        if (s == "Tab 3") {
                           // Need to delete the embedded canvases
                           // since RemoveTab() will Destroy the container
                           // window, which in turn will destroy the embedded
                           // canvas windows.
                           SafeDelete(fEc1);
                           SafeDelete(fEc2);
                        }
                        fTab->RemoveTab(0);
                        fTab->Layout();
                     }
                     break;
                  case 102:  // remove tab 5
                     {
                        int nt = fTab->GetNumberOfTabs();
                        for (int i = 0 ; i < nt; i++) {
                           TString s = fTab->GetTabTab(i)->GetString();
                           if (s == "Tab 5") {
                              fTab->RemoveTab(i);
                              fTab->Layout();
                           }
                        }
                     }
                     break;
                  case 103:  // add tabs
                     sprintf(tmp, "New Tab %d", ++newtab);
                     fTab->AddTab(tmp);
                     fTab->MapSubwindows();
                     fTab->Layout();
                     break;
                  default:
                     break;
               }
               break;
            case kCM_RADIOBUTTON:
               switch (parm1) {
                  case 81:
                     fRad2->SetState(kButtonUp);
                     break;
                  case 82:
                     fRad1->SetState(kButtonUp);
                     break;
               }
               break;
            case kCM_CHECKBUTTON:
               switch (parm1) {
                  case 92:
                     fListBox->SetMultipleSelections(fCheckMulti->GetState());
                     break;
                  default:
                     break;
               }
               break;
            case kCM_TAB:
               printf("Tab item %ld activated\n", parm1);
               break;
            default:
               break;
         }
         break;

      default:
         break;
   }
   return kTRUE;
}

TestMsgBox::TestMsgBox(const TGWindow *p, const TGWindow *main,
                       UInt_t w, UInt_t h, UInt_t options) :
     TGTransientFrame(p, main, w, h, options),
     fRedTextGC(TGButton::GetDefaultGC())
{
   // Create message box test dialog. Use this dialog to select the different
   // message dialog box styles and show the message dialog by clicking the
   // "Test" button.

   //------------------------------
   // Set foreground color in graphics context for drawing of
   // TGlabel and TGButtons with text in red.

   ULong_t red;
   fClient->GetColorByName("red", red);
   fRedTextGC.SetForeground(red);
   //---------------------------------

   int i, ax, ay;

   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   f1 = new TGCompositeFrame(this, 60, 20, kVerticalFrame | kFixedWidth);
   f2 = new TGCompositeFrame(this, 60, 20, kVerticalFrame);
   f3 = new TGCompositeFrame(f2, 60, 20, kHorizontalFrame);
   f4 = new TGCompositeFrame(f2, 60, 20, kHorizontalFrame);
   f5 = new TGCompositeFrame(f2, 60, 20, kHorizontalFrame);

   fTestButton = new TGTextButton(f1, "&Test", 1, fRedTextGC());

   // Change background of fTestButton to green
   ULong_t green;
   fClient->GetColorByName("green", green);
   fTestButton->ChangeBackground(green);

   fCloseButton = new TGTextButton(f1, "&Close", 2);
   fPictButton = new TGPictureButton(f1, fClient->GetPicture("mb_stop_s.xpm"));

   f1->Resize(fTestButton->GetDefaultWidth()+40, GetDefaultHeight());

   fTestButton->Associate(this);
   fCloseButton->Associate(this);
   fPictButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX,
                           2, 2, 3, 0);
   fL2 = new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX,
                           2, 5, 0, 2);
   fL21 = new TGLayoutHints(kLHintsTop | kLHintsRight,
                            2, 5, 10, 0);

   f1->AddFrame(fTestButton, fL1);
   f1->AddFrame(fCloseButton, fL1);
   f1->AddFrame(fPictButton, fL1);
   AddFrame(f1, fL21);

   //--------- create check and radio buttons groups

   fG1 = new TGGroupFrame(f3, new TGString("Buttons"));
   fG2 = new TGGroupFrame(f3, new TGString("Icons"));

   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft |
                           kLHintsExpandX | kLHintsExpandY,
                           2, 2, 2, 2);
   fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft,
                           0, 0, 5, 0);

   fC[0] = new TGCheckButton(fG1, new TGHotString("Yes"),     -1);
   fC[1] = new TGCheckButton(fG1, new TGHotString("No"),      -1);
   fC[2] = new TGCheckButton(fG1, new TGHotString("OK"),      -1);
   fC[3] = new TGCheckButton(fG1, new TGHotString("Apply"),   -1);
   fC[4] = new TGCheckButton(fG1, new TGHotString("Retry"),   -1);
   fC[5] = new TGCheckButton(fG1, new TGHotString("Ignore"),  -1);
   fC[6] = new TGCheckButton(fG1, new TGHotString("Cancel"),  -1);
   fC[7] = new TGCheckButton(fG1, new TGHotString("Close"),   -1);
   fC[8] = new TGCheckButton(fG1, new TGHotString("Dismiss"), -1);

   for (i=0; i<9; ++i) fG1->AddFrame(fC[i], fL4);

   fR[0] = new TGRadioButton(fG2, new TGHotString("Stop"),        21);
   fR[1] = new TGRadioButton(fG2, new TGHotString("Question"),    22);
   fR[2] = new TGRadioButton(fG2, new TGHotString("Exclamation"), 23);
   fR[3] = new TGRadioButton(fG2, new TGHotString("Asterisk"),    24);

   for (i = 0; i < 4; ++i) {
      fG2->AddFrame(fR[i], fL4);
      fR[i]->Associate(this);
   }

   fC[2]->SetState(kButtonDown);
   fR[0]->SetState(kButtonDown);

   f3->AddFrame(fG1, fL3);
   f3->AddFrame(fG2, fL3);

   fLtitle = new TGLabel(f4, new TGString("Title:"), fRedTextGC());
   fLmsg   = new TGLabel(f5, new TGString("Message:"));

   fTitle = new TGTextEntry(f4, fTbtitle = new TGTextBuffer(100));
   fMsg   = new TGTextEntry(f5, fTbmsg = new TGTextBuffer(100));

   fTbtitle->AddText(0, "MsgBox");
   fTbmsg->AddText(0, "This is a test message box.");

   fTitle->Resize(300, fTitle->GetDefaultHeight());
   fMsg->Resize(300, fMsg->GetDefaultHeight());

   fL5 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                           3, 5, 0, 0);
   fL6 = new TGLayoutHints(kLHintsRight | kLHintsCenterY,
                           0, 2, 0, 0);

   f4->AddFrame(fLtitle, fL5);
   f4->AddFrame(fTitle, fL6);
   f5->AddFrame(fLmsg, fL5);
   f5->AddFrame(fMsg, fL6);

   f2->AddFrame(f3, fL1);
   f2->AddFrame(f4, fL1);
   f2->AddFrame(f5, fL1);

   AddFrame(f2, fL2);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Message Box Test");

   MapWindow();
   fClient->WaitFor(this);
}

// Order is important when deleting frames. Delete children first,
// parents last.

TestMsgBox::~TestMsgBox()
{
   // Delete widgets created by dialog.

   delete fTestButton; delete fCloseButton;
   delete fC[0]; delete fC[1]; delete fC[2]; delete fC[3];
   delete fC[4]; delete fC[5]; delete fC[6]; delete fC[7]; delete fC[8];
   delete fR[0]; delete fR[1]; delete fR[2]; delete fR[3];
   delete fTitle; delete fLtitle;
   delete fMsg; delete fLmsg;
   delete f3; delete f4; delete f5; delete f2; delete f1;
   delete fL1; delete fL2; delete fL3; delete fL4; delete fL5; delete fL6;
   delete fL21;
}

void TestMsgBox::CloseWindow()
{
   // Close dialog in response to window manager close.

   delete this;
}

Bool_t TestMsgBox::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages sent to this dialog.

   int i, buttons, retval;
   EMsgBoxIcon icontype = kMBIconStop;

   switch(GET_MSG(msg)) {
      case kC_COMMAND:

         switch(GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch(parm1) {
                  case 1:
                     buttons = 0;
                     for (i=0; i<9; ++i)
                        if (fC[i]->GetState() == kButtonDown)
                           buttons |= mb_button_id[i];

                     for (i=0; i<4; ++i)
                        if (fR[i]->GetState() == kButtonDown)
                           { icontype = mb_icon[i]; break; }

                     new TGMsgBox(fClient->GetRoot(), this,
                                  fTbtitle->GetString(), fTbmsg->GetString(),
                                  icontype, buttons, &retval);
                     break;

                  case 2:
                     CloseWindow();
                     break;

               }
               break;

            case kCM_RADIOBUTTON:
            case kCM_CHECKBUTTON:
               if (parm1 >= 21 && parm1 <= 24) {
                  for (i=0; i<4; ++i)
                     if (fR[i]->WidgetId() != parm1)
                        fR[i]->SetState(kButtonUp);
               }
               break;

            default:
               break;
         }
         break;

      default:
         break;
   }

   return kTRUE;
}

TestSliders::TestSliders(const TGWindow *p, const TGWindow *main,
                         UInt_t w, UInt_t h) :
    TGTransientFrame(p, main, w, h)
{
   // Dialog used to test the different supported sliders.

   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   fVframe1 = new TGVerticalFrame(this, 0, 0, 0);

   fTeh1 = new TGTextEntry(fVframe1, fTbh1 = new TGTextBuffer(10), HId1);
   fTev1 = new TGTextEntry(fVframe1, fTbv1 = new TGTextBuffer(10), VId1);
   fTbh1->AddText(0, "0");
   fTbv1->AddText(0, "0");

   fTeh1->Associate(this);
   fTev1->Associate(this);

   fHslider1 = new TGHSlider(fVframe1, 100, kSlider1 | kScaleBoth, HSId1);
   fHslider1->Associate(this);
   fHslider1->SetRange(0,50);

   fVslider1 = new TGVSlider(fVframe1, 100, kSlider2 | kScaleBoth, VSId1);
   fVslider1->Associate(this);
   fVslider1->SetRange(0,8);

   fVframe1->Resize(100, 100);

   fVframe2 = new TGVerticalFrame(this, 0, 0, 0);
   fTeh2 = new TGTextEntry(fVframe2, fTbh2 = new TGTextBuffer(10), HId2);
   fTev2 = new TGTextEntry(fVframe2, fTbv2 = new TGTextBuffer(10), VId2);
   fTbh2->AddText(0, "0");
   fTbv2->AddText(0, "0");

   fTeh2->Associate(this);
   fTev2->Associate(this);

   fHslider2 = new TGHSlider(fVframe2, 150, kSlider2 | kScaleBoth, HSId2);
   fHslider2->Associate(this);
   fHslider2->SetRange(0,3);

   fVslider2 = new TGDoubleVSlider(fVframe2, 100, kDoubleScaleBoth, VSId2);
   fVslider2->Associate(this);
   fVslider2->SetRange(-10,10);

   fVframe2->Resize(100, 100);

   //--- layout for buttons: top align, equally expand horizontally
   fBly = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 3, 0);

   //--- layout for the frame: place at bottom, right aligned
   fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsRight, 20, 10, 15, 0);

   fVframe1->AddFrame(fHslider1, fBly);
   fVframe1->AddFrame(fVslider1, fBly);
   fVframe1->AddFrame(fTeh1, fBly);
   fVframe1->AddFrame(fTev1, fBly);

   fVframe2->AddFrame(fHslider2, fBly);
   fVframe2->AddFrame(fVslider2, fBly);
   fVframe2->AddFrame(fTeh2, fBly);
   fVframe2->AddFrame(fTev2, fBly);

   AddFrame(fVframe2, fBfly1);
   AddFrame(fVframe1, fBfly1);

   SetWindowName("Slider Test");
   TGDimension size = GetDefaultSize();
   Resize(size);

   SetWMSize(size.fWidth, size.fHeight);
   SetWMSizeHints(size.fWidth, size.fHeight, size.fWidth, size.fHeight, 0, 0);
   SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                              kMWMDecorMinimize | kMWMDecorMenu,
               kMWMFuncAll |  kMWMFuncResize    | kMWMFuncMaximize |
                              kMWMFuncMinimize,
               kMWMInputModeless);

   // position relative to the parent's window
   Window_t wdummy;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdummy);
   Move(ax, ay);

   MapSubwindows();
   MapWindow();

   fClient->WaitFor(this);
}

TestSliders::~TestSliders()
{
   // Delete dialog.

   delete fVframe1;
   delete fVframe2;
   delete fBfly1; delete fBly;
   delete fHslider1; delete fHslider2;
   delete fVslider1; delete fVslider2;
   delete fTeh1; delete fTeh2; delete fTev1; delete fTev2;
}

void TestSliders::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t TestSliders::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Process slider messages.

   char buf[10];

   switch (GET_MSG(msg)) {
      case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
            case kTE_TEXTCHANGED:
               switch (parm1) {
                  case HId1:
                     fHslider1->SetPosition(atoi(fTbh1->GetString()));
                     break;
                  case VId1:
                     fVslider1->SetPosition(atoi(fTbv1->GetString()));
                     break;
                  case HId2:
                     fHslider2->SetPosition(atoi(fTbh2->GetString()));
                     break;
                  case VId2:
                     fVslider2->SetPosition(atoi(fTbv2->GetString()),
                                            atoi(fTbv2->GetString())+2);
                     break;
               }
               break;
         }
         break;
      case kC_VSLIDER:
      case kC_HSLIDER:
         switch (GET_SUBMSG(msg)) {
            case kSL_POS:
               sprintf(buf, "%ld", parm2);
               switch (parm1) {
                  case HSId1:
                     fTbh1->Clear();
                     fTbh1->AddText(0, buf);
                     fClient->NeedRedraw(fTeh1);
                     break;
                  case VSId1:
                     fTbv1->Clear();
                     fTbv1->AddText(0, buf);
                     fClient->NeedRedraw(fTev1);
                     break;
                  case HSId2:
                     fTbh2->Clear();
                     fTbh2->AddText(0, buf);
                     fClient->NeedRedraw(fTeh2);
                     break;
                  case VSId2:
                     sprintf(buf, "%f", fVslider2->GetMinPosition());
                     fTbv2->Clear();
                     fTbv2->AddText(0, buf);
                     fClient->NeedRedraw(fTev2);
                     break;
               }
               break;
         }
         break;
      default:
         break;
   }
   return kTRUE;
}

TestShutter::TestShutter(const TGWindow *p, const TGWindow *main,
                         UInt_t w, UInt_t h) :
   TGTransientFrame(p, main, w, h)
{
   // Create transient frame containing a shutter widget.

   fDefaultPic = fClient->GetPicture("folder_s.xpm");
   fShutter = new TGShutter(this, kSunkenFrame);

   fTrash = new TList;

   AddShutterItem("Histograms", histo_data);
   AddShutterItem("Functions", function_data);
   AddShutterItem("Trees", tree_data);

   fLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   AddFrame(fShutter, fLayout);

   MapSubwindows();
   Resize(80, 300);

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Shutter Test");

   MapWindow();
   //fClient->WaitFor(this);
}

void TestShutter::AddShutterItem(const char *name, shutterData_t data[])
{
   TGShutterItem    *item;
   TGCompositeFrame *container;
   TGButton         *button;
   const TGPicture  *buttonpic;
   static int id = 5001;

   TGLayoutHints *l = new TGLayoutHints(kLHintsTop | kLHintsCenterX,
                                        5, 5, 5, 0);
   fTrash->Add(l);

   item = new TGShutterItem(fShutter, new TGHotString(name), id++);
   container = (TGCompositeFrame *) item->GetContainer();
   fTrash->Add(item);

   for (int i=0; data[i].pixmap_name != 0; i++) {
      buttonpic = fClient->GetPicture(data[i].pixmap_name);
      if (!buttonpic) {
         Warning("AddShutterItem", "missing pixmap \"%s\", using default",
                 data[i].pixmap_name);
         buttonpic = fDefaultPic;
      }

      button = new TGPictureButton(container, buttonpic, data[i].id);
      fTrash->Add(button);
      container->AddFrame(button, l);
      button->Associate(this);
      button->SetToolTipText(data[i].tip_text);
      data[i].button = button;
   }

   fShutter->AddItem(item);
}

TestShutter::~TestShutter()
{
   delete fLayout;
   delete fShutter;
   fTrash->Delete();
   delete fTrash;
}

void TestShutter::CloseWindow()
{
   delete this;
}

Bool_t TestShutter::ProcessMessage(Long_t, Long_t parm1, Long_t)
{
   // Process messages sent to this dialog.

   printf("Shutter button %d\n", (Int_t)parm1);
   return kTRUE;
}

TestProgress::TestProgress(const TGWindow *p, const TGWindow *main,
                           UInt_t w, UInt_t h) :
    TGTransientFrame(p, main, w, h)
{
   // Dialog used to test the different supported progress bars.

   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   fHframe1 = new TGHorizontalFrame(this, 0, 0, 0);

   fVProg1 = new TGVProgressBar(fHframe1, TGProgressBar::kFancy, 300);
   fVProg1->SetBarColor("purple");
   fVProg2 = new TGVProgressBar(fHframe1, TGProgressBar::kFancy, 300);
   fVProg2->SetFillType(TGProgressBar::kBlockFill);
   fVProg2->SetBarColor("green");

   fHframe1->Resize(300, 300);

   fVframe1 = new TGVerticalFrame(this, 0, 0, 0);

   fHProg1 = new TGHProgressBar(fVframe1, 300);
   fHProg1->ShowPosition();
   fHProg2 = new TGHProgressBar(fVframe1, TGProgressBar::kFancy, 300);
   fHProg2->SetBarColor("lightblue");
   fHProg2->ShowPosition(kTRUE, kFALSE, "%.0f events");
   fHProg3 = new TGHProgressBar(fVframe1, TGProgressBar::kStandard, 300);
   fHProg3->SetFillType(TGProgressBar::kBlockFill);

   fGO = new TGTextButton(fVframe1, "Go", 10);
   fGO->Associate(this);

   fVframe1->Resize(300, 300);

   fHint1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY, 5, 10, 5, 5);
   fHint2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5,  5, 10);
   fHint3 = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 50, 50, 0);
   fHint4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0);
   fHint5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);

   fHframe1->AddFrame(fVProg1, fHint1);
   fHframe1->AddFrame(fVProg2, fHint1);

   fVframe1->AddFrame(fHProg1, fHint2);
   fVframe1->AddFrame(fHProg2, fHint2);
   fVframe1->AddFrame(fHProg3, fHint2);
   fVframe1->AddFrame(fGO,     fHint3);

   AddFrame(fHframe1, fHint4);
   AddFrame(fVframe1, fHint5);

   SetWindowName("Progress Test");
   TGDimension size = GetDefaultSize();
   Resize(size);

   // position relative to the parent's window
   Window_t wdummy;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdummy);
   Move(ax, ay);

   MapSubwindows();
   MapWindow();

   fClient->WaitFor(this);
}

TestProgress::~TestProgress()
{
   // Delete dialog.

   delete fHframe1;
   delete fVframe1;
   delete fVProg1; delete fVProg2;
   delete fHProg1; delete fHProg2; delete fHProg3;
   delete fHint1; delete fHint2; delete fHint3; delete fHint4; delete fHint5;
   delete fGO; fGO = 0; // to get cleanly out of the loop in ProcessMessage
}

void TestProgress::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t TestProgress::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process slider messages.

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch (parm1) {
                  case 10:
                     {
                        fVProg1->Reset(); fVProg2->Reset();
                        fHProg1->Reset(); fHProg2->Reset(); fHProg3->Reset();
                        fVProg2->SetBarColor("green");
                        int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
                        int inc1 = 4, inc2 = 3, inc3 = 2, inc4 = 1;
                        while (cnt1 < 100 || cnt2 < 100 || cnt3 < 100 || cnt4 <100) {
                           if (cnt1 < 100) {
                              cnt1 += inc1;
                              fVProg1->Increment(inc1);
                           }
                           if (cnt2 < 100) {
                              cnt2 += inc2;
                              fVProg2->Increment(inc2);
                              if (cnt2 > 75)
                                 fVProg2->SetBarColor("red");
                           }
                           if (cnt3 < 100) {
                              cnt3 += inc3;
                              fHProg1->Increment(inc3);
                           }
                           if (cnt4 < 100) {
                              cnt4 += inc4;
                              fHProg2->Increment(inc4);
                              fHProg3->Increment(inc4);
                           }
                           gSystem->Sleep(100);
                           gSystem->ProcessEvents();
                           // if user closed window return
                           if (!fGO) return kTRUE;
                        }
                     }
                     break;
                  default:
                     break;
               }
               break;
            default:
               break;
         }
         break;
      default:
         break;
   }
   return kTRUE;
}


// TGNumberEntry widget test dialog
const char *const EntryTestDlg::numlabel[13] = {
   "Integer",
   "One digit real",
   "Two digit real",
   "Three digit real",
   "Four digit real",
   "Real",
   "Degree.min.sec",
   "Min:sec",
   "Hour:min",
   "Hour:min:sec",
   "Day/month/year",
   "Month/day/year",
   "Hex"
};

const Double_t EntryTestDlg::numinit[13] = {
   12345, 1.0, 1.00, 1.000, 1.0000, 1.2E-12,
   90 * 3600, 120 * 60, 12 * 60, 12 * 3600 + 15 * 60,
   19991121, 19991121, (Double_t) 0xDEADFACE
};

EntryTestDlg::EntryTestDlg(const TGWindow * p, const TGWindow * main)
 : TGTransientFrame(p, main, 10, 10, kHorizontalFrame)
{
   // build widgets
   fF1 = new TGVerticalFrame(this, 200, 300);
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
   AddFrame(fF1, fL1);
   fL2 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);
   for (int i = 0; i < 13; i++) {
      fF[i] = new TGHorizontalFrame(fF1, 200, 30);
      fF1->AddFrame(fF[i], fL2);
      fNumericEntries[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20,
                                             (TGNumberFormat::EStyle) i);
      fNumericEntries[i]->Associate(this);
      fF[i]->AddFrame(fNumericEntries[i], fL2);
      fLabel[i] = new TGLabel(fF[i], numlabel[i]);
      fF[i]->AddFrame(fLabel[i], fL2);
   }
   fF2 = new TGVerticalFrame(this, 200, 500);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
   AddFrame(fF2, fL3);
   fLowerLimit = new TGCheckButton(fF2, "lower limit:", 4);
   fLowerLimit->Associate(this);
   fF2->AddFrame(fLowerLimit, fL3);
   fLimits[0] = new TGNumberEntry(fF2, 0, 12, 10);
   fLimits[0]->SetLogStep(kFALSE);
   fLimits[0]->Associate(this);
   fF2->AddFrame(fLimits[0], fL3);
   fUpperLimit = new TGCheckButton(fF2, "upper limit:", 5);
   fUpperLimit->Associate(this);
   fF2->AddFrame(fUpperLimit, fL3);
   fLimits[1] = new TGNumberEntry(fF2, 0, 12, 11);
   fLimits[1]->SetLogStep(kFALSE);
   fLimits[1]->Associate(this);
   fF2->AddFrame(fLimits[1], fL3);
   fPositive = new TGCheckButton(fF2, "Positive", 6);
   fPositive->Associate(this);
   fF2->AddFrame(fPositive, fL3);
   fNonNegative = new TGCheckButton(fF2, "Non negative", 7);
   fNonNegative->Associate(this);
   fF2->AddFrame(fNonNegative, fL3);
   fSetButton = new TGTextButton(fF2, " Set ", 2);
   fSetButton->Associate(this);
   fF2->AddFrame(fSetButton, fL3);
   fExitButton = new TGTextButton(fF2, " Close ", 1);
   fExitButton->Associate(this);
   fF2->AddFrame(fExitButton, fL3);

   // set dialog box title
   SetWindowName("Number Entry Test");
   SetIconName("Number Entry Test");
   SetClassHints("NumberEntryDlg", "NumberEntryDlg");
   // resize & move to center
   MapSubwindows();
   UInt_t width = GetDefaultWidth();
   UInt_t height = GetDefaultHeight();
   Resize(width, height);
   Int_t ax;
   Int_t ay;
   if (main) {
      Window_t wdum;
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                      (Int_t)(((TGFrame *) main)->GetWidth() -
                                       fWidth) >> 1,
                                      (Int_t)(((TGFrame *) main)->GetHeight() -
                                       fHeight) >> 1, ax, ay, wdum);
   } else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay,
                               root_w, root_h);
      ax = (Int_t)(root_w - fWidth) >> 1;
      ay = (Int_t)(root_h - fHeight) >> 1;
   }
   Move(ax, ay);
   SetWMPosition(ax, ay);
   // make the message box non-resizable
   SetWMSize(width, height);
   SetWMSizeHints(width, height, width, height, 0, 0);
   SetMWMHints(kMWMDecorAll | kMWMDecorResizeH | kMWMDecorMaximize |
               kMWMDecorMinimize | kMWMDecorMenu,
               kMWMFuncAll | kMWMFuncResize | kMWMFuncMaximize |
               kMWMFuncMinimize, kMWMInputModeless);

   MapWindow();
   fClient->WaitFor(this);
}

EntryTestDlg::~EntryTestDlg()
{
   for (int i = 0; i < 13; i++) {
      delete fNumericEntries[i];
      delete fLabel[i];
      delete fF[i];
   }
   delete fLowerLimit;
   delete fUpperLimit;
   delete fLimits[0];
   delete fLimits[1];
   delete fPositive;
   delete fNonNegative;
   delete fSetButton;
   delete fExitButton;
   delete fF1;
   delete fF2;
   delete fL1;
   delete fL2;
   delete fL3;
}

void EntryTestDlg::CloseWindow()
{
   delete this;
}

void EntryTestDlg::SetLimits()
{
   Double_t min = fLimits[0]->GetNumber();
   Bool_t low = (fLowerLimit->GetState() == kButtonDown);
   Double_t max = fLimits[1]->GetNumber();
   Bool_t high = (fUpperLimit->GetState() == kButtonDown);
   TGNumberFormat::ELimit lim;
   if (low && high) {
      lim = TGNumberFormat::kNELLimitMinMax;
   } else if (low) {
      lim = TGNumberFormat::kNELLimitMin;
   } else if (high) {
      lim = TGNumberFormat::kNELLimitMax;
   } else {
      lim = TGNumberFormat::kNELNoLimits;
   }
   Bool_t pos = (fPositive->GetState() == kButtonDown);
   Bool_t nneg = (fNonNegative->GetState() == kButtonDown);
   TGNumberFormat::EAttribute attr;
   if (pos) {
      attr = TGNumberFormat::kNEAPositive;
   } else if (nneg) {
      attr = TGNumberFormat::kNEANonNegative;
   } else {
      attr = TGNumberFormat::kNEAAnyNumber;
   }
   for (int i = 0; i < 13; i++) {
      fNumericEntries[i]->SetFormat(fNumericEntries[i]->GetNumStyle(), attr);
      fNumericEntries[i]->SetLimits(lim, min, max);
   }
}

Bool_t EntryTestDlg::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      {
         switch (GET_SUBMSG(msg)) {
         case kCM_BUTTON:
            {
               switch (parm1) {
                  // exit button
               case 1:
                  {
                     CloseWindow();
                     break;
                  }
                  // set button
               case 2:
                  {
                     SetLimits();
                     break;
                  }
               }
               break;
            }
         }
         break;
      }
   }
   return kTRUE;
}


Editor::Editor(const TGWindow *main, UInt_t w, UInt_t h) :
    TGTransientFrame(gClient->GetRoot(), main, w, h)
{
   // Create an editor in a dialog.

   fEdit = new TGTextEdit(this, w, h, kSunkenFrame | kDoubleBorder);
   fL1 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 3, 3, 3, 3);
   AddFrame(fEdit, fL1);

   fOK = new TGTextButton(this, "  &OK  ");
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
   AddFrame(fOK, fL2);

   SetTitle();

   MapSubwindows();

   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   // editor covers right half of parent window
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                        0,
                        (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                        ax, ay, wdum);
   Move((((TGFrame *) main)->GetWidth() >> 1) + ax, ay);
   SetWMPosition((((TGFrame *) main)->GetWidth() >> 1) + ax, ay);
}

Editor::~Editor()
{
   // Delete editor dialog.

   delete fEdit;
   delete fOK;
   delete fL1;
   delete fL2;
}

void Editor::SetTitle()
{
   // Set title in editor window.

   TGText *txt = GetEditor()->GetText();
   Bool_t untitled = !strlen(txt->GetFileName()) ? kTRUE : kFALSE;

   char title[256];
   if (untitled)
      sprintf(title, "ROOT Editor - Untitled");
   else
      sprintf(title, "ROOT Editor - %s", txt->GetFileName());

   SetWindowName(title);
   SetIconName(title);
}

void Editor::Popup()
{
   // Show editor.

   MapWindow();
}

void Editor::LoadBuffer(const char *buffer)
{
   // Load a text buffer in the editor.

   fEdit->LoadBuffer(buffer);
}

void Editor::LoadFile(const char *file)
{
   // Load a file in the editor.

   fEdit->LoadFile(file);
}

void Editor::CloseWindow()
{
   // Called when closed via window manager action.

   delete this;
}

Bool_t Editor::ProcessMessage(Long_t msg, Long_t, Long_t)
{
   // Process OK button.

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               // Only one button and one action...
               delete this;
               break;
            default:
               break;
         }
         break;
      case kC_TEXTVIEW:
         switch (GET_SUBMSG(msg)) {
            case kTXT_CLOSE:
               // close window
               delete this;
               break;
            case kTXT_OPEN:
               SetTitle();
               break;
            case kTXT_SAVE:
               SetTitle();
               break;
            default:
               break;
         }
      default:
         break;
   }

   return kTRUE;
}


//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }

   TestMainFrame mainWindow(gClient->GetRoot(), 400, 220);

   theApp.Run();

   return 0;
}
