#include <vector>
#include <cstring>
#include "info_send.hpp"
#include "info_rev.hpp"
// 回调函数


int imcode(std::vector<unsigned char> &map, int &p, uint32_t num, double dnum, int sam)
{
    if (sam == 0) {    // uint32
        map[p++] = static_cast<unsigned char>((num) & 0xFF);
        map[p++] = static_cast<unsigned char>((num >> 8) & 0xFF);
        map[p++] = static_cast<unsigned char>((num >> 16) & 0xFF);
        map[p++] = static_cast<unsigned char>((num >> 24) & 0xFF);
    } else {           // double
        unsigned char buffer[sizeof(double)];
        memcpy(buffer, &dnum, sizeof(double));
        for (int i = 0; i < 8; i++) {
            map[p++] = buffer[i];
        }
    }
    return 0;
}

struct postrue
{
    double x;
    double y;
    double z;
    double dx;
    double dy;
    double dz;
    double w;
};

int main(int argc, char** argv) {

    const char* ip = "127.0.0.1";
    InfoSend IS(ip, 12345);
    InfoRev IR(12346);
    while (IS.init() != 0) {

    }
    IR.init();
    std::cout << "connect successfully\n";

    postrue P;
    P.x = 1;
    P.y = 2;
    P.z = 3;
    P.dx = 4;
    P.dy = 5;
    P.dz = 6;
    P.w = 788888;
    int len = sizeof(postrue);
    std::vector<unsigned char> postrueInfo(len);
    int pos = 0;
    imcode(postrueInfo, pos, 0, P.x, 1);
    imcode(postrueInfo, pos, 0, P.y, 1);
    imcode(postrueInfo, pos, 0, P.z, 1);
    imcode(postrueInfo, pos, 0, P.dx, 1);
    imcode(postrueInfo, pos, 0, P.dy, 1);
    imcode(postrueInfo, pos, 0, P.dz, 1);
    imcode(postrueInfo, pos, 0, P.w, 1);


    std::vector<unsigned char> reply;
    while (1) {
        
        IS.sendMsg(postrueInfo);
        reply = IR.run();
        if (reply.size() != 2 && reply[0] != 'O' && reply[1] != 'K') {
            std::cout << "msg error";
        }
        
    }
    return 0;
}
