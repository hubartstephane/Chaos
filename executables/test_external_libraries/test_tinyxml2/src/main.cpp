#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <tinyxml2.h>

int main(int argc, char ** argv)
{
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("Root");
	xmlDoc.InsertFirstChild(pRoot);
	tinyxml2::XMLElement * pElement = xmlDoc.NewElement("IntValue");
	pElement->SetText(10);
	pRoot->InsertEndChild(pElement);
	
	tinyxml2::XMLPrinter printer;
	xmlDoc.Print( &printer );
	

	std::cout << printer.CStr();
	return 0;
}
 