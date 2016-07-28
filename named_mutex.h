#ifndef NAMED_MUTEX_H
#define NAMED_MUTEX_H

#include <mutex>
#include "common.h"

class NamedMutexArray
{
public:
	// sOk if created
	// sFalse if existed
	result GetMutex(const std::string& promo_code, std::shared_ptr<std::mutex>& mutex)
	{
		std::lock_guard<std::mutex> lock(m_accessMutex);

		auto m = m_mutexes.find(promo_code);
		if (m_mutexes.end() == m)
		{
			mutex = m_mutexes.insert(std::make_pair(promo_code, std::make_shared<std::mutex>())).first->second;
			return result_code::sOk;
		}
		else
		{
			if (!m->second)
				m->second = std::make_shared<std::mutex>();
			
			mutex = m->second;
			return result_code::sFalse;
		}
	}

	result ReleaseMutex(const std::string& key)
	{
		std::lock_guard<std::mutex> lock(m_accessMutex);
		m_mutexes.erase(key);
	}

private:
	std::mutex m_accessMutex;

	// Create for each object, then release shared_ptr and destroy mutex
	std::map<std::string, std::shared_ptr<std::mutex>> m_mutexes; 
};

class AutoMutex
{
public:
	AutoMutex(NamedMutexArray& array, const std::string& key)
	{
		array.GetMutex(key, m_mutex);
		m_mutex->lock();
	}

	~AutoMutex()
	{
		m_mutex->unlock();
		array.ReleaseMutex(key);
	}

private:
	std::shared_ptr<std::mutex> m_mutex;
};


#endif // NAMED_MUTEX_H