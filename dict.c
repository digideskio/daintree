 // daintree

#include <dict.h>
#include <mem.h>
#include <string.h>
#include <crc32.h>

struct _node;

struct dict {
    dict_hash_f hashF;
    dict_free_f freeF;
    struct _node *root;
};

struct _node {
    uint8_t red;
    dict_hash_t key_hash;
    void *data;
    struct _node *link[2];
};

struct dict *dict_create(dict_hash_f hashF, dict_free_f freeF) {
    struct dict *bt = malloc(sizeof(*bt));
    bt->hashF = hashF;
    bt->freeF = freeF;
    bt->root = NULL;
    return bt;
}

static dict_hash_t _hash(const struct dict *bt, const void *key) {
    if (bt->hashF) {
        return bt->hashF(key);
    }

    return (dict_hash_t)(unsigned long)key;
}

static int _red(struct _node *root) {
    return root && root->red == 1;
}

static struct _node *_single(struct _node *root, int dir) {
    struct _node *save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;

    root->red = 1;
    save->red = 0;

    return save;
}

static struct _node *_double(struct _node *root, int dir) {
    root->link[!dir] = _single(root->link[!dir], !dir);
    return _single(root, dir);
}

static struct _node *_make(dict_hash_t key_hash, void *data) {
    struct _node *n = malloc(sizeof(*n));
    n->red = 1;
    n->key_hash = key_hash;
    n->data = data;
    n->link[0] = NULL;
    n->link[1] = NULL;
    return n;
}

static void *_inssearch(
        struct dict *bt, const void *key, void *data, int replace) {
    /* First, hash the data. */
    dict_hash_t key_hash = _hash(bt, key);

    if (!bt->root) {
        bt->root = _make(key_hash, data);
        bt->root->red = 0;
        return NULL;
    }

    struct _node head = {0, 0, 0, {0, 0}};    /* false root (black) */
    struct _node *g = NULL,
                 *t = &head,
                 *p = NULL,
                 *q;
    int dir = 0, last = 0;
    int inserted = 0;
    void *rv = NULL;

    q = t->link[1] = bt->root;

    for (;;) {
        if (!q && !inserted && !rv) {
            p->link[dir] = q = _make(key_hash, data);
            inserted = 1;
        } else if (_red(q->link[0]) && _red(q->link[1])) {
            q->red = 1;
            q->link[0]->red = 0;
            q->link[1]->red = 0;
        }

        if (_red(q) && _red(p)) {
            int dir2 = t->link[1] == g;

            if (q == p->link[last]) {
                t->link[dir2] = _single(g, !last);
            } else {
                t->link[dir2] = _double(g, !last);
            }
        }

        if (q->key_hash == key_hash) {
            if (!inserted) {
                if (replace || !q->data) {
                    if (bt->freeF) {
                        bt->freeF(q->data);
                    }
                    q->data = data;
                } else {
                    /* we've not inserted anything & we're not out to replace
                     * things; just return the node contents */
                    rv = q->data;
                }
            }
            break;
        }
        
        last = dir;
        dir = q->key_hash < key_hash;

        if (g) {
            t = g;
        }

        g = p, p = q;
        q = q->link[dir];
    }

    bt->root = head.link[1];
    bt->root->red = 0;

    return NULL;
}

void dict_insert(struct dict *bt, const void *key, void *data) {
    _inssearch(bt, key, data, 1);
}

void *dict_search(struct dict *bt, const void *key) {
    dict_hash_t key_hash = _hash(bt, key);

    struct _node *s = bt->root;
    while (s) {
        if (key_hash == s->key_hash) {
            return s->data;
        } else if (key_hash < s->key_hash) {
            s = s->link[0];
        } else {
            s = s->link[1];
        }
    }

    return NULL;
}

void *dict_search_insert(struct dict *bt, const void *key, void *data) {
    return _inssearch(bt, key, data, 0);
}

static void _free_subtree(dict_free_f f, struct _node *n) {
    if (f) {
        f(n->data);
    }

    /* We check for left and right here, rather than checking for non-NULL n
     * inside the subsequent call, as it reduces the number of function calls
     * by a single order of magnitude! */
    if (n->link[0]) {
        _free_subtree(f, n->link[0]);
    }

    if (n->link[1]) {
        _free_subtree(f, n->link[1]);
    }

    free(n);
}

void dict_remove(struct dict *bt, const void *key) {
    dict_hash_t key_hash = _hash(bt, key);

    if (!bt->root) {
        return;
    }

    struct _node head = {0, 0, 0, {0, 0}},
                 *q = &head,
                 *p = NULL,
                 *g = NULL,
                 *f = NULL;
    int dir = 1;
    q->link[1] = bt->root;

    while (q->link[dir]) {
        int last = dir;

        g = p, p = q;
        q = q->link[dir];
        dir = q->key_hash < key_hash;

        if (q->key_hash == key_hash) {
            f = q;
        }

        if (!_red(q) && !_red(q->link[dir])) {
            if (_red(q->link[!dir])) {
                p = p->link[last] = _single(q, dir);
            } else {
                struct _node *s = p->link[!last];

                if (s) {
                    if (!_red(s->link[!last]) && !_red(s->link[last])) {
                        p->red = 0;
                        s->red = 1;
                        q->red = 1;
                    } else {
                        int dir2 = g->link[1] == p;

                        if (_red(s->link[last])) {
                            g->link[dir2] = _double(p, last);
                        } else if (_red(s->link[!last])) {
                            g->link[dir2] = _single(p, last);
                        }

                        q->red = g->link[dir2]->red = 1;
                        g->link[dir2]->link[0]->red = 0;
                        g->link[dir2]->link[1]->red = 0;
                    }
                }
            }
        }
    }

    if (f) {
        if (bt->freeF) {
            bt->freeF(f->data);
        }

        f->data = q->data;
        f->key_hash = q->key_hash;
        p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
        free(q);
    }

    bt->root = head.link[1];
    if (bt->root) {
        bt->root->red = 0;
    }
}

int dict_empty(const struct dict *bt) {
    if (!bt || !bt->root || !bt->root->data) {
        return 1;
    }

    return 0;
}

void dict_free(struct dict *bt) {
    if (bt->root) {
        _free_subtree(bt->freeF, bt->root);
    }

    free(bt);
}

dict_hash_t stringdict_crc(void const *key) {
    return crc32(key, strlen(key));
}

/* vim: set sw=4 et: */
