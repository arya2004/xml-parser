#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDocument_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){
        XMLNode* str = XMLNode_child(doc.root, 0);
        printf("Struct: %s\n", XMLNode_atr_val(str, "name"));

        XMLNodeList* fields = XMLNode_children(str, "field");
        for (int i = 0; i < fields->size; i++) {
            XMLNode* field = XMLNodeList_at(fields, i);
            printf("  %s (%s)\n", XMLNode_atr_val(field, "name"), XMLNode_atr_val(field, "type"));
        }

        XMLDocument_free(&doc);
        printf("dome");
    }
    return 0;
}
