#include "renderer.hpp"

Renderer * Renderer::linkedListHead = 0;

Renderer::Renderer()
{
  linkedListNext = linkedListHead;
  linkedListHead = this;
}

Renderer::~Renderer()
{
  if (linkedListHead == this) {
    linkedListHead = linkedListNext;
  } else {
    // walk list to remove this
    Renderer *p = linkedListHead;
    while (p->linkedListNext != this)
      p = p->linkedListNext;
    p->linkedListNext = this->linkedListNext;
  }
}

void Renderer::resizeAll(int w, int h)
{
  Renderer *p = linkedListHead;
  while (p) {
    p->resize(w,h);
    p=p->linkedListNext;
  }
}