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
