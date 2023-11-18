#include <iostream>

#define ALIVE_CELL (char)0xdb
#define DEAD_CELL (char)0x20

int mod(int num, int mod) {
   int result = num % mod;
   return num < 0 ? result + mod : result;
}

int main() {

   const int width = 64;
   const int height = (width - 1) / 2;
   bool gens[2][width];

   bool ruleBits[8] = {};
   int rule;

   while (true) {

      for (int i = 0; i < width; i++) {
         gens[0][i] = false;
         gens[1][i] = false;
      }

      gens[0][height] = true;

      std::cout << ">Insert the rule (0 to 255): "; std::cin >> rule;
      if (rule < 0) break;

      for (int i = 0; i < 8; i++) {
         ruleBits[i] = rule % 2;
         rule /= 2;
      }

      system("cls");
      char index = 0;

      for (int y = 0; y < height; y++) {

         for (int x = 1; x < width - 1; x++) {

            char topLayer = 0;
            if (gens[index][mod(x - 1, width)]) topLayer += 4;
            if (gens[index][mod(x + 1, width)]) topLayer += 1;
            if (gens[index][x]) topLayer += 2;

            gens[index ^ 1][x] = ruleBits[topLayer];
            std::cout << (ruleBits[topLayer] ? ALIVE_CELL : DEAD_CELL);
         }

         std::cout << '\n';
         index ^= 1;
      }
   }

   return 0;
}
