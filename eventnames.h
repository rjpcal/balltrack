/*/////////////////////////////////////////////////////////////////////
//
// eventnames.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Mar 28 17:50:30 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
/////////////////////////////////////////////////////////////////////*/

#ifndef EVENTNAMES_H_DEFINED
#define EVENTNAMES_H_DEFINED


#ifdef __cplusplus
static const char *event_names[] =
{
#else
static char *event_names[] =
{
#endif
"",
"",
"KeyPress",
"KeyRelease",
"ButtonPress",
"ButtonRelease",
"MotionNotify",
"EnterNotify",
"LeaveNotify",
"FocusIn",
"FocusOut",
"KeymapNotify",
"Expose",
"GraphicsExpose",
"NoExpose",
"VisibilityNotify",
"CreateNotify",
"DestroyNotify",
"UnmapNotify",
"MapNotify",
"MapRequest",
"ReparentNotify",
"ConfigureNotify",
"ConfigureRequest",
"GravityNotify",
"ResizeRequest",
"CiculateNotify",
"CirculateRequest",
"PropertyNotify",
"SelectionClear",
"SelectionRequest",
"SelectionNotify",
"ColormapNotify",
"ClientMessage",
"MappingNotify",
};

#endif // !EVENTNAMES_H_DEFINED
