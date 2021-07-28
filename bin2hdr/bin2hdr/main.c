/*
bin2hdr - Generate a C header file containing the given input file as an array of chars (binary data).

Notes:
 - Regular fseek/ftell was not used due to: https://wiki.sei.cmu.edu/confluence/display/c/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file
 - Windows only (for now).
*/

#include <stdio.h>
#include <stdlib.h>

long  getFileSize(const char* filepath);
long  readBinaryData(const char* filepath, char** buffer);
char* makeOutFileName(const char* inFileName);
FILE* openOutputFile(const char* name);

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        printf("usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* inFileName = argv[1];
    char* outFileName = makeOutFileName(inFileName);
    printf("Input file\t'%s'\nOutput file\t'%s'\n", inFileName, outFileName);

    char* inBinData = NULL;
    long inBinDataLen = readBinaryData(inFileName, &inBinData);
    FILE* outFile = openOutputFile(outFileName);

    /* start header document */
    /* * * * * * * * * * * * */

    /* include guards */
    const char* guardName = "BIN2HDR_INCL_H";
    fprintf(outFile, "#ifndef %s\n#define %s\n", guardName, guardName);

    /* binary data name */
    const char* dataName = "BIN_DATA";
    fprintf(outFile, "const char* %s [%ld] = {\n", dataName, inBinDataLen);

    /* the binary array */
    int valuesPerLine = 32;
    for (long i = 0; i < inBinDataLen; i++)
    {
        fprintf(outFile, "0x%02X", inBinData[i] && 0xFF);

        /* new line or separator */
        if ((i + 1) % valuesPerLine == 0) /* new line every N elements(increment i by one since 0 % N is always 0) */
        {
            fprintf(outFile, ",\n");
        }
        else 
        {
            fprintf(outFile, ", ");
        }
    }

    /* close array and ifdef */
    fprintf(outFile, "};\n");
    fprintf(outFile, "#endif /*%s*/\n", guardName);

    /*  end header document  */
    /* * * * * * * * * * * * */


    fclose(outFile);

    /* dealocate things */
    free(inBinData);
    free(outFileName);

    printf("%ld bytes written.\n", inBinDataLen);
    return EXIT_SUCCESS;
}






#ifdef WIN32

#include <windows.h>
#include <sys/stat.h>	/* _S_IREAD */
#include <fcntl.h>		/* _O_RDONLY */
#include <io.h>		/* _SH_DENYRW */

long getFileSize(const char* filepath) {
    int fd = 0;
    _sopen_s(&fd, filepath, _O_RDONLY, _SH_DENYRW, _S_IREAD);

    if (fd == -1) {
        perror("Failed to open file");

    }

    long len = _filelength(fd); //_filelengthi64 in x64
    _close(fd);

    if (len == -1) {
        perror("Failed to get file size");
        exit(EXIT_FAILURE);
    }

    return len;
}

long readBinaryData(const char* filepath, char** buffer) {

    // get the file size
    long len = getFileSize(filepath);

    // allocate the buffer
    *buffer = (char*)calloc(sizeof(char), len);
    if (*buffer != NULL)
    {
        // open and read the data
        FILE* stream;
        if (fopen_s(&stream, filepath, "rb") == 0)
        {
            long bytes = fread(*buffer, sizeof(char), len, stream); // read all at once
            fclose(stream);

            if (bytes != len) {
                printf("Failed to read all data (read %ld of %ld)", bytes, len);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            perror("Failed to read the input file");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        perror("Failed to allocate memory to read the input file");
        exit(EXIT_FAILURE);
    }

    return len;
}

char* makeOutFileName(const char* inFileName) {
    int inFileNameLen = strlen(inFileName);

    const char* outFileSuffix = ".h";
    int outFileNameLen = inFileNameLen + strlen(outFileSuffix) + 1;
    char* outFileName = (char*)calloc(outFileNameLen, sizeof(char));
    if (NULL == outFileName) {
        perror("Failed to allocate memory for the output file name");
        exit(EXIT_FAILURE);
    }
    strcpy_s(outFileName, outFileNameLen, inFileName);
    strcat_s(outFileName, outFileNameLen, outFileSuffix);

    if (NULL == outFileName) {
        perror("Failed to allocate memory for the output file name");
        exit(EXIT_FAILURE);
    }

    return outFileName;
}

FILE* openOutputFile(const char* name) {
    FILE* ret;
    if (fopen_s(&ret, name, "w") != 0)
    {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    return ret;
}

#endif
