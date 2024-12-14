
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



#ifndef __AESCRYPT_DEFS_H__
#define __AESCRYPT_DEFS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MIN_PASSWD_LEN  6
#define MAX_PASSWD_LEN  64
#define MAX_FILENAME_LEN  1024

#define APP_ExECUTABLE L"AesCrypter.exe"
#define APP_NAME "AesCrypter"

typedef enum {
    UNINIT = 0,
    DECRYPT = 1,
    ENCRYPT = 2,
    ANALYSE = 3
} operatingmode_t;

typedef enum {
    NONE = -1,
    PASSWORD = 0,
    KEYFILE = 1,
    CERTIFICATE = 2,
    SYSTEM = 3
} encryptingmode_t;


// Structure to hold command line options
struct aescrypter_options {
    int encrypt;
    int decrypt;
    int analyse;
    int mode;
    int force;
    int read_password;
    int input_file_set;
    int output_file_set;
#ifdef _DEBUG
    int debug;
#endif

    wchar_t* password;
    wchar_t* cert_path;
    wchar_t* key_file;
    wchar_t* generate_path;
    wchar_t* input_file;
    wchar_t* output_file;
    // Constructor to initialize default values
    aescrypter_options()
        : encrypt(0), decrypt(0), analyse(0), read_password(0), input_file_set(0), output_file_set(0), force(0),
        password(nullptr), cert_path(nullptr),
        key_file(nullptr), generate_path(nullptr), input_file(nullptr), output_file(nullptr)
    {
#ifdef _DEBUG
        debug = 0;
#endif

    }
};

typedef struct {
    char aes[3];
    unsigned char version;
    unsigned char last_block_size;
} aescrypt_hdr;

typedef unsigned char sha256_t[32];

#endif // __AESCRYPT_DEFS_H__
