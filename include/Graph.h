// types:

enum GraphVertexType {
	TYPE_CORAL,
	TYPE_AIR,
	TYPE_NONE,
	NUM_TYPES
};

typedef std::pair<int, int> Edge;

typedef boost::adjacency_list_traits <
	boost::vecS, 
	boost::vecS, 
	boost::bidirectionalS >
	Traits;

// vertex properties
struct VertexInfo {
	Vec2i position;
	GraphVertexType type;
};
struct VertexInfoTag
{
    typedef boost::vertex_property_tag kind;
    static std::size_t const num;
};
typedef boost::property<VertexInfoTag, VertexInfo,
	boost::property<boost::vertex_name_t, std::string,
	boost::property<boost::vertex_index_t, long,
	boost::property<boost::vertex_color_t, boost::default_color_type,
	boost::property<boost::vertex_distance_t, long,
	boost::property<boost::vertex_predecessor_t, Traits::edge_descriptor
	> > > > > >
	VertexProperties;

// edge properties
struct EdgeInfo {
	EdgeInfo() : isReverse(false) {}
	bool isReverse;
};
struct EdgeInfoTag {
    typedef boost::edge_property_tag kind;
    static std::size_t const num;
};
typedef boost::property<boost::edge_capacity_t, int,
	boost::property<boost::edge_residual_capacity_t, int,
	boost::property<boost::edge_reverse_t, Traits::edge_descriptor,
	boost::property<EdgeInfoTag, EdgeInfo
	> > > >
	EdgeProperties;

typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	VertexProperties,
	EdgeProperties> Graph;

typedef boost::property_map<Graph, boost::vertex_name_t>::type VertexNameMap;
typedef boost::property_map<Graph, boost::vertex_color_t>::type VertexColorMap;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type EdgeReverseMap;

typedef std::pair<Graph::edge_descriptor, bool> EdgeHandle;

// methods:

Color getVertexColor( GraphVertexType type );
Color getVertexColor( boost::default_color_type color );
void triangulate( Rand &prng, Graph &graph );
