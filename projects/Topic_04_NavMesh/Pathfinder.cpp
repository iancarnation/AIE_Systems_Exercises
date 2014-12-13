#include "Pathfinder.h"

Pathfinder::Pathfinder(std::vector<Topic_04_NavMesh::NavNodeTri*>& a_navTriGraph)
{
	m_pathfinderGraph = a_navTriGraph;
}

Pathfinder::~Pathfinder()
{

}

// identify which triangle point is inside
Node* Pathfinder::IdentifyTriangle(vec3 a_point)
{
	for (auto node : m_pathfinderGraph)
	{
		if (PointInTriangle(a_point, node))
			return node;
	}

	return nullptr;
}

// check if point(p) is inside triangle(ABC) 
bool Pathfinder::PointInTriangle(vec3 a_p, Node* a_node)
{
	// cache triangle vertices
	vec3 A, B, C;
	A = a_node->vertices[0];
	B = a_node->vertices[1];
	C = a_node->vertices[2];

	if (SameSide(a_p,A, B, C) && SameSide(a_p,B, A, C) && SameSide(a_p,C, A, B))
		return true;

	return false;
}

// returns whether two points are on the same side of a line (for PointInTriangle function)
bool Pathfinder::SameSide(vec3 p1,vec3 p2, vec3 A,vec3 B)
{
	vec3 cp1 = glm::cross((B - A), (p1 - A));
	vec3 cp2 = glm::cross((B - A), (p2 - A));

	if (glm::dot(cp1, cp2) >= 0)
		return true;

	return false;
}

// check points' positions relative to all triangles **naming: points/nodes??**
void Pathfinder::DeterminePathing(vec3 a_start, vec3 a_end)
{
	// if both points in same triangle,
	// path can be straight line between
	if (IdentifyTriangle(a_start) == IdentifyTriangle(a_end))

		m_wayPoints[0] = a_end;
	
	// if one or both points are outside all triangles, 
	// return null path for now
	else if (IdentifyTriangle(a_end) == nullptr || IdentifyTriangle(a_start) == nullptr)
	
		m_wayPoints[0] = vec3(0);
	
	// if both points are on different triangles, 
	// proceed with pathfinding algorithm
	else
		Run(a_start, a_end);
}

// run pathfinding algorithm
void Pathfinder::Run(vec3 a_start, vec3 a_end)
{
	Node* startNode = IdentifyTriangle(a_start);
	Node* endNode = IdentifyTriangle(a_end);
	Node* currentNode = nullptr;

	// add starting node to the open list
	m_openList.push_back(startNode);

	// while the open list is not empty:
	while (m_openList.size() > 0)
	{
		// - current node = node from open list with the lowest cost
		currentNode = FindCheapest();
		// - if current node == goal node
		if (currentNode == endNode)
			// - - path complete!
			MakePath(startNode, endNode);

		else
		{
			// - - move current node to the closed list
			m_closedList.push_back(currentNode);
			m_openList.remove(currentNode);
			// - - search each adjacent node ** pre-load this information when building navMesh structure? **
			for (int i = 0; i < 3; i++)
			{
				Node* adjNode = currentNode->adjacentNodes[i];

				// - - - if it is not on the closed list:
				Node* p = std::find(m_closedList.begin, m_closedList.end, adjNode);
				if (p = m_closedList.end) // std::find reached end of iteration without finding adjNode
				{
					// - - - - if it is in the open list:
					p = std::find(m_openList.begin, m_openList.end, adjNode);
					if (p != m_closedList.end) // std::find did not reach end of iteration, thus adjNode is within
					{
						// - - - - - see if adjacent node's G score is any lower if you go through the current square to get there
						// determine G cost of possible new path
						GetGCost(currentNode, adjNode);

						if (adjNode.g > currentNode.g + adjNode.cost)
							// - - - - - if current path is better, update node 
							updateNode();
							
					}
					// - - - - if not in open list, add it and calculate cost
					else
					{
						updateNode();
						m_m_openList.push_back(adjNode);
					}
				}
			}
			
		}
		
	}
	
}

// finds node in the open list with the lowest movement cost
Node* Pathfinder::FindCheapest()
{
	// set currentLowest to first item in list
	Node* currentLowest = m_openList.front();
	// for each node in the open list:
	for (auto node : m_openList)
	{
		// - if node's f cost is lower than currentLowest:
		if (node->f < currentLowest->f)
			// - - set that node as currentLowest
			currentLowest = node;
	}

	return currentLowest;	
}

// make a path by traversing parent nodes from end to start
void Pathfinder::MakePath(Node* a_startNode, Node* a_endNode)
{
	Node* node = a_endNode;
	m_wayPoints.push_back(node->position);

	while (node->parent != a_startNode)
	{
		node = node->parent;
		m_wayPoints.push_front(node->position);
	}
}

UpdateNode(adjNodeRef, currentNode)
{
	adjNode.g = currentNode.g + adjNode.cost;
	adjNode.h = GetHeuristic(adjNode);
	adjNode.f = adjNode.g + adjNode.h;
	adjNode.parent = currentNode;	
}

// get vector between this node and goal node
GetHeuristic()
{
	
}