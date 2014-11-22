#ifndef __PATHFINDER_H_
# define __PATHFINDER_H_

#include <glm/glm.hpp>
#include "Topic_04_NavMesh.h"
#include <deque>
#include <list>

using glm::vec3;

typedef Topic_04_NavMesh::NavNodeTri Node;

class Pathfinder
{
public:

	Pathfinder(std::vector<Topic_04_NavMesh::NavNodeTri*>& a_navTriGraph);
	~Pathfinder();

	// find which triangle point is inside
	Node* IdentifyTriangle(vec3 a_point);
	bool PointInTriangle(vec3 a_p, Node* a_node);
	bool SameSide(vec3 p1, vec3 p2, vec3 A, vec3 B);

	// check points' positions relative to all triangles
	// and determines if the points can form a valid path
	// and if so, which pathing method to use 
	void DeterminePathing(vec3 a_start, vec3 a_end);
	// run pathfinding algorithm
	void Run(vec3 a_start, vec3 a_end);

	// finds node in the open list with the lowest movement cost
	Node* FindCheapest();
	// make a path by traversing parent nodes from end to start
	void MakePath(Node* a_startNode, Node* a_endNode);

	// collection of pathfinder nodes (pointer to main graph)
	std::vector<Node*> m_pathfinderGraph;

	std::list<Node*> m_openList;
	std::vector<Node*> m_closedList;

	std::deque<vec3> m_wayPoints;
	

};


#endif