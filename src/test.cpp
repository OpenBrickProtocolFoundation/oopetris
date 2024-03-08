#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

    NFD_Init();

    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[2] = {
        {"Source code", "c,cpp,cc"},
        {    "Headers",    "h,hpp"}
    };
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
    if (result == NFD_OKAY) {
        puts("Success!");
        puts(outPath);
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    } else {
        printf("Error: %s\n", NFD_GetError());
    }

    NFD_Quit();
    return 0;
}


/* https://github.com/btzy/nativefiledialog-extended

https://github.com/btzy/nativefiledialog-extended/tree/master/src

https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_basics.cpp

https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_advanced.cpp#L81

https://sol2.readthedocs.io/en/latest/api/usertype.html

https://github.com/teal-language/tl/blob/master/docs/tutorial.md

https://github.com/ThePhD/sol2/blob/e8e122e9ce46f4f1c0b04003d8b703fe1b89755a/examples/source/lua_value.cpp

https://github.com/ThePhD/sol2/tree/v3.3.1?tab=readme-ov-file

https://github.com/mgerhold/oopetris/tree/ai
 */
//TODO: make c bindings for the ai, so that js, python lua etc. can be used, lua is c++ native, but js and python c handles, also haskell for fun or something similar xD
