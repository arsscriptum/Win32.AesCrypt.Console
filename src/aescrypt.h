
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
#include "aescrypt_defs.h"


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
