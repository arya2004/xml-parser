#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDoc_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){
        printf("%s\n%s", doc.root->tag, doc.root->text);
        XMLDoc_free(&doc);
    }
    return 0;
}
