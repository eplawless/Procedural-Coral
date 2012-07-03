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
	static const Graph::vertices_size_type numVertices = 1000;
	static const int marginLeft = 15;
	static const int marginTop = 15;
	static const int marginRight = 15;
	static const int marginBottom = 65;
	static const int airMaximum = marginTop + 30;
	static const int coralMinimum = windowSize.y - marginBottom - 30;
	static const int numFixedAirVertices = 10;
	static const int numFixedCoralVertices = 10;

	// set up the graph
	m_graph = Graph( numVertices );

	// helper function for vertex position
	auto getVertexPosition = [&]( int idxVertex ) -> Vec2i {
		static const int gridResolution = 4;
		int x, y;
		if ( idxVertex <= numFixedAirVertices ) {
			float xScale = ( ( 1.0f * idxVertex ) / numFixedAirVertices );
			x = marginLeft + ( xScale * ( windowSize.x - marginLeft - marginRight ) );
			y = marginTop;
		} else if ( idxVertex >= numVertices - numFixedCoralVertices - 1 ) {
			float xScale = ( ( 1.0f * numVertices - idxVertex - 1 ) 
				/ numFixedCoralVertices );
			x = marginLeft + ( xScale * ( windowSize.x - marginLeft - marginRight ) );
			y = windowSize.y - marginBottom;
		} else {
			x = prng.nextInt( marginLeft, windowSize.x - marginRight );
			y = prng.nextInt( marginTop, windowSize.y - marginBottom );
		}
		x = ( x / gridResolution ) * gridResolution;
		y = ( y / gridResolution ) * gridResolution;
		return Vec2i( x, y );
	};

	// set up each vertex
	int idxVertex = 0;
	Graph::vertex_iterator vi, vi_end; 
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi, ++idxVertex ) 
	{
		GraphVertexInfo &vertexInfo = m_graph[*vi];
		vertexInfo.position = getVertexPosition( idxVertex );
		vertexInfo.name = boost::lexical_cast<std::string>( *vi );
		vertexInfo.type = vertexInfo.position.y <= airMaximum ? TYPE_AIR :
			              vertexInfo.position.y >= coralMinimum ? TYPE_CORAL :
			              TYPE_NONE;
	}

	// create the edges via delaunay triangulation
	triangulate( m_graph );
}

void ProceduralCoralApp::setup()
{
	gl::enableAlphaBlending();
	setupCarletonLogo();
	setupGraph();
}

void ProceduralCoralApp::mouseDown( MouseEvent event )
{
	setupGraph();
}

void ProceduralCoralApp::update()
{
	// TODO: build/animate min-cuts
}

void ProceduralCoralApp::drawGraph()
{
	typedef Graph::vertices_size_type num_vertices_t;
	typedef Graph::edges_size_type num_edges_t;

	static const float radius = 2.0f;
	static const float graphScale = 1.0f;
	static const float scaledRadius = radius * graphScale;
	static const ColorA edgeColor( CM_RGB, 1.0f, 0.8f, 0.0f, 0.2f );
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
		Vec2i scaledSourcePosition = sourceInfo.position * graphScale;
		Vec2i scaledTargetPosition = targetInfo.position * graphScale;
		gl::drawLine( scaledSourcePosition, scaledTargetPosition );
	}

	// draw vertices
	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi ) 
	{
		const GraphVertexInfo &vertexInfo = m_graph[*vi];
		gl::color( getVertexColor( vertexInfo.type ) );
		Vec2i scaledPosition = vertexInfo.position * graphScale;
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

	gl::color( Color::white() );
	gl::draw( m_carletonLogo, logoBounds );
}

void ProceduralCoralApp::draw()
{
	// clear to background color
	const float lightness = 0.2f;
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
