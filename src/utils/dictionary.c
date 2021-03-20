/*******************************************************************************
 * Copyright (c) 2013-2017, Andrés Martinelli <andmarti@gmail.com              *
 * All rights reserved.                                                        *
 *                                                                             *
 * This file is a part of SC-IM                                                *
 *                                                                             *
 * SC-IM is a spreadsheet program that is based on SC. The original authors    *
 * of SC are James Gosling and Mark Weiser, and mods were later added by       *
 * Chuck Martin.                                                               *
 *                                                                             *
 * Redistribution and use in source and binary forms, with or without          *
 * modification, are permitted provided that the following conditions are met: *
 * 1. Redistributions of source code must retain the above copyright           *
 *    notice, this list of conditions and the following disclaimer.            *
 * 2. Redistributions in binary form must reproduce the above copyright        *
 *    notice, this list of conditions and the following disclaimer in the      *
 *    documentation and/or other materials provided with the distribution.     *
 * 3. All advertising materials mentioning features or use of this software    *
 *    must display the following acknowledgement:                              *
 *    This product includes software developed by Andrés Martinelli            *
 *    <andmarti@gmail.com>.                                                    *
 * 4. Neither the name of the Andrés Martinelli nor the                        *
 *   names of other contributors may be used to endorse or promote products    *
 *   derived from this software without specific prior written permission.     *
 *                                                                             *
 * THIS SOFTWARE IS PROVIDED BY ANDRES MARTINELLI ''AS IS'' AND ANY            *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE      *
 * DISCLAIMED. IN NO EVENT SHALL ANDRES MARTINELLI BE LIABLE FOR ANY           *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES  *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;*
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.           *
 *******************************************************************************/

/**
 * \file dictionary.c
 * \author Andrés Martinelli <andmarti@gmail.com>
 * \date 2017-07-18
 * \brief Dictionary implementation using malloc
 */

#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

/**
 * \brief TODO Document create_dictionary()
 *
 * \return dictionary
 */

struct dictionary * create_dictionary() {
   struct dictionary * d = malloc(sizeof(struct dictionary));
   d->len = 0;
   d->list = NULL;

   return d;
}

/**
 * \brief TODO Document put()
 *
 * \param[in] d
 * \param[in] k
 * \param[in] v
 *
 * \return none
 */

void put(struct dictionary * d, char * k, char * v) {
   struct nlist *nl, **p_nl;

   if (*k == 0) return;

   // locate the key position
   for (p_nl = &d->list; *p_nl; p_nl = &(*p_nl)->next) {
      nl = *p_nl;
      int cmp = strcmp(k, nl->key);
      if (cmp > 0) continue;
      if (cmp < 0) break;

      // Duplicated keys are not allowed.
      // If an existing key is inserted, the value is overwritten.
      free(nl->val);
      nl->val = strdup(v);
      nl->intval = atoi(v);
      return;
   }

   // The key doesn't exists, Create it.
   nl = malloc(sizeof(struct nlist));
   nl->key = strdup(k);
   nl->val = strdup(v);
   nl->intval = atoi(v);
   nl->next = *p_nl;
   *p_nl = nl;
   d->len++;
}

/**
 * \brief TODO Document destroy_dictionary()
 *
 * \param[in] d
 *
 * \return none
 */

void destroy_dictionary(struct dictionary * d) {
   struct nlist * nl;
   struct nlist * n_next;

   nl = d->list;
   while (nl != NULL) {
       n_next = nl->next;
       free(nl->key);
       free(nl->val);
       free(nl);
       nl = n_next;
   }

   free(d);
   return;
}

/**
 * \brief Get the size in bytes needed to export a dictionary
 *
 * \param[in] d
 *
 * \return count
 */

int get_dict_buffer_size(struct dictionary * d) {
   struct nlist * nl;
   int count = 0;

   for (nl = d->list; nl != NULL; nl = nl->next) {
      /* <key> + '=' + <val> + '\n' */
      count += strlen(nl->key) + 1 + strlen(nl->val) + 1;
   }
   return count;
}

/**
 * \brief Get the string value for KEY
 *
 * \param[in] d
 * \param[in] key
 *
 * \return value for the key
 */

char * get(struct dictionary * d, char * key) {
   struct nlist * nl;

   for (nl = d->list; nl != NULL; nl = nl->next) {
      int cmp = strcmp(key, nl->key);
      if (cmp > 0) continue;
      if (cmp < 0) break;
      return nl->val;
   }
   return NULL;
}

/**
 * \brief Get the integer value for KEY
 *
 * \param[in] d
 * \param[in] key
 *
 * \return value for the key
 */

int get_int(struct dictionary * d, char * key) {
   struct nlist * nl;

   for (nl = d->list; nl != NULL; nl = nl->next) {
      int cmp = strcmp(key, nl->key);
      if (cmp > 0) continue;
      if (cmp < 0) break;
      return nl->intval;
   }
   return 0;
}

/* Get the key name from a value
char * get_key_name(struct dictionary * d, char * value) {
   struct nlist * nl;

   for (nl = d->list; nl != NULL; nl = nl->next) {
       if (! strcmp(nl->val, value))
           return nl->key;
   }
   return NULL;
}
*/


/**
 * \brief Save key/value pairs in D dictionary from a string STR
 *
 * \param[in] d
 * \param[in] str
 * \param[in] blank_space
 *
 * \return dictionary
 */

void parse_str(struct dictionary * d, char * str, int no_blanks) {
    char key[90];
    char value[90];
    int i;

    i = 0;
    while (*str != 0 && *str != '=' && *str != '\n' && i < sizeof(key)) {
       key[i++] = *str++;
    }
    if (i >= sizeof(key) || *str++ != '=') return;
    key[i] = 0;

    i = 0;
    while (*str != 0 && *str != '\n' && i < sizeof(value)
	   && !(no_blanks && *str == ' ')) {
       value[i++] = *str++;
    }
    if (i >= sizeof(value)) return;
    value[i] = 0;

    // Create the dictionary
    put(d, key, value);
}
