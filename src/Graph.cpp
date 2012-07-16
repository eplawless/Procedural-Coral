#include "StdAfx.h"
#include "Graph.h"

std::size_t const VertexInfoTag::num = (std::size_t)&VertexInfoTag::num;
std::size_t const EdgeInfoTag::num = (std::size_t)&EdgeInfoTag::num;

Color getVertexColor( boost::default_color_type color )
{
	switch (color) {
	case boost::white_color: return Color::white(); break;
		case boost::gray_color: return Color::gray( 0.7f ); break;
		case boost::green_color: return Color( CM_RGB, 0.0f, 1.0f, 0.0f ); break;
		case boost::red_color: return Color( CM_RGB, 1.0f, 0.0f, 0.0f ); break;
		case boost::black_color: return Color::black(); break;
		default: return Color( CM_RGB, 0.0f, 0.0f, 1.0f ); break; 
	};
}

Color getVertexColor( GraphVertexType type )
{
	static const Color coralColor( CM_RGB, 1.0f, 0.0f, 0.0f );
	static const Color airColor( CM_RGB, 0.0f, 1.0f, 0.0f );
	static const Color noneColor( CM_RGB, 0.0f, 0.0f, 1.0f );

	switch ( type ) {
	case TYPE_NONE: return noneColor;
	case TYPE_AIR: return airColor;
	case TYPE_CORAL: return coralColor;
	default: return Color( CM_RGB, 1.0f, 1.0f, 0.0f );
	}
}

void triangulate( Rand &prng, Graph &graph )
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Delaunay_triangulation_2<K>        Triangulation;
	typedef Triangulation::Edge_iterator             Edge_iterator;
	typedef Triangulation::Vertex_handle             Vertex_handle;
	typedef Triangulation::Point                     Point;
	typedef std::map<Vertex_handle, int>             VertexIndexMap;

	const static int minEdgeWeight = 1;
	const static int maxEdgeWeight = 20;

	// create the delaunay triangulation
	Triangulation triangulation;
	VertexIndexMap vertexIndexMap;

	// dump all of the BGL vertices into CGAL
	Graph::vertices_size_type numVertices = boost::num_vertices( graph );
	for ( int idx = 0; idx < numVertices; ++idx ) 
	{
		const VertexInfo &vertexInfo = boost::get( VertexInfoTag(), graph, idx );
		Point position( vertexInfo.position.x, vertexInfo.position.y );
		Vertex_handle handle = triangulation.insert( position );
		vertexIndexMap[ handle ] = idx;
	}

	// edge weight property map
	auto edgeWeightMap = boost::get( boost::edge_capacity, graph );

	// read out the edges and add them to BGL
	EdgeReverseMap edgeReverseMap = boost::get( boost::edge_reverse, graph );
	Edge_iterator ei_end = triangulation.edges_end();
	for (Edge_iterator ei = triangulation.edges_begin(); 
		ei != ei_end; ++ei)
	{
		int idxSourceInFace = ( ei->second + 2 ) % 3;
		int idxTargetInFace = ( ei->second + 1 ) % 3;
		Vertex_handle sourceVertex = ei->first->vertex( idxSourceInFace );
		Vertex_handle targetVertex = ei->first->vertex( idxTargetInFace );
		int idxSource = vertexIndexMap[ sourceVertex ];
		int idxTarget = vertexIndexMap[ targetVertex ];
		EdgeHandle forwardEdge = boost::add_edge( idxSource, idxTarget, graph );
		EdgeHandle backwardEdge = boost::add_edge( idxTarget, idxSource, graph );
		edgeReverseMap[ forwardEdge.first ] = backwardEdge.first;
		edgeReverseMap[ backwardEdge.first ] = forwardEdge.first;
		int edgeWeight = prng.nextInt( 1, 20 );
		edgeWeightMap[ forwardEdge.first ] = edgeWeight;
		edgeWeightMap[ backwardEdge.first ] = edgeWeight;
	}
}
