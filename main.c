#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDocument_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){
        XMLNode * str = XMLNode_child(doc.root,0);
        printf("%s\n", str->tag);
        for (int i = 0; i < str->children.size; i++) {
            XMLNode* n = XMLNode_child(str, i);
            printf("%s\n", n->tag);
        }

        XMLDocument_free(&doc);
        printf("dome");
    }
    return 0;
}
