#include "StdAfx.h"
#include "Graph.h"

class ProceduralCoralApp : public AppBasic {
private: // members
	gl::Texture m_carletonLogo;
	Graph m_graph;
	Graph::vertex_descriptor m_airSinkVertex;
	Graph::vertex_descriptor m_coralSourceVertex;
	bool m_drawLabels;
private: // methods
	void setupCarletonLogo();
	void setupGraph();
	void setupGraphSinks();
	void separateAirAndCoral();
	void removeAir();
	void removeVertex(Graph::vertex_descriptor vertex);
	void removeReverseEdges();
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
	VertexNameMap vertexNameMap = boost::get( boost::vertex_name_t(), m_graph );
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi, ++idxVertex ) 
	{
		VertexInfo &vertexInfo = boost::get( VertexInfoTag(), m_graph, *vi );
		vertexInfo.position = vertexPosition.get( idxVertex );
		vertexInfo.type = vertexInfo.position.y <= airMaximum ? TYPE_AIR :
			              vertexInfo.position.y >= coralMinimum ? TYPE_CORAL :
			              getRandomVertexType( prng, idxVertex );
		vertexNameMap[ *vi ] = boost::lexical_cast<std::string>( *vi );
	}

	// create the edges via delaunay triangulation
	triangulate( prng, m_graph );

	// pick the first and last vertices as sinks, and connect all of the
	// vertices of each type to the sinks with crazy weights
	setupGraphSinks();

	// separate air/coral
	separateAirAndCoral();

	removeAir();

	// remove source
	vertexNameMap = boost::get( boost::vertex_name_t(), m_graph );
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi ) 
	{
		if ( vertexNameMap[ *vi ] == "CoralSource" ) {
			removeVertex( *vi );
			break;
		}
	}


	removeReverseEdges();
}

void ProceduralCoralApp::setupGraphSinks()
{
	const Vec2i &windowSize = getWindowSize();

	// create air sink vertex
	VertexInfo airSinkVertexInfo;
	airSinkVertexInfo.position = Vec2i( 5, 5 );
	airSinkVertexInfo.type = TYPE_AIR;
	m_airSinkVertex = boost::add_vertex( airSinkVertexInfo, m_graph );

	// create coral sink vertex
	VertexInfo coralSinkVertexInfo;
	coralSinkVertexInfo.position = Vec2i( 5, windowSize.y - 5 );
	coralSinkVertexInfo.type = TYPE_CORAL;
	m_coralSourceVertex = boost::add_vertex( coralSinkVertexInfo, m_graph );

	VertexNameMap vertexNameMap = boost::get( boost::vertex_name_t(), m_graph );
	vertexNameMap[ m_coralSourceVertex ] = "CoralSource";
	vertexNameMap[ m_airSinkVertex ] = "AirSink";

	auto edgeWeightMap = boost::get( boost::edge_capacity, m_graph );
	auto edgeReverseMap = boost::get( boost::edge_reverse, m_graph );

	// create high-weight edges between sinks and the members of each set
	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi )
	{
		VertexInfo &vertexInfo = boost::get( VertexInfoTag(), m_graph, *vi );
		if ( vertexInfo.type == TYPE_NONE ) {
			continue;
		}

		EdgeHandle edge, reverseEdge;
		if ( vertexInfo.type == TYPE_AIR ) {
			edge = boost::add_edge( *vi, m_airSinkVertex, m_graph );
			reverseEdge = boost::add_edge( m_airSinkVertex, *vi, m_graph );
		} else if ( vertexInfo.type == TYPE_CORAL ) {
			edge = boost::add_edge( m_coralSourceVertex, *vi, m_graph );
			reverseEdge = boost::add_edge( *vi, m_coralSourceVertex, m_graph );
		}

		edgeReverseMap[ edge.first ] = reverseEdge.first;
		edgeReverseMap[ reverseEdge.first ] = edge.first;

		edgeWeightMap[ edge.first ] = 99999;
		edgeWeightMap[ reverseEdge.first ] = 99999;
	}
}

