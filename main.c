#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDoc_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){
        printf("%s\n%s\nAttrs:\n", doc.root->tag, doc.root->text);
        for (int i = 0; i < doc.root->attributes.size; ++i) {
            Attribute a = doc.root->attributes.data[i];
            printf("\n %s => %s", a.key, a.value);
        }
        XMLDoc_free(&doc);
    }
    return 0;
}
