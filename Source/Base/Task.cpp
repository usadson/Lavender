/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 *
 * This mechanism was made because there is no std::future::then or
 * something alike, which is a shame. Hopefully P2300R5 will make
 * this obsolete in the future :)
 */

#define SEPARATE_THREAD

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <processthreadsapi.h>
#endif // _WIN32

#include "Task.hpp"

#include <optional>
#include <thread>

#include <cassert>

namespace base {

    std::shared_ptr<Task>
    Task::run(Task &&inTask, std::any parameter) noexcept {
        auto startTask = std::make_shared<Task>(std::move(inTask));

#ifdef SEPARATE_THREAD
        std::thread([](std::optional<std::shared_ptr<Task>> nextTask, std::any parameter) {
#ifdef _WIN32
            SetThreadDescription(GetCurrentThread(), L"base::Task::run-er");
#endif // _WIN32
#else // SEPARATE_THREAD
        std::optional nextTask{ startTask };
#endif // SEPARATE_THREAD
            while (nextTask.has_value() && *nextTask != nullptr) {
                auto &task = **nextTask;

                parameter = task.m_function(std::move(parameter));

                while (nextTask->use_count() > 1 && task.m_next == nullptr) {
                    std::this_thread::yield();
                }

                auto next = task.m_next;
                nextTask.reset();
                nextTask = next;
            }
#ifdef SEPARATE_THREAD
        }, startTask, std::move(parameter)).detach();
#endif

        return startTask;
    }

    std::shared_ptr<Task>
    Task::then(std::shared_ptr<Task> &&task) noexcept {
        assert(m_next == nullptr);
        m_next = task;
        return task;
    }

}
