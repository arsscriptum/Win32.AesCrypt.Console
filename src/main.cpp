

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


#include <windows.h>
#include <Wincrypt.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <wchar.h>
#include "opts.h"
#include "log.h"
#include "aescrypt.h"
#include "password.h"
#include "version.h"
#include <stdlib.h>
#include "aescrypt_defs.h"
/*
 *  Define externals
 */
extern wchar_t* optarg;
extern int     optopt;
extern int     optind;
extern int     opterr;

#define MAX_FILENAME_LEN  1024
extern bool gLogsEnabled;
void show_banner() {
    wprintf(L"\n");
    wprintf(L"%ls v1.17 - Encryption/Decryption tool using AES.\n", APP_ExECUTABLE);
    wprintf(L"Copyright(C) 1999 - 2023  Guillaume Plante\n");
    wprintf(L"arsscriptum - arsscriptum.ddns.net\n");
    wprintf(L"\n");
}


// Function to show usage
void show_usage() {
    wprintf(L"Usage: aescrypter.exe {--encrypt|--decrypt|--analyse} file [options]\n");
    wprintf(L"\n");
    wprintf(L"Operations:\n");
    wprintf(L"  -d, --decrypt              : decrypt a file\n");
    wprintf(L"  -e, --encrypt              : encrypt a file\n");
    wprintf(L"  -a, --analyse              : analyse a file\n");
    wprintf(L"\n");
    wprintf(L"Options:\n");
    wprintf(L"  -p, --password <string>    : use clear password string\n");
    wprintf(L"  -c, --cert <path>          : use certificate\n");
    wprintf(L"  -o, --output <path>        : output file\n");
    wprintf(L"  -k, --key-file <path>      : encrypt/decrypt using a key stored in a file\n");
    wprintf(L"  -f, --force                : overwrite output file if exists\n");
    wprintf(L"  -g, --generate <path>      : only with 'encrypt' option: generate a key and store in a file\n");
    wprintf(L"  -h, --help                 : show this help message\n");
}

// Function to parse the command line arguments and populate the options structure
int parse_arguments(int argc, wchar_t* argv[], aescrypter_options* options) {
    // Initialize the options
    memset(options, 0, sizeof(aescrypter_options) / sizeof(wchar_t));

    // Parse the arguments manually
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"--decrypt") == 0 || wcscmp(argv[i], L"-d") == 0) {
            if (i + 1 < argc) {
                options->mode = (int)operatingmode_t::DECRYPT;
                options->decrypt = 1;
                options->input_file = argv[++i];
                options->input_file_set = 1;
            } else {
                COUTERR("Error: Missing input file argument for \"%s\" - %ls\n", argv[i], L"decrypt");
                return -6;
            }
        }
        else if (wcscmp(argv[i], L"--encrypt") == 0 || wcscmp(argv[i], L"-e") == 0) {
            if (i + 1 < argc) {
                options->mode = (int)operatingmode_t::ENCRYPT;
                options->encrypt = 1;
                options->input_file = argv[++i];
                options->input_file_set = 1;
            }
            else {
                COUTERR("Error: Missing input file argument for \"%s\" - %ls\n", argv[i], L"encrypt");
                return -6;
            }
        }
        else if (wcscmp(argv[i], L"--analyse") == 0 || wcscmp(argv[i], L"-a") == 0) {
            if (i + 1 < argc) {
                options->mode = (int)operatingmode_t::ANALYSE;
                options->analyse = 1;
                options->input_file = argv[++i];
                options->input_file_set = 1;
            }
            else {
                COUTERR("Error: Missing input file argument for \"%s\" - %ls\n", argv[i], L"analyse");
                return -6;
            }
        }
#ifdef _DEBUG
        else if (wcscmp(argv[i], L"--zdebug") == 0 || wcscmp(argv[i], L"-z") == 0) {
            options->debug = 1;
            gLogsEnabled = true;
        }
