// types:

enum GraphVertexType {
	TYPE_CORAL,
	TYPE_AIR,
	TYPE_NONE,
	NUM_TYPES
};

typedef std::pair<int, int> Edge;

struct VertexProperties {
	Vec2i position;
	std::string name;
	GraphVertexType type;
};

struct EdgeProperties {
	int weight;
};

typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	VertexProperties,
	EdgeProperties> Graph;

typedef std::pair<Graph::edge_descriptor, bool> EdgeHandle;
typedef boost::property_map<Graph, int EdgeProperties::*>::type EdgeWeightMap;

// methods:

Color getVertexColor( GraphVertexType type );
void triangulate( Rand &prng, Graph &graph );
