#include "task.h"

BEGIN_NAMESPACE_LIB

void TaskManager::start()
{
    if (m_running) return;

    m_running = true;
    m_thread.start(this, &TaskManager::threadEntry);
}

void TaskManager::stop()
{
    if (m_running)
    {
        m_running = false;

        m_thread.join();
        m_tasks.clear();
    }
}

void TaskManager::queueTask(Task* task)
{
    m_tasks.push(task);
}

Task* TaskManager::nextTask(int timeout)
{
    try { return m_tasks.wait(timeout); }
    catch (...) { return 0; }
}

bool TaskManager::running()
{
    return m_running;
}

void TaskManager::threadEntry()
{
    while (m_running)
    {
        Task* task = nextTask(1000);

        if (task)
        {
            try
            {
                task->run();
            }
            catch (...)
            {
                logmsg("Exception was thrown in task manager");
            }

            delete task;
        }
    }
}

END_NAMESPACE_LIB