void ProceduralCoralApp::separateAirAndCoral()
{
	boost::boykov_kolmogorov_max_flow(
		m_graph,
		m_coralSourceVertex,
		m_airSinkVertex );
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

	VertexColorMap vertexColorMap = boost::get( boost::vertex_color, m_graph );

	// draw edges
	Graph::edge_iterator ei, ei_end;
	for ( boost::tie( ei, ei_end ) = boost::edges( m_graph ); 
		ei != ei_end; ++ei )
	{
		auto sourceVertex = boost::source( *ei, m_graph );
		auto targetVertex = boost::target( *ei, m_graph );
		const VertexInfo &sourceInfo = boost::get( VertexInfoTag(), m_graph, sourceVertex );
		const VertexInfo &targetInfo = boost::get( VertexInfoTag(), m_graph, targetVertex );
		gl::color( sourceInfo.type == targetInfo.type ?
			getVertexColor( sourceInfo.type ) : edgeColor );
		Vec2i scaledSourcePosition = sourceInfo.position * graphScale;
		Vec2i scaledTargetPosition = targetInfo.position * graphScale;
		gl::drawLine( scaledSourcePosition, scaledTargetPosition );
	}

	// draw vertices
	VertexNameMap vertexNameMap = boost::get( boost::vertex_name_t(), m_graph );
	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi ) 
	{
		const VertexInfo &vertexInfo = boost::get( VertexInfoTag(), m_graph, *vi );
		gl::color( getVertexColor( vertexInfo.type ) );
		Vec2i scaledPosition = vertexInfo.position * graphScale;
		gl::drawSolidCircle( scaledPosition, scaledRadius );
		if (m_drawLabels) {
			gl::drawStringCentered( vertexNameMap[*vi], 
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

void ProceduralCoralApp::removeAir()
{
	typedef std::vector<Graph::vertex_descriptor> VertexList;

	VertexColorMap vertexColorMap = boost::get( boost::vertex_color, m_graph );
	VertexList verticesToRemove;

	// mark vertices to remove
	Graph::vertex_iterator vi, vi_end;
	for ( boost::tie( vi, vi_end ) = boost::vertices( m_graph );
		vi != vi_end; ++vi )
	{
		// don't remove coral vertices
		if ( vertexColorMap[ *vi ] == boost::black_color ) {
			continue;
		}

		// mark vertex to be removed
		verticesToRemove.push_back( *vi );
	}

	// remove vertices
	VertexList::reverse_iterator it, it_end;
	for ( it = verticesToRemove.rbegin(), it_end = verticesToRemove.rend();
		it != it_end; ++it )
	{
		removeVertex( *it );
	}
}

void ProceduralCoralApp::removeVertex( Graph::vertex_descriptor vertex )
{
	typedef std::vector<Graph::edge_descriptor> EdgeList;

	auto sourceOutEdges = boost::out_edges( vertex, m_graph );
	auto sourceInEdges = boost::in_edges( vertex, m_graph );
	auto out_ei = sourceOutEdges.first;
	auto out_ei_end = sourceOutEdges.second;
	auto in_ei = sourceInEdges.first;
	auto in_ei_end = sourceInEdges.second;

	EdgeList edgesToRemove;
	int numInEdges = std::distance( in_ei, in_ei_end );
	int numOutEdges = std::distance( out_ei, out_ei_end );
	edgesToRemove.reserve( numInEdges + numOutEdges );

	// get the edges to remove
	while ( out_ei != out_ei_end ) {
		edgesToRemove.push_back( *out_ei );
		++out_ei;
	}
	while ( in_ei != in_ei_end ) {
		edgesToRemove.push_back( *in_ei );
		++in_ei;
	}

	// remove edges
	EdgeList::iterator it, it_end = edgesToRemove.end();
	for ( it = edgesToRemove.begin(); it != it_end; ++it ) {
		auto source = boost::source( *it, m_graph );
		auto target = boost::target( *it, m_graph );
		boost::remove_edge( source, target, m_graph );
	}

	// remove vertex
	boost::remove_vertex( vertex, m_graph );
}

void ProceduralCoralApp::removeReverseEdges()
{
}




CINDER_APP_BASIC( ProceduralCoralApp, RendererGl( RendererGl::AA_MSAA_8 ) )
