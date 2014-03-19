/*
 * Image.cpp
 *
 *  Created on: Jun 12, 2011
 *      Author: tbabb
 */

#include "Image.h"

bool validate_png(std::istream &source){
    png_byte pngsig[PNGSIGSIZE];
    int is_png = 0;
    
    //Read the 8 bytes from the stream into the sig buffer
    source.read((char*)pngsig, PNGSIGSIZE);

    //Check if the read worked...
    if (!source.good()) return false;
    
    is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
    return (is_png == 0);
}
