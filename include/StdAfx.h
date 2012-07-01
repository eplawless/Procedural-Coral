#pragma once

#include "Resources.h"

#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>

#include <CGAL/basic.h>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <utility>
#include <string>
#include <sstream>

// - using declarations:
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder;

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