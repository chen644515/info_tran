#include <cstring>
#include "info_send.hpp"
#include "info_rev.hpp"
#include <vector>


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

double decode(std::vector<unsigned char> &buffer, int pos)
{
    unsigned char bytes[8];
    double dnum;
    for (int i = pos; i < pos + 8; i++) {
        bytes[i - pos] = buffer[i];
    }
    memcpy(&dnum, bytes, sizeof(double));
    return dnum;
}
int main(int argc, char** argv)
{
    const char* ip = "127.0.0.1";
    InfoRev IR(12345);
    InfoSend IS(ip, 12346);
    IR.init();
    while (IS.init() != 0) {


    }
    std::cout << "connect successfully\n";
    std::vector<unsigned char> buffer;
    int count = 0;
    std::vector<unsigned char> reply(2);
    reply[0] = 'O';
    reply[1] = 'K';

    postrue P;


    while (1) {
        buffer = IR.run();
        IS.sendMsg(reply);
        P.x = decode(buffer, 0);
        P.y = decode(buffer, 8);
        P.z = decode(buffer, 16);
        P.dx = decode(buffer, 24);
        P.dy = decode(buffer, 32);
        P.dz = decode(buffer, 40);
        P.w = decode(buffer, 48);
        std::cout << P.x << ' ' << P.y << ' ' << P.z << ' ' << P.dx << ' ' << P.dy << ' ' << P.dz << ' ' << P.w
 << '\n';
        // 等待下一次循环
    }
}