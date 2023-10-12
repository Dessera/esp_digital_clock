#include "eclock/music.hpp"

int select_music(int i) {
  if (i % 10 != 0) {
    if (i >= 1 && i <= 7) {
      return low[i - 1];
    } else if (i >= 11 && i <= 17) {
      return mid[i - 11];
    } else if (i >= 21 && i <= 27) {
      return high[i - 21];
    }
  } else {
    if (i >= 10 && i <= 70) {
      return lowj[(i - 10) / 10];
    } else if (i >= 110 && i <= 170) {
      return midj[(i - 110) / 10];
    } else if (i >= 210 && i <= 270) {
      return highj[(i - 210) / 10];
    }
  }
}
