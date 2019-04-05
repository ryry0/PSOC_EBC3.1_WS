#include <stdio.h>
#include <stdint.h>

void pieceWisePrint(uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    printf("%d\n", data[i]);
  }
}

void putLargeDataReady(uint8_t *data, size_t len) {
  const size_t MAX_BUFFER_LEN = 2;
  size_t buffer_size = MAX_BUFFER_LEN;

  for (size_t index = 0; index < len; index+=buffer_size) {

    if ((len - index) > MAX_BUFFER_LEN) {
      buffer_size = MAX_BUFFER_LEN;
      printf("lsdfl %d %d\n", MAX_BUFFER_LEN, len-index);
    }
    else if ((len - index) < 0)
      buffer_size = 0;
    else
      buffer_size = len - index;

    printf("index %d, buffsize %d, lenin %d\n", index, buffer_size, len-index);
    pieceWisePrint(data + index, buffer_size);

  }
}

int main(int argc, char ** argv) {
#define len 75
  char str[len]  = {0};
  for (size_t i = 0; i < len; ++i) {
    str[i] = i;
  }

  putLargeDataReady(str, 3);

  printf("ldlsfjalsjdflsdlfjalsjdflkajsdf");
  putLargeDataReady(str, len);
  return 0;
}
