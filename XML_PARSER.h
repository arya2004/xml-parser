//
// Created by ziegler on 8/21/23.
//


#ifndef XML_PARSER_SPEEDRUN_XML_PARSER_H
#define XML_PARSER_SPEEDRUN_XML_PARSER_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define  FALSE 0
#endif

typedef struct IAttribute{
    char * key;
    char * value;
}Attribute;

void freeAttribute(Attribute* _attribute);

typedef struct IAttributeVector{
    int heap_size;
    int size;
    Attribute * data;
}AttributeVector;

void InitAttributes(AttributeVector* _attrs);
void AddAttribute(AttributeVector* _attrs, Attribute* _attr);

typedef struct IXMLNodeVEctor{
    int heap_size;
    int size;
    struct IXMLNode** childrens;
}XMLNodeVEctor;



typedef struct IXMLNode{
    char* tag;
    char* text;
    struct IXMLNode* daddy;
    AttributeVector attributes;
    XMLNodeVEctor _children;
}XMLNode;

void InitChild(XMLNodeVEctor* _vec);
void AddChild(XMLNodeVEctor* _vec, XMLNode* _node);

XMLNode* initXMLNode(XMLNode* _daddy);
void freeXMLNode(XMLNode* _node);
XMLNode* getCHild(XMLNode* _parent, int index);

typedef struct IXMLDocumet{
    XMLNode * root;
}XMLDocument;


bool XMLDoc_load(XMLDocument* document,const char* path );
void XMLDoc_free(XMLDocument* document);




//______________________________________________________

void freeAttribute(Attribute* _attribute)
{
    free(_attribute->key);
    free(_attribute->value);
}

void InitAttributes(AttributeVector* _attrs)
{
    _attrs->heap_size = 1;
    _attrs->size=  0;
    _attrs->data = (Attribute*) malloc(sizeof(Attribute)* _attrs->heap_size);
}
void AddAttribute(AttributeVector* _attrs, Attribute* _attr)
{
    while (_attrs->size>= _attrs->heap_size) {
        _attrs->heap_size += 2;
        _attrs->data = (Attribute *) realloc(_attrs->data, sizeof(Attribute) * _attrs->heap_size);
    }
    _attrs->data[_attrs->size] = *_attr;
    _attrs->size++;
}


void InitChild(XMLNodeVEctor* _vec)
{
    _vec->heap_size = 1;
    _vec->size=  0;
    _vec->childrens = (XMLNode **) malloc(sizeof(XMLNode*)* _vec->heap_size);
}

void AddChild(XMLNodeVEctor* _vec, XMLNode* _node)
{
    while (_vec->size>= _vec->heap_size) {
        _vec->heap_size += 2;
        _vec->childrens = (XMLNode **) realloc(_vec->childrens, sizeof(XMLNode*) * _vec->heap_size);
    }
    _vec->childrens[_vec->size] = _node;
    _vec->size++;
}


XMLNode* initXMLNode(XMLNode* _daddy)
{
    XMLNode * node = (XMLNode*) malloc(sizeof(XMLNode));
    node->daddy = _daddy;
    node->tag = NULL;
    node->text = NULL;
    InitAttributes(&node->attributes);
    InitChild(&node->_children);
    if(_daddy) AddChild(&_daddy->_children, node);
    //_daddy->attributes.heap_size = 1;
    //_daddy->attributes.size=  0;
    //_daddy->attributes.data = (Attribute*) malloc(sizeof(Attribute)* _daddy->attributes.heap_size);
    return node;
}

void freeXMLNode(XMLNode* _node)
{
    if(_node->tag) free(_node->tag);

    if(_node->text) free(_node->text);
    for (int i = 0; i < _node->attributes.size; ++i) {
        freeAttribute(&_node->attributes.data[i]);
    }

    free(_node);

}

XMLNode* getCHild(XMLNode* _parent, int index){
    return _parent->_children.childrens[index];
}

bool XMLDoc_load(XMLDocument* document,const char* path )
{
    FILE* fp = fopen(path,"r");
    if(!fp){
        printf("cant open lol");
        return false;
    }
    //get fiel size
    fseek(fp,  0,  SEEK_END);
    int size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    char* buffer = (char *) malloc(sizeof (char) * size + 1);
    fread(buffer,1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    document->root = initXMLNode(NULL);

    //lexical analsis
    char lexical_buffer[265];
    int lex_pointer = 0;
    int i = 0;

    XMLNode * current = NULL;
    while (buffer[i] != '\0')
    {
        if(buffer[i] == '<')
        {
            //inner text
            lex_pointer[lexical_buffer] = '\0';
            if(strlen(lexical_buffer) > 0)
            {
                if(!current)
                {
                    printf("text outside doc");
                    return false;
                }
                current->text = strdup(lexical_buffer);
                lex_pointer = 0;
            }

            //end of node
            if(buffer[i+1] == '/')
            {
                i+=2;
                while (buffer[i] != '>')
                {
                    lexical_buffer[lex_pointer] = buffer[i];
                    i++;
                    lex_pointer++;
                }
                lexical_buffer[lex_pointer] = '\0';
                if(!current)
                {
                    printf("didnt read opening before");
                    return false;
                }
                if(strcmp(current->tag, lexical_buffer))
                {
                    printf("current not same as lexical buffet\n %s %s", current->tag, lexical_buffer);
                    return false;
                }
                current = current->daddy;
                i++;
                continue;
            }

            //set current node
            if(!current) current = document->root;
            else current = initXMLNode(current);
            i++;
            //get beginning of tag
            Attribute curr_attr = {0,0};
            while (buffer[i] != '>')
            {
                lex_pointer[lexical_buffer] = buffer[i]; //magic ;)
                i++;
                lex_pointer++;
                //tag name
                if(buffer[i] == ' ' && !current->tag){
                    lexical_buffer[lex_pointer] = '\0';
                    current->tag = strdup(lexical_buffer);
                    lex_pointer = 0;
                    i++;
                    continue;
                }
                //usually ignore spaces except above case
                if(lexical_buffer[lex_pointer] == ' '){
                    lex_pointer--;
                    continue;
                }

                //key
                if(buffer[i] == '='){
                    lexical_buffer[lex_pointer] = '\0';
                    curr_attr.key = strdup(lexical_buffer);
                    lex_pointer = 0;
                    continue;
                }
                //value of attrivbte
                if(buffer[i] == '"'){
                    if(!curr_attr.key){
                        printf("no key");
                        return false;
                    }
                    lex_pointer = 0;
                    i++;
                    while (buffer[i] != '"'){
                        lexical_buffer[lex_pointer] = buffer[i];
                        i++;
                        lex_pointer++;
                    }
                    lex_pointer[lexical_buffer] = '\0';
                    curr_attr.value = strdup(lexical_buffer);
                    AddAttribute(&current->attributes,&curr_attr);
                    curr_attr = (Attribute) {0,0};
                    lex_pointer = 0;
                    i++;
                    continue;
                }
            }
            //set tag name if none
            lex_pointer[lexical_buffer] = '\0';
            if(!current->tag){
                current->tag = strdup(lexical_buffer);
            }
            //current->tag = strdup(lexical_buffer);

            //reset tis

            lex_pointer = 0;
            i++;
            continue;
        }else{
            lex_pointer[lexical_buffer] = i[buffer];
            i++;
            lex_pointer++;

        }
    }

    return true;
}


void XMLDoc_free(XMLDocument* document)
{
    free(document->root);
}

#endif //XML_PARSER_SPEEDRUN_XML_PARSER_H
