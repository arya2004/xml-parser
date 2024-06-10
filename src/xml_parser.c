// xml_parser.c
#include "xml_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Function definitions

bool ends_with(const char* haystack, const char* needle) {
    int h_len = strlen(haystack);
    int n_len = strlen(needle);
    if (h_len < n_len) {
        return false;
    }
    for (int i = 0; i < n_len; ++i) {
        if (haystack[h_len - n_len + i] != needle[i]) {
            return false;
        }
    }
    return true;
}

void freeAttribute(Attribute* attribute) {
    free(attribute->key);
    free(attribute->value);
}

void InitAttributes(AttributeVector* attrs) {
    attrs->heap_size = 1;
    attrs->size = 0;
    attrs->data = (Attribute*) malloc(sizeof(Attribute) * attrs->heap_size);
}

void AddAttribute(AttributeVector* attrs, Attribute* attr) {
    while (attrs->size >= attrs->heap_size) {
        attrs->heap_size *= 2;
        attrs->data = (Attribute*) realloc(attrs->data, sizeof(Attribute) * attrs->heap_size);
    }
    attrs->data[attrs->size] = *attr;
    attrs->size++;
}

void InitChild(XMLNodeVector* vec) {
    vec->heap_size = 1;
    vec->size = 0;
    vec->childrens = (XMLNode**) malloc(sizeof(XMLNode*) * vec->heap_size);
}

void AddChild(XMLNodeVector* vec, XMLNode* node) {
    while (vec->size >= vec->heap_size) {
        vec->heap_size *= 2;
        vec->childrens = (XMLNode**) realloc(vec->childrens, sizeof(XMLNode*) * vec->heap_size);
    }
    vec->childrens[vec->size] = node;
    vec->size++;
}

XMLNode* initXMLNode(XMLNode* daddy) {
    XMLNode* node = (XMLNode*) malloc(sizeof(XMLNode));
    node->daddy = daddy;
    node->tag = NULL;
    node->text = NULL;
    InitAttributes(&node->attributes);
    node->children = (XMLNodeVector*) malloc(sizeof(XMLNodeVector));
    InitChild(node->children);
    if (daddy) {
        AddChild(daddy->children, node);
    }
    return node;
}

void freeXMLNode(XMLNode* node) {
    if (node->tag) free(node->tag);
    if (node->text) free(node->text);
    for (int i = 0; i < node->attributes.size; ++i) {
        freeAttribute(&node->attributes.data[i]);
    }
    free(node->attributes.data);
    for (int i = 0; i < node->children->size; ++i) {
        freeXMLNode(node->children->childrens[i]);
    }
    free(node->children->childrens);
    free(node->children);
    free(node);
}

XMLNode* getChild(XMLNode* parent, int index) {
    return parent->children->childrens[index];
}

bool XMLDoc_load(XMLDocument* document, const char* path) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        printf("Can't open file\n");
        return false;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*) malloc(sizeof(char) * size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    document->root = initXMLNode(NULL);

    // Lexical analysis
    char lexical_buffer[256];
    int lex_pointer = 0;
    int i = 0;

    XMLNode* current = document->root;
    while (buffer[i] != '\0') {
        if (buffer[i] == '<') {
            lexical_buffer[lex_pointer] = '\0';
            if (strlen(lexical_buffer) > 0) {
                if (!current) {
                    printf("Text outside document\n");
                    return false;
                }
                current->text = strdup(lexical_buffer);
                lex_pointer = 0;
            }

            if (buffer[i + 1] == '/') {
                i += 2;
                while (buffer[i] != '>') {
                    lexical_buffer[lex_pointer] = buffer[i];
                    i++;
                    lex_pointer++;
                }
                lexical_buffer[lex_pointer] = '\0';
                if (!current) {
                    printf("No matching opening tag\n");
                    return false;
                }
                if (strcmp(current->tag, lexical_buffer)) {
                    printf("Mismatched closing tag\n");
                    return false;
                }
                current = current->daddy;
                i++;
                continue;
            }

            if (buffer[i + 1] == '!') {
                while (buffer[i] != ' ' && buffer[i] != '>') {
                    lexical_buffer[lex_pointer] = buffer[i];
                    i++;
                    lex_pointer++;
                }
                lexical_buffer[lex_pointer] = '\0';
                if (!strcmp(lexical_buffer, "<!--")) {
                    while (!ends_with(lexical_buffer, "-->")) {
                        lexical_buffer[lex_pointer] = buffer[i];
                        i++;
                        lex_pointer++;
                        lexical_buffer[lex_pointer] = '\0';
                    }
                    continue;
                }
            }

            current = initXMLNode(current);
            i++;

            Attribute curr_attr = {0, 0};
            while (buffer[i] != '>') {
                lexical_buffer[lex_pointer] = buffer[i];
                i++;
                lex_pointer++;

                if (buffer[i] == ' ' && !current->tag) {
                    lexical_buffer[lex_pointer] = '\0';
                    current->tag = strdup(lexical_buffer);
                    lex_pointer = 0;
                    i++;
                    continue;
                }

                if (lexical_buffer[lex_pointer] == ' ') {
                    lex_pointer--;
                    continue;
                }

                if (buffer[i] == '=') {
                    lexical_buffer[lex_pointer] = '\0';
                    curr_attr.key = strdup(lexical_buffer);
                    lex_pointer = 0;
                    continue;
                }

                if (buffer[i] == '"') {
                    if (!curr_attr.key) {
                        printf("No attribute key\n");
                        return false;
                    }
                    lex_pointer = 0;
                    i++;
                    while (buffer[i] != '"') {
                        lexical_buffer[lex_pointer] = buffer[i];
                        i++;
                        lex_pointer++;
                    }
                    lexical_buffer[lex_pointer] = '\0';
                    curr_attr.value = strdup(lexical_buffer);
                    AddAttribute(&current->attributes, &curr_attr);
                    curr_attr = (Attribute) {0, 0};
                    lex_pointer = 0;
                    i++;
                    continue;
                }
            }
            lexical_buffer[lex_pointer] = '\0';
            if (!current->tag) {
                current->tag = strdup(lexical_buffer);
            }
            lex_pointer = 0;
            i++;
            continue;
        } else {
            lexical_buffer[lex_pointer] = buffer[i];
            i++;
            lex_pointer++;
        }
    }

    free(buffer);
    return true;
}

void XMLDoc_free(XMLDocument* document) {
    freeXMLNode(document->root);
}
