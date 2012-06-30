#include "Resources.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "CGAL/basic.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder;

class ProceduralCoralApp : public AppBasic {
private: // members
	gl::Texture m_carletonLogo;
private: // methods
	void drawCarletonLogo();
public: // methods
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ProceduralCoralApp::setup()
{
	gl::enableAlphaBlending();

	const DataSourceRef &logoResource = loadResource( RES_CARLETON_LOGO );
	const ImageSourceRef &logoImage = loadImage( logoResource );
	m_carletonLogo = gl::Texture( logoImage );
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
}

void ProceduralCoralApp::update()
{
}

void ProceduralCoralApp::draw()
{
	Color gray( 0.5f, 0.5f, 0.5f );
	gl::clear( gray ); 
	drawCarletonLogo();
}

void ProceduralCoralApp::drawCarletonLogo()
{
	Area logoBounds = m_carletonLogo.getBounds();

	float logoScale = 0.75f;
	logoBounds.setX2( static_cast<int>( logoBounds.getX2() * logoScale ) );
	logoBounds.setY2( static_cast<int>( logoBounds.getY2() * logoScale ) );

	const Vec2i &windowSize = getWindowSize();
	const Vec2i &logoSize = logoBounds.getSize();
	Vec2i margin( 10, 10 );
	logoBounds.moveULTo( windowSize - logoSize - margin );

	gl::draw( m_carletonLogo, logoBounds );
}


CINDER_APP_BASIC( ProceduralCoralApp, RendererGl )
