/* ppmhist.c - read a portable pixmap and compute a color histogram
**
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include "ppm.h"
#include "ppmcmap.h"

#define MAXCOLORS 100000

static int
countcompare( ch1, ch2 )
colorhist_vector ch1, ch2;
    {
    return ch2->value - ch1->value;
    }

int
main( argc, argv )
    int argc;
    char* argv[];
    {
    FILE* ifp;
    pixel** pixels;
    colorhist_vector chv;
    int argn, rows, cols, colors, i;
    pixval maxval;
    char* usage = "[-map] [ppmfile]";
    int map;


    ppm_init( &argc, argv );

    map = 0;
    argn = 1;

    while ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' )
        {
        if( pm_keymatch( argv[argn], "-map", 2 ) )
            map = 1;
        else if( pm_keymatch( argv[argn], "-nomap", 2 ) )
            map = 0;
        else
            pm_usage( usage );
        ++argn;
        }

    if ( argn != argc )
        {
        ifp = pm_openr( argv[argn] );
        argn++;
        }
    else
        ifp = stdin;

    if ( argn != argc )
        pm_usage( usage );

    pixels = ppm_readppm( ifp, &cols, &rows, &maxval );

    pm_close( ifp );

    chv = ppm_computecolorhist( pixels, cols, rows, MAXCOLORS, &colors );
    if ( chv == (colorhist_vector) 0 )
        pm_error( "too many colors - try doing a ppmquant" );

    /* Sort by count. */
    qsort( (char*) chv, colors, sizeof(struct colorhist_item), countcompare );

    /* And print the histogram. */
    if( map )
        {
        printf("P3\n# color map\n%d 1\n%d\n", colors, maxval);
        printf( "# r   g   b   \tlum\tcount\n" );
        printf( "#--- --- ---  \t---\t-----\n" );
        }
    else
        {
        printf( " r   g   b \tlum\tcount\n" );
        printf( "--- --- ---\t---\t-----\n" );
        }

    for ( i = 0; i < colors; i++ )
        printf(
            "%3d %3d %3d%s\t%d\t%d\n", PPM_GETR(chv[i].color),
            PPM_GETG(chv[i].color), PPM_GETB(chv[i].color),
            (map ? " #" : ""),
            (int) ( PPM_LUMIN( chv[i].color ) + 0.5 ),
            chv[i].value );

    ppm_freecolorhist( chv );

    exit( 0 );
    }
