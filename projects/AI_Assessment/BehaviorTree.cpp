/******************************************************************************
* This file is part of the Behavior Tree Starter Kit.
*
* Copyright (c) 2012, AiGameDev.com.
*
* Credits: Alex J. Champandard
*****************************************************************************/

#include <vector>
#include "Shared.h"
#include "Test.h"

namespace bt1
{

	// ===========================================================================

	enum Status
		/**
		* Return values of and valid states for behaviors
		*/
	{
		INVALID,
		SUCCESS,
		FAILURE,
		RUNNING,
		ABORTED,
	};

	class Behavior
		/**
		* Base class for actions, conditions and composites
		*/
	{
	public:
		virtual Status update() = 0;

		virtual void onInitialize()			{}
		virtual void onTerminate(Status)	{}

		Behavior()
			: status(INVALID)
		{
		}

		virtual ~Behavior()
		{
		}

		Status tick()
		{
			if (status != RUNNING)
			{
				onInitialize();
			}

			status = update();

			if (status != RUNNING)
			{
				onTerminate(status);
			}
			return status;
		}

		void reset()
		{
			status = INVALID;
		}

		void abort()
		{
			onTerminate(ABORTED);
			status = ABORTED;
		}

		bool isTerminated() const // why const??
		{
			return status == SUCCESS || status == FAILURE;
		}

		bool isRunning() const
		{
			return status == RUNNING;
		}

		Status getStatus() const
		{
			return status;
		}

	private:
		Status status;
	};

	// -------------------------------------------------------------------

	struct MockBehavior : public Behavior
	{
		int initializeCalled;
		int terminateCalled;
		int updateCalled;
		Status returnStatus;
		Status terminateStatus;

		MockBehavior()
			: initializeCalled(0)
			, terminateCalled(0)
			, updateCalled(0)
			, returnStatus(RUNNING)
			, terminateStatus(INVALID)
		{
		}

		virtual ~MockBehavior()
		{
		}

		virtual void onInitialize()
		{
			++initializeCalled;
		}

		virtual void onTerminate(Status s)
		{
			++terminateCalled;
			terminateStatus = s;
		}

		virtual Status update()
		{
			++updateCalled;
			return returnStatus;
		}
	};

	TEST(StarterKit1, TaskInitialize)
	{
		MockBehavior t;
		CHECK_EQUAL(0, t.initializeCalled);

		t.tick();
		CHECK_EQUAL(1, t.initializeCalled);
	};

	TEST(StarterKit1, TaskUpdate)
	{
		MockBehavior t;
		CHECK_EQUAL(0, t.updateCalled);

		t.tick();
		CHECK_EQUAL(1, t.updateCalled);
	};

	TEST(StarterKit1, TaskInitialize)
	{
		MockBehavior t;
		CHECK_EQUAL(0, t.terminateCalled);

		t.tick();
		CHECK_EQUAL(1, t.terminateCalled);
	};

	// ==========================================================================

	class Decorator : public Bahavior
	{
	protected:
		Behavior* child;

	public:
		Decorator(Behavior* a_child) : child(a_child) {}
	};

	class Repeat : public Decorator
	{
	public:
		Repeat(Behavior* a_child)
			: Decorator(a_child)
		{
		}

		void setCount(int count)
		{
			limit = count;
		}

		void onInitialize()
		{
			counter = 0;
		}

		Status update()
		{
			for (;;)
			{
				child->tick();
				if (child->getStatus() == RUNNING) break;
				if (child->getStatus() == FAILURE) return FAILURE;
				if (++counter == limit) return SUCCESS;
				child->reset();
			}
			return INVALID;
		}

	protected:
		int limit;
		int counter;
	};

	// ===============================================================================

	class Composite : public Behavior
	{
	public:
		void addChild(Behavior* child) { children.push_back(child); }
		void removeChild(Behavior*);
		void clearChildren();
	protected:
		typedef std::vector<Behavior*> Behaviors;
		Behaviors children;
	};

	class Sequence : public Composite
	{
	protected:
		virtual ~Sequence()
		{
		}

		virtual void onInitialize()
		{
			currentChild = children.begin();
		}

		virtual Status update()
		{
			// Keep going until a child behavior says it's running.
			for (;;)
			{
				Status s = (*currentChild)->tick();

				// If the child fails, or keeps running, do the same.
				if (s != SUCCESS)
				{
					return s;
				}

				// Hit the end of the array, job done!
				if (++currentChild == children.end())
				{
					return SUCCESS;
				}
			}
		}

		Behaviors::iterator currentChild;
	};

	// -----------------------------------------------------------------------

	template <class COMPOSITE> // using template because there are multiple composites?? (Comp. & Seq.)?? -- No, it is in order to handle any object of type 'Composite' 
	class MockComposite : public COMPOSITE
	{
	public:
		MockComposite(size_t size)
		{
			for (size_t i = 0; i<size; ++i)
			{
				COMPOSITE::children.push_back(new MockBehavior);
			}
		}

