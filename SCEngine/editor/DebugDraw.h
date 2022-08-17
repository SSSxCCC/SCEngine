#ifndef _DebugDraw_H_
#define _DebugDraw_H_

#include <memory>
#include "box2d/box2d.h"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw {
public:
	DebugDraw();
	~DebugDraw();
	void Create(std::shared_ptr<GameWorld> gameWorld);
	void Destroy();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

	void DrawString(int x, int y, const char* string, ...);
	void DrawString(const b2Vec2& pw, const char* string, ...);
	void DrawAABB(b2AABB* aabb, const b2Color& c);

	void Flush();

	bool m_showUI;
	GLRenderPoints* m_points;
	GLRenderLines* m_lines;
	GLRenderTriangles* m_triangles;

	std::shared_ptr<GameWorld> mGameWorld;
};

#endif // _DebugDraw_H_
