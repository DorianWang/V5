/* $Id: parasol.h 7632 2007-11-14 18:22:06Z greg $ */
/************************************************************************/
/*	parasol.h - main PARASOL header file				*/
/*									*/
/*	Copyright (C) 1993 School of Computer Science, 			*/
/*		Carleton University, Ottawa, Ont., Canada		*/
/*		Written by John Neilson					*/
/*									*/
/*  This library is free software; you can redistribute it and/or	*/
/*  modify it under the terms of the GNU Library General Public		*/
/*  License as published by the Free Software Foundation; either	*/
/*  version 2 of the License, or (at your option) any later version.	*/
/*									*/
/*  This library is distributed in the hope that it will be useful,	*/
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*/
/*  Library General Public License for more details.			*/
/*									*/
/*  You should have received a copy of the GNU Library General Public	*/
/*  License along with this library; if not, write to the Free		*/
/*  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.	*/
/*									*/
/*  The author may be reached at neilson@scs.carleton.ca or through	*/
/*  the School of Computer Science at Carleton University.		*/
/*									*/
/*	Created: 03/06/91 (JEN)						*/
/*									*/
/*	Revised: 06/08/91 (JEN) Removed "machine" from setjmp.h ref for	*/
/*				compatibility				*/
/*									*/
/*		 05/02/92 (PKC) Added version.h file.                  	*/
/*		 26/05/93 (JEN) Added malloc.h file.			*/
/*		 09/06/93 (JEN) Revised for Version 2			*/
/*		 07/18/95 (PRM)	Added ctype.h #include			*/
/************************************************************************/

#ifndef	_PARASOL
#define	_PARASOL

#if HAVE_CONFIG_H
#include	<config.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<setjmp.h>
#include	<signal.h>
#include	<math.h>
#include <parasol/para_protos.h>
#include "test_storage.h"

#if defined(HAVE_MALLOC_H)
#include	<malloc.h>
#endif

#ifdef __cplusplus
   #include <iostream>
   #if defined(__GNUC__)
      #define FUNC_NAME_OUT std::cout << "Current func: " << __PRETTY_FUNCTION__ << "\n";
   #elif defined(_MSC_VER)
      #define FUNC_NAME_OUT std::cout << "Current func: " << __FUNCSIG__ << "\n";
   #else
      #define FUNC_NAME_OUT std::cout << "Current func: " << __func__ << "\n"
   #endif

#else
   // Don't include iostream, use printf instead.
   #if defined(__GNUC__)
      #define FUNC_NAME_OUT printf("Current func: %s\n", __PRETTY_FUNCTION__);
   #elif defined(_MSC_VER)
      #define FUNC_NAME_OUT printf("Current func: %s\n", __FUNCSIG__);
   #else
      #define FUNC_NAME_OUT printf("Current func: %s\n", __func__);
   #endif
#endif

#define PRINT_FUNC_NAME_ONCE static int already_printed = 1; \
                           if (already_printed > 0){ \
                              FUNC_NAME_OUT \
                              already_printed--; \
                           }
#define PRINT_FUNC_NAME_N(x) static int already_printed = (x); \
                           if (already_printed > 0){ \
                              FUNC_NAME_OUT \
                              already_printed--; \
                           }


#endif	/* _PARASOL */
