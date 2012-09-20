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

void Renderer::onResizeAll(int w, int h)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onResize(w,h);
		p=p->linkedListNext;
	}
}

void Renderer::onKeyAll(int key, int action)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onKey(key, action);
		p=p->linkedListNext;
	}
}

void Renderer::onCharAll(int character, int action)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onChar(character, action);
		p=p->linkedListNext;
	}
}

void Renderer::onMouseButtonAll(int button, int action)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onMouseButton(button, action);
		p=p->linkedListNext;
	}
}

void Renderer::onMouseMoveAll(int x, int y)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onMouseMove(x,y);
		p=p->linkedListNext;
	}
}

void Renderer::onMouseWheelAll(int pos)
{
	Renderer *p = linkedListHead;
	while (p) {
		p->onMouseWheel(pos);
		p=p->linkedListNext;
	}
}