		~MockComposite()
		{
			for (size_t i_0; i<COMPOSITE::children.size(); ++i)
			{
				delete COMPOSITE::children[i];
			}
		}

		MockBehavior& operator[](size_t index)
		{
			ASSERT(INDEX < COMPOSITE::children.size());
			return *static_cast<Mockbehaior*>(COMPOSITE::children[index]);
		}
	};

	typedef MockComposite<Sequence> MockSequence;

	TEST(StarterKit1, SequenceTwoChildrenFails)
	{
		MockSequence seq(2);

		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(0, seq[0].terminateCalled);

		seq[0].returnStatus = FAILURE;
		CHECK_EQUAL(seq.tick(), FAILURE);
		CHECK_EQUAL(1, sequ[0].terminateCalled);
		CHECK_EQUAL(0, seq[1].initializeCalled);
	}

	TEST(StarterKit1, SequenceTwoChildrenContinues)
	{
		MockSequence seq(2);

		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(0, seq[0].terminateCalled);
		CHECK_EQUAL(0, seq[1].initializeCalled);

		seq[0].returnStatus = SUCCESS;
		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(1, sequ[0].terminateCalled);
		CHECK_EQUAL(0, seq[1].initializeCalled);
	}

	TEST(StarterKit1, SequenceOneChildPassThrough)
	{
		Status status[2] = { SUCESS, FAILURE };
		for (int i = 0; i<2; ++i)
		{
			MockSequence seq(1);

			CHECK_EQUAL(seq.tick(), RUNNING);
			CHECK_EQUAL(0, seq[0].terminateCalled);

			seq[0].returnStatus = status[i];
			CHECK_EQUAL(seq.tick(), status[i]);
			CHECK_EQUAL(1, sequ[0].terminateCalled);
		}
	}

	// ====================================================================

	class Selector : public Composite
	{
	protected:
		virtual ~Selector()
		{
		}

		virtual void onInitialize()
		{
			currentChild = children.begin();
		}

		virtual Status update()
		{
			// Keep going until a child behavior says it's running.
			for (;;)
			{
				Status s = (*currentChild)->tick();

				// If the child fails, or keeps running, do the same.
				if (s != FAILURE)
				{
					return s;
				}

				// Hit the end of the array, job done!
				if (++currentChild == children.end())
				{
					return FAILURE;
				}
			}
		}

		Behaviors::iterator currentChild;
	};

	// -----------------------------------------------------------------------

	typedef MockComposite<Selector> MockSelector;

	TEST(StarterKit1, SelectorTwoChildrenContinues)
	{
		MockSelector seq(2);

		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(0, seq[0].terminateCalled);

		seq[0].returnStatus = FAILURE;
		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(1, sequ[0].terminateCalled);
	}

	TEST(StarterKit1, SelectorTwoChildrenSucceeds)
	{
		MockSelector seq(2);

		CHECK_EQUAL(seq.tick(), RUNNING);
		CHECK_EQUAL(0, seq[0].terminateCalled);

		seq[0].returnStatus = SUCCESS;
		CHECK_EQUAL(seq.tick(), SUCCESS);
		CHECK_EQUAL(1, sequ[0].terminateCalled);
	}

	TEST(StarterKit1, SelectorOneChildPassThrough)
	{
		Status status[2] = { SUCCESS, FAILURE };
		for (int i = 0; i<2; ++i)
		{
			MockSelector seq(1);

			CHECK_EQUAL(seq.tick(), RUNNING);
			CHECK_EQUAL(0, seq[0].terminateCalled);

			seq[0].returnStatus = status[i];
			CHECK_EQUAL(seq.tick(), status[i]);
			CHECK_EQUAL(1, sequ[0].terminateCalled);
		}
	}

	class Parallel : public Composite
	{
	public:
		enum Policy
		{
			RequireOne,
			RequireAll,
		};

		Parallel(Policy forSuccess, Policy forFailure)
			: successPolicy(forSuccess)
			, failurePolicy(forFailure)
		{
		}

		virtual ~Parallel() {}

	protected:
		Policy successPolicy;
		Policy failurePolicy;

		virtual Status update()
		{
			size_t successCount = 0, failureCount = 0;

			for (Behaviors::iterator it = children.begin(); it != children.end(); ++it)
			{
				Behavior& b = **it;
				if (!b.isTerminated())
				{
					b.tick();
				}

				if (b.getStatus() == SUCCESS)
				{
					++successCount;
					if (successPolicy == RequireOne)
					{
						return SUCCESS;
					}
				}

				if (b.getStatus() == FAILURE)
				{
					++failureCount;
					if (failurePolicy == RequireOne)
					{
						return FAILURE;
					}
				}
			}

			if (failurePolicy == RequireAll && failureCount == children.size())
			{
				return FAILURE;
			}

			if (successPolicy == RequireAll && successCount == children.size())
			{
				return SUCCESS;
			}

			return RUNNING;
		}

