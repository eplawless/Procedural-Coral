#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ProceduralCoralApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ProceduralCoralApp::setup()
{
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
}

void ProceduralCoralApp::update()
{
}

void ProceduralCoralApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( ProceduralCoralApp, RendererGl )
