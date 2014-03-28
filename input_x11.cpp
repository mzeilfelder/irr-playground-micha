// Based on code from: Volume One: Xlib Programming Manual - Chapter 11
// http://menehune.opt.wfu.edu/Kokua/Irix_6.5.21_doc_cd/usr/share/Insight/library/SGI_bookshelves/SGI_Developer/books/XLib_PG/sgi_html/ch11.html
// Changes by Michael Zeilfelder:
// - Make it compile on modern compilers
// - kick out an outdated XSI workaround which prevented it from working on a modern X11.
// - reduce code to the minimal core of stuff necessary
// - use some c++ feature (comments, variable declaration no longer at the start)

#include <stdio.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

// Include <locale.h> or the non-standard X substitutesdepending on the X_LOCALE compilation flag
#include <X11/Xlocale.h>

/*
 * This function chooses the "more desirable" of two input styles.  The
 * style with the more complicated Preedit style is returned, and if the
 * styles have the same Preedit styles, then the style with the more
 * complicated Status style is returned.  There is no "official" way to
 * order interaction styles.  This one makes the most sense to me.
 * This is a long procedure for a simple heuristic.
 */
XIMStyle ChooseBetterStyle(XIMStyle style1, XIMStyle style2)
{
	XIMStyle s,t;
	XIMStyle preedit = XIMPreeditArea | XIMPreeditCallbacks |
                       XIMPreeditPosition | XIMPreeditNothing | XIMPreeditNone;
	XIMStyle status = XIMStatusArea | XIMStatusCallbacks |
                      XIMStatusNothing | XIMStatusNone;
	if (style1 == 0) return style2;
	if (style2 == 0) return style1;
	if ((style1 & (preedit | status)) == (style2 & (preedit | status)))
		return style1;
	s = style1 & preedit;
	t = style2 & preedit;
	if (s != t) 
	{
		if (s | t | XIMPreeditCallbacks)
			return (s == XIMPreeditCallbacks)?style1:style2;
		else if (s | t | XIMPreeditPosition)
			return (s == XIMPreeditPosition)?style1:style2;
		else if (s | t | XIMPreeditArea)
			return (s == XIMPreeditArea)?style1:style2;
		else if (s | t | XIMPreeditNothing)
			return (s == XIMPreeditNothing)?style1:style2;
	}
	else 
	{ /* if preedit flags are the same, compare status flags */
		s = style1 & status;
		t = style2 & status;
		if (s | t | XIMStatusCallbacks)
			return (s == XIMStatusCallbacks)?style1:style2;
		else if (s | t | XIMStatusArea)
			return (s == XIMStatusArea)?style1:style2;
		else if (s | t | XIMStatusNothing)
			return (s == XIMStatusNothing)?style1:style2;
	}
}


