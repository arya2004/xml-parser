#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDoc_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){

        XMLNode * main_node = getCHild(doc.root,0);


        XMLDoc_free(&doc);
    }
    return 0;
}
