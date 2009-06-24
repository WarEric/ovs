/*
 * Copyright (c) 2009 Nicira Networks.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <config.h>
#include "shash.h"
#include <assert.h>
#include "hash.h"

static size_t
hash_name(const char *name)
{
    return hash_string(name, 0);
}

void
shash_init(struct shash *sh)
{
    hmap_init(&sh->map);
}

void
shash_destroy(struct shash *sh)
{
    if (sh) {
        shash_clear(sh);
    }
}

void
shash_clear(struct shash *sh)
{
    struct shash_node *node, *next;

    HMAP_FOR_EACH_SAFE (node, next, struct shash_node, node, &sh->map) {
        hmap_remove(&sh->map, &node->node);
        free(node->name);
        free(node);
    }
}

bool
shash_is_empty(const struct shash *shash)
{
    return hmap_is_empty(&shash->map);
}

/* It is the caller's responsible to avoid duplicate names, if that is
 * desirable. */
void
shash_add(struct shash *sh, const char *name, void *data)
{
    struct shash_node *node = xmalloc(sizeof *node);
    node->name = xstrdup(name);
    node->data = data;
    hmap_insert(&sh->map, &node->node, hash_name(name));
}

void
shash_delete(struct shash *sh, struct shash_node *node)
{
    hmap_remove(&sh->map, &node->node);
    free(node->name);
    free(node);
}

/* If there are duplicates, returns a random element. */
struct shash_node *
shash_find(const struct shash *sh, const char *name)
{
    struct shash_node *node;

    HMAP_FOR_EACH_WITH_HASH (node, struct shash_node, node,
                             hash_name(name), &sh->map) {
        if (!strcmp(node->name, name)) {
            return node;
        }
    }
    return NULL;
}

void *
shash_find_data(const struct shash *sh, const char *name)
{
    struct shash_node *node = shash_find(sh, name);
    return node ? node->data : NULL;
}
