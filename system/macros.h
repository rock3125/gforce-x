#pragma once

//////////////////////////////////////////////////////////
// macros

// precond
#define PreCond(c)				{ System::Fail((c),#c,__LINE__,__FILE__); assert(c); }

#define safe_delete(d)			{ if ((d)!=NULL) { delete (d); (d)=NULL; } }
#define safe_release(r)			{ if ((r)!=NULL) { (r)->Release(); (r)=NULL; } }
#define safe_delete_array(d)	{ if ((d)!=NULL) { delete[] (d); (d)=NULL; } }

#define safe_delete_stl_array(d) { for (int i=0; i<(d).size(); i++) { safe_delete((d)[i]); } (d).clear(); }
