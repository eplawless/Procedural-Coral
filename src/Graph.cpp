#include "StdAfx.h"
#include "Graph.h"

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

void triangulate( Graph &graph )
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
	typedef Triangulation::All_edges_iterator  All_edges_iterator;
	typedef Triangulation::Edge_iterator       Edge_iterator;
	typedef Triangulation::Vertex_circulator   Vertex_circulator;
	typedef Triangulation::Point               Point;
	typedef Triangulation::Segment             Segment;
	typedef All_edges_iterator::Face_handle    Face_handle;
	typedef Triangulation::Vertex_handle       Vertex_handle;
	typedef std::vector<Point>                 PointList;

	// what is wrong with you, CGAL creators? is it... idiot disease?
	std::map<Vertex_handle, int> vertexIndexMap;

	Triangulation triangulation;

	// dump all of the BGL vertices into CGAL
	Graph::vertex_iterator vi, vi_end;
	Graph::vertices_size_type numVertices = boost::num_vertices( graph );
	for ( int idx = 0; idx < numVertices; ++idx ) 
	{
		const GraphVertexInfo &vertexInfo = graph[idx];
		Point position( vertexInfo.position.x, vertexInfo.position.y );
		Vertex_handle handle = triangulation.insert(position);
		vertexIndexMap[handle] = idx;
	}

	// read out the edges
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
		boost::add_edge( idxSource, idxTarget, graph );
	}
}
