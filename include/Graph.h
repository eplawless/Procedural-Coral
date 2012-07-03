// types:

enum GraphVertexType {
	TYPE_CORAL,
	TYPE_AIR,
	TYPE_NONE,
	NUM_TYPES
};

typedef std::pair<int, int> Edge;
struct GraphVertexInfo {
public: // members
	Vec2i position;
	std::string name;
	GraphVertexType type;
public: // methods
	GraphVertexInfo() 
		: position( 0, 0 )
		, name( "" )
		, type( TYPE_NONE )
	{
	}
};

typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	GraphVertexInfo> Graph;

// methods:

Color getVertexColor( GraphVertexType type );
void triangulate( Graph &graph );
