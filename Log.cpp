#include "Function.h"

#include <ostream>

/*ログを出そう*/

///出力ウィンドウに文字を出す

void Log(std::ostream& os, const std::string& message) {
    os << message << std::endl;
    OutputDebugStringA(message.c_str());
}

