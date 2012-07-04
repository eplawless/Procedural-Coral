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
	void setupGraphSinks();
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

class VertexPositionHelper
{
public: // methods
	VertexPositionHelper(
		Rand &prng,
		int numVertices,
		int marginTop,
		int marginBottom,
		int marginLeft,
		int marginRight,
		const Vec2i &windowSize )
		: m_prng( prng )
		, m_numVertices( numVertices )
		, m_marginTop( marginTop )
		, m_marginBottom( marginBottom )
		, m_marginLeft( marginLeft )
		, m_marginRight( marginRight )
		, m_windowSize( windowSize )
	{
	}

	Vec2i get( int idxVertex )
	{
		static const int gridResolution = 4;
		static const int numFixedAirVertices = 10;
		static const int numFixedCoralVertices = 10;

		int x, y;
		if ( idxVertex <= numFixedAirVertices ) {
			float xScale = ( ( 1.0f * idxVertex ) / numFixedAirVertices );
			x = m_marginLeft + ( xScale * 
				( m_windowSize.x - m_marginLeft - m_marginRight ) );
			y = m_marginTop;
		} else if ( idxVertex >= m_numVertices - numFixedCoralVertices - 1 ) {
			float xScale = ( ( 1.0f * m_numVertices - idxVertex - 1 ) 
				/ numFixedCoralVertices );
			x = m_marginLeft + ( xScale * 
				( m_windowSize.x - m_marginLeft - m_marginRight ) );
			y = m_windowSize.y - m_marginBottom;
		} else {
			x = m_prng.nextInt( m_marginLeft, m_windowSize.x - m_marginRight );
			y = m_prng.nextInt( m_marginTop, m_windowSize.y - m_marginBottom );
		}
		x = ( x / gridResolution ) * gridResolution;
		y = ( y / gridResolution ) * gridResolution;
		return Vec2i( x, y );
	}

private: // members
	Rand &m_prng;
	int m_numVertices;
	int m_marginTop;
	int m_marginBottom;
	int m_marginLeft;
	int m_marginRight;
	const Vec2i &m_windowSize;
};

static GraphVertexType getRandomVertexType(
	Rand &prng,
	int idxVertex )
{
	static const int markedVertexPercent = 10;
	bool shouldGiveType = prng.nextInt(0, 100) < markedVertexPercent;
	if ( !shouldGiveType ) {
		return TYPE_NONE;
	}
	return static_cast<GraphVertexType>( prng.nextInt( NUM_TYPES - 1 ) );
}

void ProceduralCoralApp::setupGraph()
{
	Rand prng( static_cast<uint32_t>( time( NULL ) ) );
	static const Graph::vertices_size_type numVertices = 2000;
	const Vec2i &windowSize = getWindowSize();
	static const int marginTop = 15;
	static const int marginBottom = 65;
	static const int marginLeft = 15;
	static const int marginRight = 10;
	static const int airMaximum = marginTop + 30;
	static const int coralMinimum = windowSize.y - marginBottom - 30;

	// capture variables
	VertexPositionHelper vertexPosition( prng, numVertices, marginTop,
		marginBottom, marginLeft, marginRight, windowSize );

	// set up the graph
	m_graph = Graph( numVertices );

	// set up each vertex
	int idxVertex = 0;
	Graph::vertex_iterator vi, vi_end; 
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi, ++idxVertex ) 
	{
		GraphVertexInfo &vertexInfo = m_graph[*vi];
		vertexInfo.position = vertexPosition.get( idxVertex );
		vertexInfo.name = boost::lexical_cast<std::string>( *vi );
		vertexInfo.type = vertexInfo.position.y <= airMaximum ? TYPE_AIR :
			              vertexInfo.position.y >= coralMinimum ? TYPE_CORAL :
			              getRandomVertexType( prng, idxVertex );
	}

	// create the edges via delaunay triangulation
	triangulate( m_graph );

	// pick the first and last vertices as sinks, and connect all of the
	// vertices of each type to the sinks with crazy weights
	setupGraphSinks();
}

void ProceduralCoralApp::setupGraphSinks()
{
	Graph::vertex_descriptor airSinkVertex, coralSinkVertex;
	const Vec2i &windowSize = getWindowSize();

	GraphVertexInfo airSinkVertexInfo;
	airSinkVertexInfo.position = Vec2i( 5, 5 );
	airSinkVertexInfo.name = "AirSink";
	airSinkVertexInfo.type = TYPE_AIR;
	airSinkVertex = boost::add_vertex( airSinkVertexInfo, m_graph );

	GraphVertexInfo coralSinkVertexInfo;
	coralSinkVertexInfo.position = Vec2i( 5, windowSize.y - 5 );
	coralSinkVertexInfo.name = "coralSink";
	coralSinkVertexInfo.type = TYPE_CORAL;
	coralSinkVertex = boost::add_vertex( coralSinkVertexInfo, m_graph );

	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi )
	{
		GraphVertexInfo &vertexInfo = m_graph[*vi];
		if ( vertexInfo.type == TYPE_AIR ) {
			boost::add_edge( airSinkVertex, *vi, m_graph );
		} else if ( vertexInfo.type == TYPE_CORAL ) {
			boost::add_edge( coralSinkVertex, *vi, m_graph );
		}
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
	Graph::edge_iterator ei, ei_end;
	for ( boost::tie( ei, ei_end ) = boost::edges( m_graph ); 
		ei != ei_end; ++ei )
	{
		const GraphVertexInfo &sourceInfo = m_graph[ei->m_source];
		const GraphVertexInfo &targetInfo = m_graph[ei->m_target];
		gl::color( sourceInfo.type == targetInfo.type ?
			getVertexColor( sourceInfo.type ) : edgeColor );
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
