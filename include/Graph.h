enum GraphVertexType {
	TYPE_CORAL,
	TYPE_AIR,
	TYPE_NONE,
	NUM_TYPES
};

Color getVertexColor( GraphVertexType type );

typedef std::pair<int, int> Edge;
typedef cinder::Vec2i Point;

struct GraphVertexInfo {
public: // members
	Point position;
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
