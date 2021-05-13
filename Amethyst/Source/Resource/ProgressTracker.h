#pragma once
#include <string>
#include <unordered_map>

namespace Amethyst
{
	enum class ProgressType
	{
		ModelImporter,
		World,
		ResourceCache
	};

	struct Progress //One progress "instance" for each ProgressType above.
	{
		Progress()
		{
			Clear();
		}

		void Clear()
		{
			m_ProgressStatus.clear();
			m_JobsDone = 0;
			m_JobCount = 0;
			m_IsLoading = false;
		}

		std::string m_ProgressStatus;
		int m_JobsDone;
		int m_JobCount;
		bool m_IsLoading;
	};

	class ProgressTracker
	{
	public:
		static ProgressTracker& RetrieveInstance()
		{
			static ProgressTracker instance;
			return instance;
		}

		ProgressTracker() = default;
		
		void Reset(ProgressType progressType)
		{
			m_Reports[progressType].Clear();
		}

		const std::string& RetrieveStatus(ProgressType progressType) { return m_Reports[progressType].m_ProgressStatus; }
		void SetStatus(ProgressType progressType, const std::string& progressStatus) { m_Reports[progressType].m_ProgressStatus = progressStatus;  }
		
		void SetJobCount(ProgressType progressType, int jobCount) { m_Reports[progressType].m_JobCount = jobCount; } //Total jobs (for example 100 nodes to load) for this task.
		void IncrementJobsDone(ProgressType progressType) { m_Reports[progressType].m_JobsDone++; }
		void SetJobsDone(ProgressType progressType, int jobsDone) { m_Reports[progressType].m_JobsDone = jobsDone; }
		
		float RetrievePercentage(ProgressType progressType) { return static_cast<float>(m_Reports[progressType].m_JobsDone) / static_cast<float>(m_Reports[progressType].m_JobCount); }	
		bool RetrieveLoadStatus(ProgressType progressType) { return m_Reports[progressType].m_IsLoading; }
		void SetLoadStatus(ProgressType progressType, bool isLoading) { m_Reports[progressType].m_IsLoading = isLoading; }

	private:
		std::unordered_map<ProgressType, Progress> m_Reports;
	};
}