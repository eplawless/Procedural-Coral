#include "StdAfx.h"

class ProceduralCoralApp : public AppBasic {
private: // members
	gl::Texture m_carletonLogo;
	Graph m_graph;
	bool m_drawLabels;
private: // methods
	void setupCarletonLogo();
	void setupGraph();
	void drawCarletonLogo();
	void drawGraph();
public: // methods
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ProceduralCoralApp::setupCarletonLogo()
{
	const DataSourceRef &logoResource = loadResource( RES_CARLETON_LOGO );
	const ImageSourceRef &logoImage = loadImage( logoResource );
	m_carletonLogo = gl::Texture( logoImage );
}

void ProceduralCoralApp::setupGraph()
{
	// Make convenient labels for the vertices
	enum { A, B, C, D, E, N };
	const int numVertices = N;
	const char* name = "ABCDE";

	// writing out the edges in the graph
	Edge edgeArray[] = {
		Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C), 
		Edge(C,E), Edge(B,D), Edge(D,E) };
	const int numEdges = sizeof( edgeArray ) / sizeof( Edge );

	// initialize the graph
	m_graph = Graph( edgeArray, edgeArray + numEdges, numVertices );

	// set vertex properties
	for ( int idx = 0; idx < numVertices; ++idx ) {
		int x = 30 + 50 * ( idx % 3 );
		int y = 10 + 30 * ( idx / 3 );
		m_graph[idx].position = Point( x, y );

		std::stringstream nameStream;
		nameStream << "Vertex " << idx;
		m_graph[idx].name = nameStream.str();
	}
}

void ProceduralCoralApp::setup()
{
	gl::enableAlphaBlending();
	glEnable(GL_MULTISAMPLE);
	setupCarletonLogo();
	setupGraph();
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
	m_drawLabels = !m_drawLabels;
}

void ProceduralCoralApp::update()
{
}

void ProceduralCoralApp::drawGraph()
{
	typedef Graph::vertices_size_type num_t;
	const float radius = 5.0f;
	const float graphScale = 1.0f;
	const float scaledRadius = radius * graphScale;
	const Vec2i textOffset( 0, static_cast<int>( scaledRadius ) );
	const Color textColor( cinder::CM_RGB, 0.0f, 0.4f, 0.9f );
	const Font font( "Arial", 16 * graphScale );

	// draw vertices
	num_t numVertices = boost::num_vertices( m_graph );
	for ( num_t idx = 0; idx < numVertices; ++idx ) {
		const Point &position = m_graph[idx].position;
		Point scaledPosition = position * graphScale;
		gl::drawSolidCircle( scaledPosition, scaledRadius );
		if (m_drawLabels) {
			const std::string &name = m_graph[idx].name;
			gl::drawStringCentered( name, scaledPosition + textOffset, 
				textColor, font );
		}
	}
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

void ProceduralCoralApp::draw()
{
	Color gray( 0.5f, 0.5f, 0.5f );
	gl::clear( gray ); 
	drawCarletonLogo();
	drawGraph();
}


CINDER_APP_BASIC( ProceduralCoralApp, RendererGl( RendererGl::AA_MSAA_8 ) )
