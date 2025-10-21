#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>

#include <curl/curl.h>

#include "../include/cJSON.h"

/* MemoryStruct holds data for curl callback */
struct MemoryStruct {
    char *memory;                   // pointer to the data
    size_t size;                    // size of the data in bytes
};

/*
 * Callback function for curl to write data to memory.
 * Parameter: pointer to the data
 * Parameter: size of each data element
 * Parameter: number of data elements
 * Parameter: pointer to the MemoryStruct
 * Return: number of bytes written
 */
static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

/*
 * Pull data from a URL into a MemoryStruct.
 * Parameter: CURL handle
 * Parameter: pointer to MemoryStruct to hold the data
 * Parameter: URL to fetch data from
 * Return: 0 on success
 * Return: 1 on failure
 */
int pullDatabase(CURL *handle, struct MemoryStruct *chunk, const char *url);

/*
 * Get the SHA1 hash of a file.
 * Parameter: the file
 * Return: pointer to the hash string (must be freed by caller)
 */
char *getHash(FILE *fp);

/*
 * Print ROM information from a cJSON object.
 * Parameter: cJSON object containing information about the ROM
 * Parameter: cJSON object containing the ROM hash
 */
void printRomInfo(cJSON *romInfo, cJSON *romHash);

/*
 * Check if a ROM file is in the database.
 * Parameter: file pointer to the ROM file
 * Return: true if the ROM is in the database
 * Return: false if the ROM is not in the database
 */
SDL_bool isRomInDatabase(FILE *fp);

/*
 * Check if a file is valid.
 * A file is valid if it exists and is in the database.
 * Handles logging of errors.
 * Parameter: the name of the file
 * Parameter: the file pointer
 * Parameter: the stat struct
 * Return: 0 if the file is valid
 * Return: 1 if the file is not valid
 */
SDL_bool isFileValid(const char *filename, FILE *fp, struct stat *st);

#endif /* FILE_H */
