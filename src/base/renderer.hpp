#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer {
	static Renderer * linkedListHead;
	Renderer * linkedListNext;

public:
	static void onResizeAll(int w, int h);
	static void onKeyAll(int key, int action);
	static void onCharAll(int character, int action);
	static void onMouseButtonAll(int button, int action);
	static void onMouseMoveAll(int x, int y);
	static void onMouseWheelAll(int pos);

	Renderer();
	~Renderer();

	virtual void render() =0;

	virtual void onResize(int w, int h) {};
	virtual void onKey(int key, int action) {};
	virtual void onChar(int character, int action) {};
	virtual void onMouseButton(int button, int action) {};
	virtual void onMouseMove(int x, int y) {};
	virtual void onMouseWheel(int pos) {};
};

#endif//RENDERER_HPP
