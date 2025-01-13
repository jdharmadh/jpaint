#ifndef STACK_H_
#define STACK_H_

#include <stdint.h>

typedef struct {
  uint16_t x, y;
} Point;

typedef struct {
  u_int16_t top;  // top represents next available slot, 0 if empty
  u_int16_t capacity;
  Point* array;
} Stack;

Stack* createStack(unsigned capacity);
int isFull(Stack* stack);
int isEmpty(Stack* stack);
void push(Stack* stack, uint16_t x, uint16_t y);
Point pop(Stack* stack);
Point peek(Stack* stack);
void clear(Stack* stack);
void destroy(Stack* stack);

#endif  // STACK_H_
