#include "Resources.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder;

class ProceduralCoralApp : public AppBasic {
private: // members
	gl::Texture m_carletonLogo;
private: // methods
	void drawLogo();
public: // methods
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ProceduralCoralApp::setup()
{
	// set opengl options
	gl::enableAlphaBlending();

	// load resources
	m_carletonLogo = gl::Texture( loadImage( loadResource(RES_CARLETON_LOGO) ) );
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
}

void ProceduralCoralApp::update()
{
}

void ProceduralCoralApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) ); 
	drawLogo();
}

void ProceduralCoralApp::drawLogo()
{
	Area logoBounds = m_carletonLogo.getBounds();

	float logoScale = 0.5f;
	logoBounds.setX2( logoBounds.getX2() * logoScale );
	logoBounds.setY2( logoBounds.getY2() * logoScale );

	const Vec2i &windowSize = getWindowSize();
	const Vec2i &logoSize = logoBounds.getSize();
	Vec2i margin( 10, 10 );
	logoBounds.moveULTo( windowSize - logoSize - margin );

	gl::draw( m_carletonLogo, logoBounds );
}


CINDER_APP_BASIC( ProceduralCoralApp, RendererGl )
