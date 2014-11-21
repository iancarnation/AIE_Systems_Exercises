// check if point(p) is inside triangle(ABC) 
PointInTriangle(p, A,B,C)
{
	if (SameSide(p,A, B,C) && SameSide(p,B, A,C) && SameSide(p,C, A,B))
		return true;
	return false;
}

// returns whether two points are on the same side of a line (for PointInTriangle function)
SameSide(p1,p2, A,B)
{
	cp1 = cross((B - A), (p1 - A));
	cp2 = cross((B - A), (p2 - A));
	if (dot(cp1, cp2) >= 0)
		return true;
	return false;
}

// run pathfinding algorithm
Run(startNode, goalNode)
{
	// add starting node to the open list
	openList.push_back(startNode);
	// while the open list is not empty:
	while (openList.getSize() > 0)
	{
		// - current node = node from open list with the lowest cost
		currentNode = FindCheapest();
		// - if current node == goal node
		if (currentNode == goalNode)
			// - - path complete!
			return makePath();

		else
		{
			// - - move current node to the closed list
			closedList.push_back(currentNode);
			openList.remove(currentNode);
			// - - search each adjacent node ** pre-load this information when building navMesh structure? **
			for (adjNode of thisNode)
			{
				// - - - if it is not on the closed list:
				if (adjNode not in closedList)
				{

					// - - - - if it is in the open list:
					if (adjNode in openList)
					{

						// - - - - - see if current path is better than the one previously found for this adjacent node
						if (adjNode.g > currentNode.g + adjNode.cost)
							// - - - - - if current path is better, update node 
							updateNode();
							
					}
					// - - - - if not in open list, add it and calculate cost
					else
					{
						updateNode();
						openList.push_back(adjNode);
					}
				}
			}
			
		}
		
	}
	
}

// finds node in the open list with the lowest movement cost
FindCheapest()
{
	// set currentLowest to first item in list
	currentLowest = openList[0];
	// for each node in the open list:
	for (node in openList)
	{
		// - if node's cost is lower than currentLowest:
		if node.cost < currentLowest.cost
			// - - set that node as currentLowest
			currentLowest = node;
	}

	return currentLowest;	
}

// make a path by traversing parent nodes from goal to start
MakePath()
{
	node = goalNode;
	path.push_back(node);
	while (node.parent != startNode)
	{
		node = node.parent;
		path.push_front(node);
	}
}

UpdateNode(adjNode, currentNode)
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