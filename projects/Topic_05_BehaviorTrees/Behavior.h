#pragma once

#include <vector>

class Agent;

class Behavior
{
public:

	Behavior() {}
	virtual ~Behavior() {}

	virtual bool execute(Agent* a_agent) = 0; // the =0 apparently requires you to overload the method, forcing you to make a class which derives from this one, not instantiating this class itself
};

class Composite : public Behavior
{
public:

	Composite() {}
	virtual ~Composite() {}

	void addChild(Behavior* a_behavior) { m_children.push_back(a_behavior); }

protected:

	std::vector<Behavior*> m_children;
};

// executes OR logic
class Selector : public Composite
{
public:

	Selector() {}
	virtual ~Selector() {}

	virtual bool execute(Agent* a_agent)
	{
		for (auto behavior : m_children)
		{
			if (behavior->execute(a_agent) == true)
				return true;
		}

		return false;
	}
};

// executes AND logic
class Sequence : public Composite
{
public:

	Sequence() {}
	virtual ~Sequence() {}

	virtual bool execute(Agent* a_agent)
	{
		for (auto behavior : m_children)
		{
			if (behavior->execute(a_agent) == false)
				return false;
		}

		return true;
	}
};