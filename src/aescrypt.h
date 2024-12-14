
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



#ifndef __AESCRYPT_H__
#define __AESCRYPT_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "aes.h"
#include "sha256.h"

typedef enum TOperatingMode {
    Encrypt = 0,
    Decrypt = 1,
    Analyse = 2
} TOperatingModeType;

// Structure to hold command line options
struct aescrypter_options {
    int encrypt;
    int decrypt;
    int analyse;
    int mode;
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
        : encrypt(0), decrypt(0), analyse(0), read_password(0), input_file_set(0), output_file_set(0),
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

void dump_cmd_options(const aescrypter_options* options);
int parse_arguments(int argc, char* argv[], aescrypter_options* options);
void show_usage();
void show_banner();
void show_version();
void cleanup(const wchar_t* outfile);
bool file_exists(const std::wstring& filename);
std::wstring generate_output_filename(const std::wstring& input_file, int mode);
int encrypt_stream(FILE* infp, FILE* outfp, wchar_t* passwd, int passlen);
int decrypt_stream(FILE* infp, FILE* outfp, wchar_t* passwd, int passlen);
#endif // __AESCRYPT_H__
