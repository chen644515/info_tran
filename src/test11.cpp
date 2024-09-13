#include <iostream>
#include <vector>
#include <cstring>
int main()
{
    double dnum = 0.111111111;
    unsigned char buffer[sizeof(double)];
    memcpy(buffer, &dnum, sizeof(double));
    double a = 0;
    memcpy(&a, buffer, sizeof(double));
    std::cout << a << '\n';
}