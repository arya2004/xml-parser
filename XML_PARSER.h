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

//helper func  to see if first str ends with second
bool ends_with(const char* haystack, const char* needle){
    int h_len = strlen(haystack);
    int n_len = strlen(needle);
    if(h_len < n_len){
        return false;
    }
    for (int i = 0; i < n_len; i++) {
        if(haystack[h_len - n_len + i] != needle[i])
            return false;
    }
    return true;
}


//___________________________________
typedef struct _XMLAttribute{
    char* key;
    char* value;
} XMLAttribute;

void XMLAttribute_free(XMLAttribute* attr);

typedef struct _XMLAttributeList{
    int heap_size;
    int size;
    XMLAttribute* data;
}XMLAttributeList;

void XMLAttributeList_init(XMLAttributeList * list);
void XMLAttributeList_add(XMLAttributeList * list, XMLAttribute * attr);

typedef struct _XMLNodeList{
    int heap_size;
    int size;
    struct _XMLNode** data;
}XMLNodeList;

void XMLNodeList_init(XMLNodeList * list);
//void XMLNodeList_add(XMLNodeList * list, struct _XMLNode * node);

typedef struct _XMLNode{
    char* tag;
    char* inner_text;
    struct _XMLNode* parent;
    XMLAttributeList attributes;
    XMLNodeList children;
}XMLNode;

XMLNode* XMLNode_new(XMLNode* parent);
void XMLNode_free(XMLNode* node);
XMLNode* XMLNode_child(XMLNode* parent,int index);
char* XMLNode_atr_val(XMLNode* node, char * key);

typedef struct _XMLDocument{
    XMLNode * root;
    char* version;
    char* encoding;
}XMLDocument;


bool XMLDocument_load(XMLDocument* doc, const char* path);
void XMLDocument_free(XMLDocument* doc);




//______________________________________________________

void XMLAttribute_free(XMLAttribute* attr)
{
    free(attr->value);
    free(attr->key);
}

void XMLAttributeList_init(XMLAttributeList * list)
{
    list->heap_size = 1;
    list->size = 0;
    list->data = (XMLAttribute*) malloc(sizeof (XMLAttribute)* list->heap_size);
}
void XMLAttributeList_add(XMLAttributeList * list, XMLAttribute * attr)
{
    while (list->size >= list->heap_size){
        list->heap_size *= 2;
        list->data = (XMLAttribute*) realloc(list->data, sizeof (XMLAttribute) * list->heap_size);
    }
    list->data[list->size] = *attr;
    list->size++;
}

void XMLNodeList_init(XMLNodeList * list)
{
    list->heap_size = 1;
    list->size = 0;
    list->data = (XMLNode**) malloc(sizeof (XMLNode*) * list->heap_size);

}
void XMLNodeList_add(XMLNodeList * list, XMLNode * node)
{
    while (list->size >= list->heap_size){
        list->heap_size *= 2;
        list->data = (XMLNode**) realloc(list->data, sizeof (XMLNode*) * list->heap_size);
    }
    list->data[list->size] = node;
    list->size++;
}

XMLNode* XMLNode_new(XMLNode* parent)
{
    XMLNode* node = (XMLNode*) malloc(sizeof (XMLNode ));
    node->parent = parent;
    node->tag = NULL;
    node->inner_text = NULL;
    XMLAttributeList_init(&node->attributes);
    XMLNodeList_init(&node->children);
    if(parent)
        XMLNodeList_add(&parent->children,node);
    return node;
}

void XMLNode_free(XMLNode* node)
{
    if(node->tag)
        free(node->tag);
    if(node->inner_text)
        free(node->inner_text);
    for (int i = 0; i < node->attributes.size; i++) {
        XMLAttribute_free(&node->attributes.data[i]);
    }
    free(node);
}

XMLNode* XMLNode_child(XMLNode* parent,int index)
{
    return parent->children.data[index];
}

char* XMLNode_atr_val(XMLNode* node, char * key)
{
    for (int i = 0; i < node->attributes.size; i++) {
        XMLAttribute  a = node->attributes.data[i];
        if(!strcmp(a.key, key)){
            return a.value;
        }
    }
    return NULL;
}

