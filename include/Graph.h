// - typedefs: 
typedef std::pair<int, int> Edge;
typedef cinder::Vec2i Point;
struct GraphVertexInfo {
	Point position;
	std::string name;
};
typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	GraphVertexInfo> Graph;
