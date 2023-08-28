#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDocument_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){

        printf("%s\n%s\n", doc.root->tag, doc.root->inner_text);


        XMLDocument_free(&doc);
    }
    return 0;
}
