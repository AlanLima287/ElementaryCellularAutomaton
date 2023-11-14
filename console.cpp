#include <iostream>

typedef unsigned char uchar;
using namespace std;

#define ALIVE_CELL (char)0xdb
#define DEAD_CELL (char)0x20

inline int mod(int num, int mod) {
   int result = num % mod;
   return (result < 0) ? result + mod : result;
}

int main(int argc, char* args[]) {

   int rule = 30;
   const int width = 86;
   const int heigth = width >> 1;
   bool generations[2][width] = {};

   while (true) {

      for (int i = 0; i < width; i++) {
         generations[0][i] = false;
         generations[1][i] = false;
      }

      generations[0][width >> 1] = true;

      cout << ">Insert the rule (0 to 255): "; cin >> rule;

      system("cls");

      short index = 0;
      for (int y = 0; y < heigth; y++) {
         for (int x = 0; x < width; x++) {

            short top = 0;
            
            if (generations[index][mod(x - 1, width)]) top += 4;
            if (generations[index][mod(x + 1, width)]) top += 1;
            if (generations[index][x]) top += 2;

            generations[index ^ 1][x] = (rule & (1 << top)) > 0;
            cout << (generations[index][x] ? ALIVE_CELL : DEAD_CELL);
         }

         cout << '\n';
         index ^= 1;
      }
   }

   return 0;
}