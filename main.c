#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDoc_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){

        XMLNode * more_node = getCHild(getCHild(doc.root,0),0);
        printf("%s => %s\n", more_node->tag, more_node->text);
        XMLNode * second  = getCHild(doc.root,1);
        printf("%s => %s\n", second->text, second->tag);
    }
    return 0;
}
