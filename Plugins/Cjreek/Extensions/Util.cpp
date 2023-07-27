#include "nwnx.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

NWNX_EXPORT ArgumentStack ColorString(ArgumentStack&& args)
{
    std::string sStr = args.extract<std::string>();
    uint32_t nColor = args.extract<int32_t>();
    uint8_t nRed = (nColor >> 16) & 0xFF;
    uint8_t nGreen = (nColor >> 8) & 0xFF;
    uint8_t nBlue = nColor & 0xFF;

    if (nRed == 0) nRed = 1;
    if (nGreen == 0) nGreen = 1;
    if (nBlue == 0) nBlue = 1;

    std::string sResult = "<c";
    sResult += (char)(nRed);
    sResult += (char)(nGreen);
    sResult += (char)(nBlue);
    sResult += ">";
    sResult += sStr;
    sResult += "</c>";

    return sResult;
}
