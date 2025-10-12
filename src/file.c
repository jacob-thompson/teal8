#include <unistd.h>

#include <SDL_log.h>
#include <openssl/evp.h>

#include "../include/file.h"

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "not enough memory (realloc returned NULL)\n"
        );
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int pullDatabase(CURL *handle, struct MemoryStruct *chunk, const char *url)
{
    CURLcode res;

    // set URL to fetch
    curl_easy_setopt(handle, CURLOPT_URL, url);

    // send all data to this function
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);

    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)chunk);

    // some servers don't like requests that are made without a user-agent field
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // perform the request, res will get the return code
    res = curl_easy_perform(handle);

    // check for errors
    if (res != CURLE_OK) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "curl query failed: %s\n",
            curl_easy_strerror(res)
        );
        return -1;
    }

    return 0;
}

char *getHash(FILE *fp)
{
    EVP_MD_CTX *shaContext = EVP_MD_CTX_new();
    if (shaContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create SHA1 context\n");
        return NULL;
    }

    if (EVP_DigestInit_ex(shaContext, EVP_sha1(), NULL) != 1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to initialize SHA1 context\n");
        EVP_MD_CTX_free(shaContext);
        return NULL;
    }

    // compute SHA1 hash of the ROM
    unsigned char buffer[20]; // SHA1 produces a 20-byte hash
    size_t bytesRead = 0;

    // read the ROM file in chunks and update the SHA1 context
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        EVP_DigestUpdate(shaContext, buffer, bytesRead);
    }

    unsigned char hash[SHA1_BLOCK_SIZE];
    EVP_DigestFinal_ex(shaContext, hash, NULL);
    EVP_MD_CTX_free(shaContext);
    rewind(fp); // reset file pointer to the beginning of the file

    /*
    printf("SHA1 hash: ");
    for (int i = 0; i < SHA1_BLOCK_SIZE; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
    */

    // convert hash to hex string
    char *hashString = malloc(SHA1_HASH_SIZE * sizeof(char));
    for (int i = 0; i < SHA1_BLOCK_SIZE; i++) {
        sprintf(&hashString[i << 1], "%02x", hash[i]);
    }
    hashString[SHA1_HASH_SIZE - 1] = '\0';

    return hashString;
}

void printProgramInfo(cJSON *program_info, cJSON *romHash)
{
    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "Program info: %s\n",
        cJSON_Print(program_info)
    );

    cJSON *title = cJSON_GetObjectItemCaseSensitive(program_info, "title");
    cJSON *release = cJSON_GetObjectItemCaseSensitive(program_info, "release");
    SDL_LogInfo(
        SDL_LOG_CATEGORY_APPLICATION,
        "Title: %s (%s)\n",
        title ? title->valuestring : "Title Unknown",
        release ? release->valuestring : "Release Date Unknown"
    );
    cJSON_ArrayForEach(romHash, cJSON_GetObjectItemCaseSensitive(program_info, "authors")) {
        if (cJSON_IsString(romHash) && (romHash->valuestring != NULL)) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Author: %s\n", romHash->valuestring);
        }
    }
    cJSON *description = cJSON_GetObjectItemCaseSensitive(program_info, "description");
    SDL_LogInfo(
        SDL_LOG_CATEGORY_APPLICATION,
        "Description: %s\n",
        description ? description->valuestring : "No Description Available"
    );
}

bool isRomInDatabase(FILE *fp)
{
    struct MemoryStruct hashChunk, infoChunk;

    // will be reallocated as needed
    hashChunk.memory = malloc(1);
    infoChunk.memory = malloc(1);

    // initial size is 0
    hashChunk.size = 0;
    infoChunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle;
    curl_handle = curl_easy_init();

    if (curl_handle == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to initialize curl\n");
        return false;
    }

    // pull the SHA1 hash database
    if (pullDatabase(
        curl_handle,
        &hashChunk,
        "https://raw.githubusercontent.com/chip-8/chip-8-database/refs/heads/master/database/sha1-hashes.json"
    ) != 0) {
        curl_easy_cleanup(curl_handle);
        return false;
    }

    cJSON *hashJson = cJSON_Parse(hashChunk.memory);
    if (hashJson == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "failed to parse json: %s\n",
                error_ptr
            );
        }
        curl_easy_cleanup(curl_handle);
        return false;
    }

    /*
    const char *str = cJSON_Print(hashJson);
    if (str == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to print json\n");
        cJSON_Delete(hashJson);
        curl_easy_cleanup(curl_handle);
        return false;
    }
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "JSON data: %s\n", str);
    free((void *)str);
    */

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%lu bytes retrieved from hash database\n", (unsigned long)hashChunk.size);

    char *hashString = getHash(fp);

    cJSON *romHash = cJSON_GetObjectItemCaseSensitive(hashJson, hashString);
    if (cJSON_IsNull(romHash) || romHash == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "rom hash not found in database: %s\n",
            hashString
        );
        free(hashString);
        free(hashChunk.memory);
        cJSON_Delete(hashJson);
        curl_easy_cleanup(curl_handle);
        return false;
    } else {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "rom hash found in database: %s\n",
            cJSON_Print(romHash)
        );
    }

    // pull the program info database
    if (pullDatabase(
        curl_handle,
        &infoChunk,
        "https://raw.githubusercontent.com/chip-8/chip-8-database/refs/heads/master/database/programs.json"
    ) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to pull program info database\n");
        free(hashString);
        free(hashChunk.memory);
        cJSON_Delete(hashJson);
        curl_easy_cleanup(curl_handle);
        return false;
    }

    cJSON *infoJson = cJSON_Parse(infoChunk.memory);
    if (infoJson == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to parse json: %s\n",
                         error_ptr);
        }
        free(hashString);
        free(hashChunk.memory);
        cJSON_Delete(hashJson);
        curl_easy_cleanup(curl_handle);
        return false;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%lu bytes retrieved from info database\n", (unsigned long)infoChunk.size);

    // get program info using the index found earlier
    int index = romHash->valueint;
    cJSON *program_info = cJSON_GetArrayItem(infoJson, index);
    if (program_info == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to retrieve program info from database\n"
        );
    } else {
        printProgramInfo(program_info, romHash);
    }

    // cleanup
    free(hashString);
    free(hashChunk.memory);
    free(infoChunk.memory);
    cJSON_Delete(hashJson);
    cJSON_Delete(infoJson);
    curl_easy_cleanup(curl_handle);

    return true;
}

bool isFileValid(const char *filename, FILE *fp, struct stat *st)
{
    if (fp == NULL || fstat(fileno(fp), st) == -1) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to open %s\n",
            filename
        );
        return false;
    }

    if (!isRomInDatabase(fp)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "%s not found in database\n",
            filename
        );
        return false;
    }

    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "%s opened successfully\n",
        filename
    );
    return true;
}
