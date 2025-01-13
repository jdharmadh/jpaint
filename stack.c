#include "stack.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Stack* createStack(unsigned capacity) {
  Stack* stack = (Stack*)malloc(sizeof(Stack));
  stack->capacity = capacity;
  stack->top = 0;
  stack->array = (Point*)malloc(stack->capacity * sizeof(Point));
  return stack;
}
int isFull(Stack* stack) { return stack->top == stack->capacity; }
int isEmpty(Stack* stack) { return stack->top == 0; }
void push(Stack* stack, uint16_t x, uint16_t y) {
  if (isFull(stack)) {
    stack->capacity *= 2;
    stack->array =
        (Point*)realloc(stack->array, stack->capacity * sizeof(Point));
  }
  stack->array[++stack->top].x = x;
  stack->array[stack->top].y = y;
}
Point pop(Stack* stack) {
  if (isEmpty(stack)) {
    perror("popped when empty");
    exit(EXIT_FAILURE);
  }
  return stack->array[stack->top--];
}
Point peek(Stack* stack) {
  if (isEmpty(stack)) {
    perror("peeked when empty");
    exit(EXIT_FAILURE);
  }
  return stack->array[stack->top];
}
void clear(Stack* stack) { stack->top = 0; }

void destroy(Stack* stack) {
  free(stack->array);
  free(stack);
}