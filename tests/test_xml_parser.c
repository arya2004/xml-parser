// test_xml_parser.c
#include "include/xml_parser.h"
#include <assert.h>
#include <stdio.h>

void test_loadXML() {
    XMLDocument doc;
    assert(XMLDoc_load(&doc, "examples/meow.xml") == true);
    XMLDoc_free(&doc);
}

int main() {
    test_loadXML();
    printf("All tests passed!\n");
    return 0;
}
