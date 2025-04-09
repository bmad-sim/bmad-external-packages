#ifndef PGDISPD_H
#define PGDISPD_H

#include "figdisp.h"

int getcolors(int vistype,              /* The type of visual to use */
              Visual **visual,          /* The visual actually used */
              Colormap *cmap,           /* The color map actually used */
              unsigned long *pix,       /* The pixels allocated */
              int maxcolors,            /* The maximum number of colors to allocate */
              int mincolors,            /* The minimum number of colors to allocate */
              int *depth,               /* The depth of the visual actually used */
              int maxdepth,             /* The maximum allowed visual depth */
              int mindepth);            /* The minimum allowed visual depth */

int proccom(unsigned short *buf,        /* the buffer of commands and arguments */
            int len,                    /* the length of the buffer */
            unsigned short *retbuf,     /* a buffer for return values */
            int *retbuflen);            /* the length of retbuf */

void returnbuf(short *msg,	/* the message to send to the client. */
               int len,	        /* The length of the message. */
               Window destwin);	/* The window who's atom should be changed. */

int waitevent();

int handlexevent(XEvent event,
                 int *go_on);	/* whether the calling routine shoudl exit successfully */

#endif
