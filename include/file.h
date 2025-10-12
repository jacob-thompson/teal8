#include <stdbool.h>
#include <sys/stat.h>

#include <curl/curl.h>

#include "../include/cJSON.h"

#define SHA1_BLOCK_SIZE 20
#define SHA1_HASH_SIZE 41

/** Struct to hold data for curl callback
*/
struct MemoryStruct {
    char *memory;
    size_t size;
};

/** Callback function for curl to write data to memory
 * @param contents pointer to the data
 * @param size size of each data element
 * @param nmemb number of data elements
 * @param userp pointer to the MemoryStruct
 * @return number of bytes written
 */
static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

/** Pull data from a URL into a MemoryStruct using curl
 * @param handle CURL handle
 * @param chunk pointer to MemoryStruct to hold the data
 * @param url URL to fetch data from
 */
int pullDatabase(CURL *handle, struct MemoryStruct *chunk, const char *url);

/** Get the SHA1 hash of a file
 * @param fp file pointer to the file
 * @return pointer to the hash string (must be freed by caller)
 */
char *getHash(FILE *fp);

/** Print program information from a cJSON object
 * @param program_info cJSON object containing program information
 * @param romHash cJSON object containing ROM hash information
 */
void printProgramInfo(cJSON *program_info, cJSON *romHash);

/** Check if a ROM file is in the database
 * @param fp file pointer to the ROM file
 * @return true if the ROM is in the database, false otherwise
 */
bool isRomInDatabase(FILE *fp);

/**
    * Check if a file is valid.
    * A file is valid if it exists and is not a directory.
    * Handles logging of errors.
    * @param fp the file pointer
    * @param st the stat struct
    * @return 0 if the file is valid, 1 otherwise
*/
bool isFileValid(const char *filename, FILE *fp, struct stat *st);
