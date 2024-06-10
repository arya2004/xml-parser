// xml_parser.h
// Created by ziegler on 8/21/23.

#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <stdbool.h>

typedef struct IAttribute {
    char* key;
    char* value;
} Attribute;

typedef struct IAttributeVector {
    int heap_size;
    int size;
    Attribute* data;
} AttributeVector;

typedef struct IXMLNode {
    char* tag;
    char* text;
    struct IXMLNode* daddy;
    AttributeVector attributes;
    struct IXMLNodeVector* children;
} XMLNode;

typedef struct IXMLNodeVector {
    int heap_size;
    int size;
    XMLNode** childrens;
} XMLNodeVector;

typedef struct IXMLDocument {
    XMLNode* root;
} XMLDocument;

// Function declarations
bool ends_with(const char* haystack, const char* needle);

void freeAttribute(Attribute* attribute);

void InitAttributes(AttributeVector* attrs);
void AddAttribute(AttributeVector* attrs, Attribute* attr);

void InitChild(XMLNodeVector* vec);
void AddChild(XMLNodeVector* vec, XMLNode* node);

XMLNode* initXMLNode(XMLNode* daddy);
void freeXMLNode(XMLNode* node);
XMLNode* getChild(XMLNode* parent, int index);

bool XMLDoc_load(XMLDocument* document, const char* path);
void XMLDoc_free(XMLDocument* document);

#endif // XML_PARSER_H
