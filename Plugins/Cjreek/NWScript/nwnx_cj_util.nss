#include "nwnx"

const string NWNX_Cjreek_Util = "NWNX_Cjreek"; ///< @private

string NWNX_Util_ColorString(string sString, int nColor);

string NWNX_Util_ColorString(string sString, int nColor)
{
    string sFunc = "ColorString";

    NWNX_PushArgumentInt(nColor);
    NWNX_PushArgumentString(sString);

    NWNX_CallFunction(NWNX_Cjreek_Util, sFunc);

    return NWNX_GetReturnValueString();
}
