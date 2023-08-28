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

typedef struct _XMLNode{
    char* tag;
    char* inner_text;
    struct _XMLNode* parent;
}XMLNode;

XMLNode* XMLNode_new(XMLNode* parent);
void XMLNode_free(XMLNode* node);

typedef struct _XMLDocument{
    XMLNode * root;
}XMLDocument;


bool XMLDocument_load(XMLDocument* doc, const char* path);
void XMLDocument_free(XMLDocument* doc);




//______________________________________________________

XMLNode* XMLNode_new(XMLNode* parent)
{
    XMLNode* node = (XMLNode*) malloc(sizeof (XMLNode ));
    node->parent = parent;
    node->tag = NULL;
    node->inner_text = NULL;
    return node;
}

void XMLNode_free(XMLNode* node)
{
    if(node->tag)
        free(node->tag);
    if(node->inner_text)
        free(node->inner_text);
    free(node);
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
    XMLNode* current_node = NULL;

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

            //set current node
            if(!current_node)
                current_node = doc->root;
            else
                current_node = XMLNode_new(current_node);
            i++;
            while (buffer[i] != '>'){
                lex[lexi] = buffer[i];
                lexi++;
                i++;
            }
            lex[lexi] = '\0';
            current_node->tag = strdup(lex);

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
