
//==============================================================================
//
//  AES Crypt for Windows (console application)
//  Copyright (C) 2007, 2008, 2009, 2010, 2013-2015
//
//  Contributors:
//      Glenn Washburn <crass@berlios.de>
//      Paul E. Jones <paulej@packetizer.com>
//      Mauro Gilardi <galvao.m@gmail.com>
//      Guilaume Plante 2020 <guillaumeplante.qc@gmail.com>
//
//============================================================================
//  Copyright (C) Guilaume Plante 2020 <guillaumeplante.qc@gmail.com>
//==============================================================================


#ifndef __AESCRYPT_PASSWORD_H__
#define __AESCRYPT_PASSWORD_H__

#include "aescrypt_defs.h"
/*
 * Error codes for read_password function.
 */
#define AESCRYPT_READPWD_GETWCHAR    -5 
#define AESCRYPT_READPWD_TOOLONG     -6
#define AESCRYPT_READPWD_TOOSHORT    -7
#define AESCRYPT_READPWD_NOMATCH     -8

/*
 * Function Prototypes
 */
const wchar_t* read_password_error(int error);
int read_password(wchar_t* buffer, operatingmode_t mode);

#endif // __AESCRYPT_PASSWORD_H__
