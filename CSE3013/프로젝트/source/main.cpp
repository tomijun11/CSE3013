#include "ofMain.h"
#include "ofApp.h"

int main() {
	//
	// for window without console
	//========================================================================
	// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	ofSetupOpenGL(1024, 768, OF_WINDOW);			// â ũ�⸦ 1024 x 768�� ������
	//ofSetupOpenGL(1024, 768, OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
