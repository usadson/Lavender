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

#pragma once

#include <any>
#include <functional>

namespace base {
    
    struct Task {
        using FunctionType = std::function<std::any (std::any&&)>;
        
        [[nodiscard]] explicit
        Task(FunctionType &&function)
                : m_function(std::move(function)) {
        }

        template<typename Result>
        [[nodiscard]] static Task
        create(auto function) {
            return Task([func = std::function<Result ()>(function)](std::any) -> std::any {
                return func();
            });
        }

        [[nodiscard]]
        Task(Task &&) noexcept = default;

        Task &operator=(const Task &) noexcept = delete;
        Task &operator=(Task &&) noexcept = default;

        static std::shared_ptr<Task>
        run(Task &&, std::any) noexcept;

        /* discardable */ std::shared_ptr<Task>
        then(std::shared_ptr<Task> &&) noexcept;

        /* discardable */ std::shared_ptr<Task>
        then(Task &&task) noexcept {
            return then(std::make_shared<Task>(std::move(task)));
        }

        /* discardable */ std::shared_ptr<Task>
        then(FunctionType &&function) noexcept {
            return then(std::make_shared<Task>( std::move(function)));
        }

        template<typename Result>
        static Task
        then(auto function) {
            return Task([func = std::function<Result()>(function)](std::any) -> std::any {
                return func();
            });
        }

        template<typename Result, typename Parameter>
        static Task
        then(auto function) {
            return Task([func = std::function<Result(Parameter)>(function)](std::any &&parameter) -> std::any {
                if constexpr (std::is_same_v<Result, void>) {
                    func(std::any_cast<Parameter&&>(std::move(parameter)));
                    return {};
                } else {
                    return func(std::any_cast<Parameter&&>(std::move(parameter)));
                }
            });
        }

    private:
        FunctionType m_function;
        std::shared_ptr<Task> m_next{};
    };
    
} // namespace base

