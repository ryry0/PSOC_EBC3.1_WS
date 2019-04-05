#include <stdio.h>
#include <ring_buffer.h>

int main(int argc, char ** argv) {

  ring_buffer_t ring_buffer = rb_create(10);
  ring_buffer_t input_buffer = rb_create(10);

  for (int i = 0; i < 10; ++i) {
    rb_pushFront(ring_buffer, 1.0*i);
  }

  for (int i = 0; i < 10; ++i) {
    printf("%f\t", rb_get(ring_buffer, i));
  }
  printf("\n");

  for (int i = 0; i < 5; ++i) {
    rb_pushFront(ring_buffer, -1.0*i);
  }

  for (int i = 0; i < 10; ++i) {
    printf("%f\t", rb_get(ring_buffer, i));
  }
  printf("\n");

  for (;;) {
    float input = 0;
    scanf("%f", &input);

    rb_pushFront(input_buffer, input);
    for (int i = 0; i < 10; ++i) {
      printf("%f\t", rb_get(input_buffer, i));
    }
    printf("\n");
  }
  return 0;
}
