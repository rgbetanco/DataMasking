#include <assert.h>
#include <sodium.h>
#include <string.h>

#include "common_chinese.h"

unsigned int rand_from_zero_to_max(unsigned int max)
{
    char myString[32];
    uint32_t myInt;

    if (sodium_init() < 0)
    {
        printf("panic! the Sodium library couldn't be initialized, it is not safe to use\n");
        return 0;
    }

    // myString will be an array of 32 random bytes, not null-terminated
    randombytes_buf(myString, 32);

    // myInt will be a random number between 0 and RAND_MAX
    myInt = randombytes_uniform(RAND_MAX);
    return myInt % max;
}

int unicode_to_utf8(unsigned long unic, char *pOutput, int outSize)
{
    assert(pOutput != NULL);
    assert(outSize >= 6);

    if (unic <= 0x0000007F)
    {
        // * U-00000000 - U-0000007F:  0xxxxxxx
        *pOutput = (unic & 0x7F);
        return 1;
    }
    else if (unic >= 0x00000080 && unic <= 0x000007FF)
    {
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        *(pOutput + 1) = (unic & 0x3F) | 0x80;
        *pOutput = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    }
    else if (unic >= 0x00000800 && unic <= 0x0000FFFF)
    {
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        *(pOutput + 2) = (unic & 0x3F) | 0x80;
        *(pOutput + 1) = ((unic >> 6) & 0x3F) | 0x80;
        *pOutput = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else if (unic >= 0x00010000 && unic <= 0x001FFFFF)
    {
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput + 3) = (unic & 0x3F) | 0x80;
        *(pOutput + 2) = ((unic >> 6) & 0x3F) | 0x80;
        *(pOutput + 1) = ((unic >> 12) & 0x3F) | 0x80;
        *pOutput = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    }
    else if (unic >= 0x00200000 && unic <= 0x03FFFFFF)
    {
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput + 4) = (unic & 0x3F) | 0x80;
        *(pOutput + 3) = ((unic >> 6) & 0x3F) | 0x80;
        *(pOutput + 2) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput + 1) = ((unic >> 18) & 0x3F) | 0x80;
        *pOutput = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    }
    else if (unic >= 0x04000000 && unic <= 0x7FFFFFFF)
    {
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput + 5) = (unic & 0x3F) | 0x80;
        *(pOutput + 4) = ((unic >> 6) & 0x3F) | 0x80;
        *(pOutput + 3) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput + 2) = ((unic >> 18) & 0x3F) | 0x80;
        *(pOutput + 1) = ((unic >> 24) & 0x3F) | 0x80;
        *pOutput = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}

void gen_trad_chiniese_text(char* output, const unsigned int cnt)
{
    size_t buffer_len = 100;
    char* buffer = (char*)malloc(buffer_len);
    for (unsigned int i = 0; i < cnt; i++)
    {
        memset(buffer, 0, buffer_len);
        unsigned int idx = rand_from_zero_to_max(4808);
        unicode_to_utf8(common_chinese[idx], buffer, buffer_len);
        strcat(output, buffer);
    }
    free(buffer);
}

// gcc chinese_generator.c -lsodium -o chinese_generator.out
#if debug
#define UTF8_LONGEST_SIZE 6

int main()
{
    printf("---Testing random generate common chinese text---\n");
    int cnt = 10;
    char* output = (char*)malloc((cnt + 1) * UTF8_LONGEST_SIZE);
    gen_trad_chiniese_text(output, cnt);
    printf("%s\n", output);
    free(output);
    return 0;
}
#endif