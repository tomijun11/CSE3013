#include "ofMain.h"
#include "ofApp.h"

int main() {
	//
	// for window without console
	//========================================================================
	// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	ofSetupOpenGL(1024, 768, OF_WINDOW);			// 창 크기를 1024 x 768로 설정함
	//ofSetupOpenGL(1024, 768, OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
