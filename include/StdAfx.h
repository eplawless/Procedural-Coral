#pragma once

#include "Resources.h"

#include <cinder/app/AppBasic.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ImageIo.h>
#include <cinder/Rand.h>

#include <CGAL/basic.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <boost/lexical_cast.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/unordered_map.hpp>

#include <utility>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <map>

// - using declarations:
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder;