int main(int argc, char *argv[])
{
	if (setlocale(LC_ALL, "") == NULL) // rather important or switching to another language might not work at all
	{
		fprintf(stderr, "cannot set locale. \n");
		return 1;
	}
	Display *dpy = 	XOpenDisplay(NULL);
	if (!dpy) 
	{
		fprintf(stderr, "cannot open Display. \n");
		return 1;
	}
	if (!XSupportsLocale()) 
	{
		fprintf(stderr, "X does not support locale %s. \n", setlocale(LC_ALL, NULL));
		return 1;
	}
	if (XSetLocaleModifiers("") == NULL) // (no troubles when not setting it, guess default locale is fine as well)
	{
		fprintf(stderr, "Warning: cannot set locale modifiers. \n");
	}

	// Create a fontset (in Debian it's in package: ttf-xfree86-nonfree)
	char **missing_charsets;
	int num_missing_charsets = 0;
	char *default_string;	
	XFontSet fontset = XCreateFontSet(dpy,
	                                  "-adobe-helvetica-*-r-*-*-*-120-*-*-*-*-*-*,\
	                                  -misc-fixed-*-r-*-*-*-130-*-*-*-*-*-*",
	                                  &missing_charsets, &num_missing_charsets,
	                                  &default_string);

	// Print a warning message if there are charsets for which no fonts can be found.
	if (num_missing_charsets > 0) 
	{
		fprintf(stderr, "The following charsets are missing:\n");
		for(int i=0; i < num_missing_charsets; i++)
			fprintf(stderr, "%s\n", missing_charsets[i]);
		XFreeStringList(missing_charsets);
		fprintf(stderr, "Following string will be used instead of characters from those sets: %s\n", default_string);
	}

	// setup a window
	int screen = DefaultScreen(dpy);
	Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0, 400, 100,
	                                 2, WhitePixel(dpy,screen),BlackPixel(dpy,screen));
	XGCValues gcv;	
	GC gc = XCreateGC(dpy,win,0,&gcv);
	XSetForeground(dpy,gc,WhitePixel(dpy,screen));
	XSetBackground(dpy,gc,BlackPixel(dpy,screen));
	XMapWindow(dpy,win);	

	// Connect to an input method.
	XIM im = XOpenIM(dpy, NULL, NULL, NULL);
	if (!im) 
	{
		fprintf(stderr, "Couldn't open input method");
		return 1;
	}

	// Flags we want (keep it simple for now)
	XIMStyle app_supported_styles = XIMPreeditNone | XIMPreeditNothing | XIMPreeditArea;
	app_supported_styles |= XIMStatusNone | XIMStatusNothing | XIMStatusArea;

	// Check what we can get
	XIMStyles *im_supported_styles;
	XGetIMValues(im, XNQueryInputStyle, &im_supported_styles, NULL);
	XIMStyle best_style = 0;
	for(int i=0; i < im_supported_styles->count_styles; i++) 
	{
		XIMStyle style = im_supported_styles->supported_styles[i];
		if ((style & app_supported_styles) == style) // we can handle it?
		{
			best_style = ChooseBetterStyle(style, best_style);
//			best_style = style;
//			break;
		}
	}
	if (best_style == 0)
	{
		fprintf(stderr, "Didn't get our interaction style.\n");
		return 1;
	}
	XFree(im_supported_styles);

	// Create an IC using the style we chose.
	// Also set the window and fontset attributes.
	XVaNestedList list = XVaCreateNestedList(0,XNFontSet,fontset,NULL);
	XIC ic = XCreateIC(im,
	                   XNInputStyle, best_style,
	                   XNClientWindow, win,
	                   XNPreeditAttributes, list,
	                   XNStatusAttributes, list,
	                   NULL);
	XFree(list);
	if (ic == NULL) 
	{
		fprintf(stderr, "Couldn't create input context\n");
		return 1;
	}
	long im_event_mask = 0;	
	XGetICValues(ic, XNFilterEvents, &im_event_mask, NULL);
	XSelectInput(dpy,win, ExposureMask | KeyPressMask | im_event_mask);
	XSetICFocus(ic);

	wchar_t string[2000];
	int str_len = 0;
	while(1) 
	{
		XEvent event;		
		XNextEvent(dpy, &event);
		if (XFilterEvent(&event, None))
			continue;
		switch (event.type) 
		{
			case Expose:
				// draw the string at a hard-coded location
				if (event.xexpose.count == 0)
					XwcDrawString(dpy, win, fontset, gc, 10, 50, string, str_len);
			break;
			case KeyPress:
			{
				Status status;				
				KeySym keysym;				
				int buf_len = 10;				
				wchar_t *buffer = (wchar_t *)malloc(buf_len * sizeof(wchar_t));
				int len = XwcLookupString(ic, &event.xkey, buffer, buf_len,
				                          &keysym, &status);

				if (status == XBufferOverflow) 
				{
					buf_len = len;
					buffer = (wchar_t *)realloc((char *)buffer, 
					                            buf_len * sizeof(wchar_t));
					len = XwcLookupString(ic, &event.xkey, buffer, buf_len,
					                      &keysym, &status);
				}
				bool redraw = false;
				switch (status) 
				{
					case XLookupNone: 		
						printf("XLookupNone\n"); 
					break;
					case XLookupKeySym: 	
						printf("XLookupKeySym: %d\n", (int)keysym); 
					break;
					case XBufferOverflow:	
						printf("XBufferOverflow\n"); 
					break;
					case XLookupBoth:
						printf("XLookupBoth: %d\n", (int)keysym);
						// fallthrough 
					case XLookupChars:
						if ( len == 0 )
							printf("len is 0\n");
						for(int i=0; i < len; i++)
							string[str_len++] = buffer[i];
						redraw = true;
						break;
				}
				free(buffer);
				
				if (redraw) 
				{
					// redraw string when we got new characters
					XClearWindow(dpy, win);
					XwcDrawString(dpy, win, fontset, gc, 10, 50, string, str_len);
				}
			}
			break;
		}
	}

	return 0;
}