#endif
        else if (wcscmp(argv[i], L"--force") == 0 || wcscmp(argv[i], L"-f") == 0) {
            options->force = 1;
        }
        else if (wcscmp(argv[i], L"--password") == 0 || wcscmp(argv[i], L"-p") == 0) {
            if (i + 1 < argc) {
                options->password = argv[++i];
            }else {
                COUTERR("Error: Missing password argument\n");
                return -6;
            }
        }
        else if (wcscmp(argv[i], L"--output") == 0 || wcscmp(argv[i], L"-o") == 0) {
            if (i + 1 < argc) {
                options->output_file = argv[++i];
                options->output_file_set = 1;
            }
            else {
                COUTERR("Error: Missing output file path argument\n");
                return -5;
            }
        }
        else if (wcscmp(argv[i], L"--cert") == 0 || wcscmp(argv[i], L"-c") == 0) {
            if (i + 1 < argc) {
                options->cert_path = argv[++i];
            }
            else {
                COUTERR("Error: Missing certificate path argument\n");
                return -4;
            }
        }
        else if (wcscmp(argv[i], L"--key-file") == 0 || wcscmp(argv[i], L"-k") == 0) {
            if (i + 1 < argc) {
                options->key_file = argv[++i];
            }
            else {
                COUTERR("Error: Missing key file path argument\n");
                return -3;
            }
        }
        else if (wcscmp(argv[i], L"--generate") == 0 || wcscmp(argv[i], L"-g") == 0) {
            if (i + 1 < argc) {
                options->generate_path = argv[++i];
            }
            else {
                COUTERR("Error: Missing generate path argument\n");
                return -2;
            }
        }
        else if (wcscmp(argv[i], L"--help") == 0 || wcscmp(argv[i], L"-h") == 0) {
            show_usage();
            return -1;
        }
        else {

        }
    }

    // Ensure exactly one operation is selected
    if (options->encrypt + options->decrypt + options->analyse != 1) {
        COUTERR("Error: You must specify exactly one operation (--encrypt, --decrypt, or --analyse)\n");
        show_usage();
        return -1;
    }

    // Check if a file was provided
    if (!options->input_file || !options->input_file_set){
        COUTERR("Error: Missing file argument\n");
        show_usage();
        return -1;
    }

    return 0;
}

bool delete_file_if_exists(const std::string& file_name) {
    // Check if the file exists using ifstream
    std::ifstream file(file_name);
    if (file.good()) {
        file.close(); // Close the file after checking existence

        // Attempt to delete the file
        if (std::remove(file_name.c_str()) == 0) {
            std::cout << "File '" << file_name << "' deleted successfully." << std::endl;
            return true;  // File deleted successfully
        }
        else {
            std::cerr << "Error: Unable to delete file '" << file_name << "'." << std::endl;
            return false; // File deletion failed
        }
    }
    else {
        std::cerr << "Error: File '" << file_name << "' does not exist." << std::endl;
        return false; // File does not exist
    }
}

// Debug function to dump the options structure
void dump_cmd_options(const aescrypter_options* options) {
    CTITLE("DEBUG - COMMAND LINE OPTIONS");

    // Print the operation selected
    if (options->encrypt) {
        //wprintf(L"Operation: Encrypt\n");
        COPTIONH("Operation", "Encrypt");
    }
    else if (options->decrypt) {
       // wprintf(L"Operation: Decrypt\n");
        COPTIONH("Operation", "Decrypt");
    }
    else if (options->analyse) {
       // wprintf(L"Operation: Analyse\n");
        COPTIONH("Operation", "Analyse");
    }

    // Print the file being processed
    if (options->input_file_set) {
        //wprintf(L"File: %ls\n", options->file);
        COPTIONW("input_file", options->input_file);
    }
    else {
        COPTION("input_file" ,"None");

    }
    if (options->output_file_set) {
        //wprintf(L"File: %ls\n", options->file);
        COPTIONW("output_file", options->output_file);
    }
    else {
        COPTION("output_file", "not set, will be detected");

    }
    // Print other options if provided
    if (options->password) {
        COPTIONW("password", options->password);
    }
    if (options->cert_path) {
       // wprintf(L"Certificate Path: %ls\n", options->cert_path);
        COPTIONW("certificate", options->cert_path);
    }
    if (options->key_file) {
        COPTIONW("key_file", options->key_file);
    }
    if (options->generate_path) {
 
        ConsoleCmdOptionW("generate", options->generate_path);
    }

    wprintf(L"----------------------\n\n");


}

