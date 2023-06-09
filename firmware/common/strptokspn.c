/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2022 by William WIlgus
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied. *
 ****************************************************************************/

#include "config.h"

#include <stddef.h>
#include <string.h>
#include "strtok_r.h"
/* strptokspn_r is a custom implementation of strtok_r that does NOT modify 
 *  the source string.
 *
 * strptokspn_r reads ptr as a series of zero or more tokens,
 *  and sep as delimiters of the tokens
 * The tokens can be separated by one or more of the delimiters
 * first call searches for the first token skipping over any leading delimiters.
 * Returns pointer to first token
 * Pointer *len contains the span to the first delimeter
 *  (this would be the resulting strlen had token actually been NULL terminated)
 * Pointer **end contains pointer to first character after the last delimeter
 *
 * When strptokspn_r is called with a ptr == NULL, the next token is read from
 * Pointer **end
 *
 * Note the returned token is NOT NULL terminated by the function as in strtok_r
 * However the caller can use ret[len] = '\0'; to emulate a call to strtok_r
*/
const char *strptokspn_r(const char *ptr, const char *sep, size_t *len, const char **end)
{
    if (ptr == NULL) /* we got NULL input so then we get last position instead */
    {
        ptr = *end;
    }

    /* pass all letters that are including in the separator string */
    while (*ptr && strchr(sep, *ptr))
        ++ptr;

    if (*ptr != '\0')
    {
        /* so this is where the next piece of string starts */
        const char *start = ptr;
        *len = strcspn(ptr, sep); /* Get span until any sep character in string */
        *end = ptr + *len;
        if (**end) /* the end is not a null byte */
            ++*end;
        return start;
    }
    return NULL;
}

#if !defined(HAVE_STRTOK_R)
char * strtok_r(char *ptr, const char *sep, char **end)
{
    size_t len;
    char * ret = (char*) strptokspn_r((const char*)ptr, sep, &len, (const char**) end);
    if (ret)
        ret[len] = '\0';
    return ret;
}
#endif
