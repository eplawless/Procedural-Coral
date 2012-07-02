#include "StdAfx.h"
#include "Graph.h"

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
	void prepareSettings( Settings *settings );
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
	Rand prng( time( NULL ) );
	const Vec2i &windowSize = getWindowSize();
	const Graph::vertices_size_type numVertices = 1000;
	const Vec2i margin( 20, 20 );
	const int airMaximum = margin.y + 20;
	const int coralMinimum = windowSize.y - margin.y - 30;

	// initialize the graph
	m_graph = Graph( numVertices );

	// set vertex properties
	Graph::vertex_iterator vi, vi_end; 
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi ) 
	{
		GraphVertexInfo &vertexInfo = m_graph[*vi];
		int x = prng.nextInt( margin.x, windowSize.x - margin.x );
		int y = prng.nextInt( margin.y, windowSize.y - margin.y );
		vertexInfo.position = Point( x, y );
		vertexInfo.name = boost::lexical_cast<std::string>( *vi );
		vertexInfo.type = y <= airMaximum ?   TYPE_AIR :
			              y >= coralMinimum ? TYPE_CORAL :
			                                  TYPE_NONE;
	}
}

void ProceduralCoralApp::setup()
{
	gl::enableAlphaBlending();
	setupCarletonLogo();
	setupGraph();
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
	m_drawLabels = !m_drawLabels;
}

void ProceduralCoralApp::update()
{
	// TODO: build/animate delaunay triangulation
	// TODO: build/animate min-cuts
}

void ProceduralCoralApp::drawGraph()
{
	typedef Graph::vertices_size_type num_vertices_t;
	typedef Graph::edges_size_type num_edges_t;

	static const float radius = 1.0f;
	static const float graphScale = 1.0f;
	static const float scaledRadius = radius * graphScale;
	static const Color edgeColor( CM_RGB, 1.0f, 0.8f, 0.0f );
	static const Vec2i textOffset( 0, static_cast<int>( scaledRadius ) );
	static const Color textColor( CM_RGB, 0.0f, 0.4f, 0.9f );
	static const Font font( "Arial", 16 * graphScale );

	// draw edges
	gl::color( edgeColor );
	Graph::edge_iterator ei, ei_end;
	for ( boost::tie( ei, ei_end ) = boost::edges( m_graph ); 
		ei != ei_end; ++ei )
	{
		const GraphVertexInfo &sourceInfo = m_graph[ei->m_source];
		const GraphVertexInfo &targetInfo = m_graph[ei->m_target];
		Point scaledSourcePosition = sourceInfo.position * graphScale;
		Point scaledTargetPosition = targetInfo.position * graphScale;
		gl::drawLine( scaledSourcePosition, scaledTargetPosition );
	}

	// draw vertices
	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi ) 
	{
		const GraphVertexInfo &vertexInfo = m_graph[*vi];
		gl::color( getVertexColor( vertexInfo.type ) );
		Point scaledPosition = vertexInfo.position * graphScale;
		gl::drawSolidCircle( scaledPosition, scaledRadius );
		if (m_drawLabels) {
			gl::drawStringCentered( vertexInfo.name, 
				scaledPosition + textOffset, textColor, font );
		}
	}
}

void ProceduralCoralApp::drawCarletonLogo()
{
	Area logoBounds = m_carletonLogo.getBounds();

	const float logoScale = 0.75f;
	logoBounds.setX2( static_cast<int>( logoBounds.getX2() * logoScale ) );
	logoBounds.setY2( static_cast<int>( logoBounds.getY2() * logoScale ) );

	const Vec2i &windowSize = getWindowSize();
	const Vec2i &logoSize = logoBounds.getSize();
	Vec2i margin( 10, 10 );
	logoBounds.moveULTo( windowSize - logoSize - margin );

	gl::color( Color( CM_RGB, 1.0f, 1.0f, 1.0f ) );
	gl::draw( m_carletonLogo, logoBounds );
}

void ProceduralCoralApp::draw()
{
	const float lightness = 0.1f;
	gl::clear( Color( CM_HSV, 0, 0, lightness ) ); 

	drawCarletonLogo();
	drawGraph();
}

void ProceduralCoralApp::prepareSettings( Settings *settings )
{
	m_drawLabels = false;
	settings->setResizable( false );
	settings->setTitle( "Procedural Coral - Eric Lawless" );
}



CINDER_APP_BASIC( ProceduralCoralApp, RendererGl( RendererGl::AA_MSAA_8 ) )