// Main function
int wmain(int argc, wchar_t* argv[]) {
    aescrypter_options options;
    int rc = 0, passlen = 0;
    FILE* infp = NULL;
    FILE* outfp = NULL;
    operatingmode_t mode = UNINIT;
    encryptingmode_t encrypt_mode = encryptingmode_t::NONE;
    wchar_t* infile = NULL, pass[MAX_PASSWD_LEN + 1];
    int file_count = 0;
    int output_file_len = 0;
    wchar_t output_file[MAX_FILENAME_LEN + 1];
    wmemset(output_file, 0, MAX_FILENAME_LEN + 1);
    wmemset(pass, 0, MAX_PASSWD_LEN + 1);
#ifdef _DEBUG
    gLogsEnabled = false;
#else 
    gLogsEnabled = false;
#endif

    show_banner();

    int result = parse_arguments(argc, argv, &options);
    if (result != 0) return result;
#ifdef _DEBUG
    // Dump the parsed options (for debugging)
    if (options.debug) {
        dump_cmd_options(&options);
    }
#endif
    if (!options.input_file_set) {
        COUTERR("Error: Missing file\n");
        return -1;
    }

    // Check if the input file exists
    if (!file_exists(options.input_file)) {
        COUTERR("Error: Input file %s does not exist.", options.input_file);
        return -7;
    }
    if (!options.output_file_set) {
        std::wstring output_str = generate_output_filename(options.input_file, options.mode);
        COUTCMD("Generate the output file name based on the mode (%d) %ls \n", options.mode, output_str.c_str());
        if (file_exists(output_str)) {
            std::wstring other_output_str = output_str + L".2";
            output_str = generate_output_filename(other_output_str, options.mode);
        }
        if (output_str.length() > MAX_FILENAME_LEN) {
            COUTERR("Error: invalid file length %d", output_str.length());
            return -11;
        }
        wcscpy(output_file, output_str.c_str());
    }
    else {
        wcscpy(output_file, options.output_file);
        output_file_len = (int)wcslen(output_file);

        if (file_exists(output_file)) {
            if (options.force) {
                COUTWARN("Output file %s already exist.", output_file);

                std::wstring output_wstr = output_file;
                std::string output_str(output_wstr.begin(), output_wstr.end());
                COUTCMD("delete_file_if_exists: %ls\n", output_str.c_str());
                delete_file_if_exists(output_str);
            }
            else {
                COUTERR("Output file % s already exist.", output_file);
                return -12;
            }
        }
        
    }
    

    if ((options.password && wcslen(options.password) < MIN_PASSWD_LEN)) {
        COUTERR("Error: invalid password length %d. minimum is", wcslen(options.password), MIN_PASSWD_LEN);
        return -11;
    }
    
    if (!options.password || (options.password && wcslen(options.password) == 0)) {
        // Prompt for password if not provided on the command line
        passlen = read_password(pass, mode);
        encrypt_mode = encryptingmode_t::PASSWORD;
        COUTCMD("read_password(%d) returns %d", mode, passlen)
        switch (passlen)
        {
        case 0: //no password in input
            COUTERR( "Error: No password supplied.\n");
            cleanup(output_file);
            return -1;
        case AESCRYPT_READPWD_GETWCHAR:
        case AESCRYPT_READPWD_TOOSHORT:
        case AESCRYPT_READPWD_TOOLONG:
            COUTERR( "Error in read_password: %ls.\n",
                read_password_error(passlen));
            cleanup(output_file);
            return -1;
        case AESCRYPT_READPWD_NOMATCH:
            COUTERR( "Error: Passwords don't match.\n");
            cleanup(output_file);
            return -1;
        }

        if (passlen < 0)
        {
            cleanup(output_file);
            // For security reasons, erase the password
            wmemset(pass, 0, passlen);
            return -1;
        }
    }
    else {
        wcscpy(pass, options.password);
        passlen = (int)wcslen(options.password);

        COUTCMD("Using password: %ls\n", options.password);

    }

    if ((infp = _wfopen(options.input_file, L"rb")) == NULL)
    {
        if ((outfp != stdout) && (outfp != NULL))
        {
            fclose(outfp);
        }
        fwprintf(stderr, L"Error opening input file %s : ", options.input_file);
        perror("");
        cleanup(output_file);
        // For security reasons, erase the password
        wmemset(pass, 0, passlen);
        return  -1;
    }

    // Perform the selected operation
    switch (options.mode)
    {
    case operatingmode_t::DECRYPT:
        if (options.key_file) {
            COUTCMD("Using key file: %ls\n", options.key_file);
        }

        if ((outfp = _wfopen(output_file, L"wb")) == NULL)
        {
            if ((infp != stdin) && (infp != NULL))
            {
                fclose(infp);
            }
            fwprintf(stderr, L"Error opening output file %s : ", output_file);
            perror("");
            cleanup(output_file);
            // For security reasons, erase the password
            wmemset(pass, 0, passlen);
            return  -1;
        }


        switch (encrypt_mode) {
        case encryptingmode_t::PASSWORD:
            COUTCMD("decrypt_stream() - encrypt file \"%ls\" with password %ls, len %d\n", output_file, pass, passlen);
            rc = decrypt_stream(infp, outfp, pass, passlen);
            break;
        case encryptingmode_t::KEYFILE:
            COUTCMD("Using key file: %ls\n", options.key_file);
            COUTERR("Error: Not Implemented");
            encrypt_mode = encryptingmode_t::KEYFILE;
            break;
        case encryptingmode_t::CERTIFICATE:
            COUTCMD("Generating key and saving to: %ls\n", options.generate_path);
            encrypt_mode = encryptingmode_t::CERTIFICATE;
            COUTERR("Error: Not Implemented");
            break;
        case encryptingmode_t::SYSTEM:
            break;
        default:
            COUTERR("Error: Not Implemented");
            return -1;
            break;
        }
        break;
    case operatingmode_t::ENCRYPT:
        COUTCMD("Encrypting file: %ls\n", options.input_file);

        if (outfp == NULL)
        {
            if ((outfp = _wfopen(output_file, L"wb")) == NULL)
            {
                if ((infp != stdin) && (infp != NULL))
                {
                    fclose(infp);
                }
                fwprintf(stderr, L"Error opening output file %s : ", output_file);
                perror("");
                cleanup(output_file);
                // For security reasons, erase the password
                wmemset(pass, 0, passlen);
                return  -1;
            }
        }

        switch (encrypt_mode) {
        case encryptingmode_t::PASSWORD:
            COUTCMD("encrypt_stream() - encrypt file \"%ls\" with password %ls, len %d\n", output_file, pass, passlen);
            rc = encrypt_stream(infp, outfp, pass, passlen);
            break;
        case encryptingmode_t::KEYFILE:
            COUTCMD("Using key file: %ls\n", options.key_file);
            COUTERR("Error: Not Implemented");
            encrypt_mode = encryptingmode_t::KEYFILE;
            break;
        case encryptingmode_t::CERTIFICATE:
            COUTCMD("Generating key and saving to: %ls\n", options.generate_path);
            encrypt_mode = encryptingmode_t::CERTIFICATE;
            COUTERR("Error: Not Implemented");
            break;
        case encryptingmode_t::SYSTEM:
            break;
        default:
            COUTERR("Error: Not Implemented");
            return -1;
            break;
        }
        break;
    case operatingmode_t::ANALYSE:
        COUTCMD("Analysing file: %ls\n", options.input_file);
        COUTERR("Error: Not Implemented");
        break;
    }

    // Reset input/output file names and desriptors
    output_file[0] = '\0';
    infp = NULL;
    outfp = NULL;
    return rc;
}

// Function to check if the input file exists
bool file_exists(const std::wstring& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Function to generate the output filename based on the mode and input file
std::wstring generate_output_filename(const std::wstring& input_file, int mode) {
    std::wstring output_file = input_file;

    // Check if the mode is encryption
    if (mode == (int)operatingmode_t::ENCRYPT) {
        // Append ".aes" to the input filename for encryption
        output_file += L".aes";
    }
    // Check if the mode is decryption
    else if (mode == (int)operatingmode_t::DECRYPT) {
        // Remove the ".aes" extension if present
        size_t pos = output_file.find(L".aes");
        if (pos != std::wstring::npos) {
            output_file.erase(pos, 4); // Remove ".aes"
        }
    }

    return output_file;
}

/*
 *  version
 *
 *  Displays the program version to the user.
 */
void show_version()
{
    wprintf(L"\n%s version %s (%s)\n\n", APP_ExECUTABLE, LPROG_VERSION, LPROG_DATE);
}


/*
 *  cleanup
 *
 *  Removes output files that are not fully and properly created.
 */
void cleanup(const wchar_t* outfile)
{
    if (wcscmp(outfile, L"-") && outfile[0] != '\0')
    {
        _wunlink(outfile);
    }
}
