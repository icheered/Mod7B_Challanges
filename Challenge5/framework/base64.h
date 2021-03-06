/************************************************************
 *                                                          *
 **                 DO NOT EDIT THIS FILE                  **
 ***         YOU DO NOT HAVE TO LOOK IN THIS FILE         ***
 **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
 *****                                                  *****
 ************************************************************
*/

#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>
#include <vector>

std::string base64_encode(unsigned char const*, unsigned int len);
std::vector<unsigned char> base64_decode(std::string const& s);
#endif