		virtual void onTerminate(Status)
		{
			for (Behaviors::iterator it = children.begin(); it != children.end(); ++it)
			{
				Behavior& b = **it;
				if (b.isRunning())
				{
					b.abort();
				}
			}
		}
	};

	// ** following is pasted - - type out when time to learn more :) ** ***** vvvvvvvv

	TEST(StarterKit1, ParallelSucceedRequireAll)
	{
		Parallel parallel(Parallel::RequireAll, Parallel::RequireOne);
		MockBehavior children[2];
		parallel.addChild(&children[0]);
		parallel.addChild(&children[1]);
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[0].m_eReturnStatus = BH_SUCCESS;
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[1].m_eReturnStatus = BH_SUCCESS;
		CHECK_EQUAL(BH_SUCCESS, parallel.tick());
	}
	TEST(StarterKit1, ParallelSucceedRequireOne)
	{
		Parallel parallel(Parallel::RequireOne, Parallel::RequireAll);
		MockBehavior children[2];
		parallel.addChild(&children[0]);
		parallel.addChild(&children[1]);
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[0].m_eReturnStatus = BH_SUCCESS;
		CHECK_EQUAL(BH_SUCCESS, parallel.tick());
	}
	TEST(StarterKit1, ParallelFailureRequireAll)
	{
		Parallel parallel(Parallel::RequireOne, Parallel::RequireAll);
		MockBehavior children[2];
		parallel.addChild(&children[0]);
		parallel.addChild(&children[1]);
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[0].m_eReturnStatus = BH_FAILURE;
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[1].m_eReturnStatus = BH_FAILURE;
		CHECK_EQUAL(BH_FAILURE, parallel.tick());
	}
	TEST(StarterKit1, ParallelFailureRequireOne)
	{
		Parallel parallel(Parallel::RequireAll, Parallel::RequireOne);
		MockBehavior children[2];
		parallel.addChild(&children[0]);
		parallel.addChild(&children[1]);
		CHECK_EQUAL(BH_RUNNING, parallel.tick());
		children[0].m_eReturnStatus = BH_FAILURE;
		CHECK_EQUAL(BH_FAILURE, parallel.tick());
	}

	class Monitor : public Parallel
	{
	public:
		Monitor()
			: Parallel(Parallel::RequireOne, Parallel::RequireOne)
		{
		}
		void addCondition(Behavior* condition)
		{
			m_Children.insert(m_Children.begin(), condition);
		}
		void addAction(Behavior* action)
		{
			m_Children.push_back(action);
		}
	};
	// ============================================================================
	class ActiveSelector : public Selector
	{
	protected:
		virtual void onInitialize()
		{
			m_Current = m_Children.end();
		}
		virtual Status update()
		{
			Behaviors::iterator previous = m_Current;
			Selector::onInitialize();
			Status result = Selector::update();
			if (previous != m_Children.end() && m_Current != previous)
			{
				(*previous)->onTerminate(BH_ABORTED);
			}
			return result;
		}
	};
	typedef MockComposite<ActiveSelector> MockActiveSelector;
	TEST(StarterKit1, ActiveBinarySelector)
	{
		MockActiveSelector sel(2);
		sel[0].m_eReturnStatus = BH_FAILURE;
		sel[1].m_eReturnStatus = BH_RUNNING;
		CHECK_EQUAL(sel.tick(), BH_RUNNING);
		CHECK_EQUAL(1, sel[0].m_iInitializeCalled);
		CHECK_EQUAL(1, sel[0].m_iTerminateCalled);
		CHECK_EQUAL(1, sel[1].m_iInitializeCalled);
		CHECK_EQUAL(0, sel[1].m_iTerminateCalled);
		CHECK_EQUAL(sel.tick(), BH_RUNNING);
		CHECK_EQUAL(2, sel[0].m_iInitializeCalled);
		CHECK_EQUAL(2, sel[0].m_iTerminateCalled);
		CHECK_EQUAL(1, sel[1].m_iInitializeCalled);
		CHECK_EQUAL(0, sel[1].m_iTerminateCalled);
		sel[0].m_eReturnStatus = BH_RUNNING;
		CHECK_EQUAL(sel.tick(), BH_RUNNING);
		CHECK_EQUAL(3, sel[0].m_iInitializeCalled);
		CHECK_EQUAL(2, sel[0].m_iTerminateCalled);
		CHECK_EQUAL(1, sel[1].m_iInitializeCalled);
		CHECK_EQUAL(1, sel[1].m_iTerminateCalled);
		sel[0].m_eReturnStatus = BH_SUCCESS;
		CHECK_EQUAL(sel.tick(), BH_SUCCESS);
		CHECK_EQUAL(3, sel[0].m_iInitializeCalled);
		CHECK_EQUAL(3, sel[0].m_iTerminateCalled);
		CHECK_EQUAL(1, sel[1].m_iInitializeCalled);
		CHECK_EQUAL(1, sel[1].m_iTerminateCalled);
	}

} // namespace bt1

// **** pasted *****  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^