static void parse_attrs(char* buffer,int* i,char* lex ,int* lexi, XMLNode* current_node)
{
    XMLAttribute  curr_attr = {0,0};
    while (buffer[*i] != '>'){
        lex[(*lexi)++] = buffer[(*i)++];

        //tag name
        if(buffer[*i] == ' ' && !current_node->tag){
            lex[*lexi] = '\0';
            current_node->tag = strdup(lex);
            lexi = 0;
            (*i)++;
            continue;
        }
        //usually ignore spaces
        if(lex[*lexi -1] == ' '){
            (*lexi)--;
            continue;
        }
        //equal sign indicates buffer has attr key
        if(buffer[*i] == '='){
            lex[(*lexi)] = '\0';
            curr_attr.key = strdup(lex);
            *lexi = 0;
            continue;
        }
        //attr value. check if key exist
        if(buffer[*i] == '"'){
            if(!curr_attr.key){
                fprintf(stderr, "no key, dumass\n");
                return;
            }
            *lexi = 0;
            (*i)++;

            while (buffer[*i] != '"'){
                lex[*lexi] = buffer[*i];
                (*lexi)++;
                (*i)++;
            }
            lex[*lexi] = '\0';
            curr_attr.value = strdup(lex);
            XMLAttributeList_add(&current_node->attributes, &curr_attr);
            curr_attr.key = NULL;
            curr_attr.value = NULL;
            (*lexi) = 0;
            (*i)++;
            continue;

        }

    }
}

bool XMLDocument_load(XMLDocument* doc, const char* path)
{
    FILE* file = fopen(path,"r");
    if(!file){
        fprintf(stderr, "cant load file from '%s', lol", path);
    }
    //get file size
    fseek(file, 0 , SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    //load file data
    char* buffer = (char*) malloc(sizeof(char) * size + 1);
    fread(buffer,1,size, file );
    fclose(file);
    buffer[size] = '\0';

    //set parent to null, if later this is root node
    doc->root = XMLNode_new(NULL);

    //lexical analysis
    char lex[256];
    int lexi = 0;
    int i = 0;
    XMLNode* current_node = doc->root;

    while (buffer[i] != '\0')
    {
        if(buffer[i] == '<')
        {
            //check for inner-text
            lex[lexi] = '\0';
            if(lexi > 0){
                if(!current_node){
                    fprintf(stderr, "text out of doc\n");
                    return false;
                }
                current_node->inner_text = strdup(lex);
                lexi = 0;
            }
            //end of node
            if(buffer[i+1] == '/'){
                i+=2;
                while (buffer[i] != '>'){
                    lex[lexi] = buffer[i];
                    lexi++;
                    i++;
                }
                lex[lexi] = '\0';

                if(!current_node){
                    fprintf(stderr, "already at the root");
                    return false;
                }
                //not, cause if str equal, then they true
                if(strcmp(current_node->tag,lex)){
                    fprintf(stderr, "end start tags dont match,\n %s != %s ", current_node->tag, lexi);
                    return false;
                }
                //if end tag, then go pack to parent
                current_node = current_node->parent;
                i++;
                continue;
            }

            //special tag
            if(buffer[i+1] == '!'){
                while (buffer[i] != ' ' && buffer[i] != '>'){
                    lex[lexi++] = buffer[i++];
                }
                lex[lexi] = '\0';

                if(!strcmp(lex, "<!--")){
                    lex[lexi] = '\0';
                    while (!ends_with(lex, "-->")){
                        lex[lexi++] = buffer[i++];
                        lex[lexi] = '\0';
                    }
                    //i++;
                    continue;
                }
            }

            //declaration tag
            if(buffer[i + 1] == '?'){
                while (buffer[i] != ' ' && buffer[i] != '>'){
                    lex[lexi++] = buffer[i++];
                }
                lex[lexi] = '\0';

                //xml declaration
                if(!strcmp(lex, "<?xml")){
                    lexi = 0;
                    XMLNode* desc = XMLNode_new(NULL);
                    parse_attrs(buffer,&i,lex,&lexi,desc);

                    doc->version = XMLNode_atr_val(desc,"version");
                    doc->encoding = XMLNode_atr_val(desc, "encoding");
                    continue;
                }
            }

            //set current node
            current_node = XMLNode_new(current_node);

            // get beginning of a tag
            i++;
            parse_attrs(buffer,&i,lex,&lexi,current_node);
            //set tag name, if none
            lex[lexi] = '\0';
            if(!current_node->tag )
                current_node->tag = strdup(lex);
           // current_node->tag = strdup(lex);

            //reset the lexer
            lexi = 0;
            i++;
            continue;
        }else{
            lex[lexi] = buffer[i];
            lexi++;
            i++;
        }
    }

    return true;
}
void XMLDocument_free(XMLDocument* doc)
{
    XMLNode_free(doc->root);
}

#endif //XML_PARSER_SPEEDRUN_XML_PARSER_H
