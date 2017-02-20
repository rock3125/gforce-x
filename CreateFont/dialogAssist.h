#pragma once

void SetText( HWND ctrl, const std::string& str );


// set width/height of a control without moving it
void SetControlHeight( HWND ctrl, int height );
void SetControlWidth( HWND ctrl, int width );
void SetControlFont( HWND ctrl, HFONT font, bool redraw );
void EnableControl( HWND ctrl, bool enable );
void ShowControl( HWND ctrl, bool show );
bool Enabled( HWND ctrl );

// edit assist routines
void EditPrint( HWND ctrl, char *text, ... );
void EditPrintSingle( HWND ctrl, const char *text );
void EditClear( HWND ctrl );
void EditGetText( HWND ctrl, char* buffer, size_t bufferSize );
int  EditGetInt( HWND ctrl );
float EditGetFloat( HWND ctrl );

// tab controls
int AddTabItem( HWND tabControl, size_t index, const std::string& _str, long data = 0 );

// listbox assist routines
void ListBoxAddString1( HWND ctrl, const char* str );
void ComboBoxAddString( HWND ctrl, const char* str );
void ComboBoxDeleteString( HWND ctrl, size_t index );
void ComboBoxClear( HWND ctrl );
void ComboBoxDeleteStrings( HWND ctrl );

// checkbox/radio button assist routines
bool ButtonGetCheck( HWND ctrl );
void ButtonSetCheck( HWND ctrl, bool check );

// dropdownbox assist routines
int  DropDownBoxGetSelection( HWND ctrl ); // -1 if no selection
void DropDownBoxSetSelection( HWND ctrl, int sel );

// slider
void SliderSetFrequency( HWND ctrl, size_t freq );
void SliderSetRange( HWND ctrl, size_t min, size_t max );
void SliderSetPos( HWND ctrl, size_t pos );
void SliderSetPos( HWND ctrl, float pos );
void SliderGetPos( HWND ctrl, size_t& pos );
void SliderGetPos( HWND ctrl, float& pos );
int  SliderGetPos( HWND ctrl );

// point inside rect?
bool InsideRect( RECT& r, POINT& p );

char* TranslateString( char *inbuf, char* outbuf );

// additem to gui list
#ifdef _USEGUI
void AddItem( TGUI*& page, TGUI* item );
#endif

// scrollbars
void ScrollbarSetRange( HWND ctrl, int min, int max );
void ScrollbarSetPosition(HWND ctrl, int thumbPos);
int  ScrollbarGetPosition( HWND ctrl );
void ScrollbarGetRange( HWND ctrl, int& min, int& max);

