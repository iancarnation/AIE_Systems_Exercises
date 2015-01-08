#pragma once

#include "Behavior.h"
#include <glm/glm.hpp>

class Agent
{
public:

	Agent() : m_behavior(nullptr), m_position(0), m_target(0) {}
	virtual ~Agent() {}

	const glm::vec3&	getPosition() const { return m_position; }
	const glm::vec3&	getTarget() const { return m_target; }

	void				setPosition(const glm::vec3& a_pos) { m_position = a_pos; }
	void				setTarget(const glm::vec3& a_pos) { m_target = a_pos; }

	void				setBehavior(Behavior* a_behavior) { m_behavior = a_behavior; }

	virtual void		update(float a_deltaTime)
	{
		if (m_behavior != nullptr)
			m_behavior->execute(this);
	}

private:

	Behavior* m_behavior;

	glm::vec3 m_position;
	glm::vec3 m_target;
};

class Task
{
public:

	Task() : m_complete(false) {}
	virtual ~Task() {}

	bool isComplete() const { return m_complete; }

	virtual bool preConditionsMet(Agent* a_agent) { return true; }

	virtual bool performTask(Agent* a_agent) = 0;

protected:

	bool m_complete;
};

class CompoundTask : public Task
{
public:

	CompoundTask() {}
	virtual ~CompoundTask() { for (auto task : m_task) task; }

	void addTask(Task* a_task) { m_task.push_back(a_task); }

	virtual bool performTask(Agent* a_agent)
	{
		// if task is not complete
		if (!m_complete)
		{
			if (preConditionsMet(a_agent))
			{
				for (auto task : m_task)
				{
					if (task->isComplete() == false &&
						task->performTask(a_agent) == false)
						return false;
				}

				// all tasks complete!
				m_complete == true;
			}
		}


		return m_complete;
	}

protected:

	std::vector<Task*> m_task;
};