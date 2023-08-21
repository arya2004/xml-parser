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

typedef struct IXMLDocumet{
    char* src;
}XMLDocument;

bool XMLDoc_load(XMLDocument* document,const char* path );
void XMLDoc_free(XMLDocument* document);


void tester(int a)
{
    printf("%i", a);
}


//______________________________________________________

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

    document->src = (char *) malloc(sizeof (char) * size + 1);
    fread(document->src,1, size, fp);
    fclose(fp);
    document->src[size] = '\0';

    return true;
}


void XMLDoc_free(XMLDocument* document)
{
    free(document->src);
}

#endif //XML_PARSER_SPEEDRUN_XML_PARSER_H
