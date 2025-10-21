#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>

#include <curl/curl.h>

#include "../include/cJSON.h"

/* MemoryStruct holds data for curl callback */
struct MemoryStruct {
    char    *memory;    // pointer to the data
    size_t  size;       // size of the data in bytes
};

/*
 * Callback function for curl to write data to memory.
 *
 * Parameters:
 * a pointer to the data,
 * the size of each data element,
 * the number of data elements,
 * a pointer to the MemoryStruct
 *
 * Return:
 * the number of bytes written
 */
static size_t
writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

/*
 * Pull data from a URL into a MemoryStruct.
 *
 * Parameters:
 * the CURL handle
 * a pointer to MemoryStruct to hold the data,
 * the URL to fetch data from
 *
 * Return:
 * 0 on success,
 * 1 on failure
 */
int
pullDatabase(CURL *handle, struct MemoryStruct *chunk, const char *url);

/*
 * Get the SHA1 hash of a ROM.
 *
 * Parameter:
 * the ROM
 *
 * Return:
 * pointer to the hash string (must be freed by caller)
 */
const char *
getHash(FILE *romFile);

/*
 * Print ROM information from a cJSON object.
 *
 * Parameters:
 * cJSON object containing information about the ROM,
 * cJSON object containing the ROM hash
 */
void
printRomInfo(cJSON *romInfo, cJSON *romHash);

/*
 * Check if a ROM file is in the database.
 *
 * Parameter:
 * the ROM file
 *
 * Return:
 * true if the ROM is in the database,
 * false if the ROM is not in the database
 */
SDL_bool
isRomInDatabase(FILE *romFile);

/*
 * Check if a ROM is valid.
 * A ROM is valid if it exists and is in the database.
 * Handles logging of errors.

 * Parameters:
 * the name of the ROM,
 * the ROM,
 * the stat structure
 *
 * Return:
 * 0 if the ROM is valid,
 * 1 if the ROM is not valid
 */
SDL_bool
isRomValid(const char *romName, FILE *romFile, struct stat *st);

#endif /* FILE_H */
