#ifndef __LATIN1_TO_ASCII_H__
#define __LATIN1_TO_ASCII_H__

#include <string>

using std::string;

inline string converter(unsigned char c)
{
  switch(c)
    {
    case 160: return "";
    case 161: return "";
    case 162: return "";
    case 163: return "";
    case 164: return "";
    case 165: return "";
    case 166: return "";
    case 167: return "";
    case 168: return "";
    case 169: return "";
    case 170: return "";
    case 171: return "";
    case 172: return "";
    case 173: return "";
    case 174: return "";
    case 175: return "";
    case 176: return "";
    case 177: return "";
    case 178: return "";
    case 179: return "";
    case 180: return "";
    case 181: return "";
    case 182: return "";
    case 183: return "";
    case 184: return "";
    case 185: return "";
    case 186: return "";
    case 187: return "";
    case 188: return "";
    case 189: return "";
    case 190: return "";
    case 191: return "";
    case 192: return "A";
    case 193: return "A";
    case 194: return "A";
    case 195: return "A";
    case 196: return "Ae";
    case 197: return "A";
    case 198: return "Ae";
    case 199: return "C";
    case 200: return "E";
    case 201: return "E";
    case 202: return "E";
    case 203: return "E";
    case 204: return "I";
    case 205: return "I";
    case 206: return "I";
    case 207: return "I";
    case 208: return "Th";
    case 209: return "N";
    case 210: return "O";
    case 211: return "O";
    case 212: return "O";
    case 213: return "O";
    case 214: return "Oe";
    case 215: return "";
    case 216: return "Oe";
    case 217: return "U";
    case 218: return "U";
    case 219: return "U";
    case 220: return "Ue";
    case 221: return "Y";
    case 222: return "Th";
    case 223: return "ss";
    case 224: return "a";
    case 225: return "a";
    case 226: return "a";
    case 227: return "a";
    case 228: return "ae";
    case 229: return "a";
    case 230: return "ae";
    case 231: return "c";
    case 232: return "e";
    case 233: return "e";
    case 234: return "e";
    case 235: return "e";
    case 236: return "i";
    case 237: return "i";
    case 238: return "i";
    case 239: return "i";
    case 240: return "o";
    case 241: return "n";
    case 242: return "o";
    case 243: return "o";
    case 244: return "o";
    case 245: return "o";
    case 246: return "oe";
    case 247: return "";
    case 248: return "oe";
    case 249: return "u";
    case 250: return "u";
    case 251: return "u";
    case 252: return "ue";
    case 253: return "y";
    case 254: return "th";
    case 255: return "y";
    default:
      string r;
      r += c;
      return r;
    }
}

inline string latin1_to_ascii(const string& s)
{
  string result;
  for(string::size_type i = 0; i < s.size(); ++i)
    {
      result += converter(s[i]);
    }
  return result;
}

#endif
