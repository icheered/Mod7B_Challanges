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
#include <cstdint>
#include <string>
#include <vector>
std::string base64_encode(std::vector<int32_t> const& data);
std::string base64_decode(std::string const& s);
#endif
