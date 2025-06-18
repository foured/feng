#pragma once

#include <functional>
#include <optional>

namespace feng {

    template<typename Ret, typename... Args>
    class func_link {
    public:
        class subscription {
        public:
            subscription() {}

            ~subscription() {
                unsubscribe();
            }

            subscription(const subscription&) = delete;
            subscription& operator=(const subscription&) = delete;

            subscription(subscription&& other) noexcept
                : _id(other._id), _func_link(std::move(other._func_link)) {
                other._id = 0;
                other._func_link = nullptr;
            }

            subscription& operator=(subscription&& other) noexcept {
                if (this != &other) {
                    unsubscribe();
                    _id = other._id;
                    _func_link = std::move(other._func_link);
                    other._id = 0;
                    other._func_link = nullptr;
                }
                return *this;
            }

        private:
            friend class func_link;

            void unsubscribe() {
                if (_func_link != nullptr && _id != 0) {
                    _func_link->unsubscribe(_id);
                    _func_link = nullptr;
                    _id = 0;
                }
            }

            subscription(uint32_t id, func_link* e)
                : _id(id), _func_link(e) {
            }

            uint32_t _id = 0;
            func_link* _func_link = nullptr;
        };

        using func_type = std::function<Ret(Args...)>;

        [[nodiscard]] subscription subscribe(func_type func) {
            _func = std::move(func);
            _current_id++;
            return subscription(_current_id, this);
        }

        void unsubscribe(uint32_t id) {
            if (id == _current_id) {
                _func = nullptr;
                _current_id = 0;
            }
        }

        template<typename R = Ret>
        typename std::enable_if<std::is_void<R>::value, void>::type
        invoke(Args... args) {
            if (_func) {
                _func(std::forward<Args>(args)...);
            }
        }

        template<typename R = Ret>
        typename std::enable_if<!std::is_void<R>::value, std::optional<R>>::type
        invoke(Args... args) {
            if (_func) {
                return _func(std::forward<Args>(args)...);
            }
            return std::nullopt;
        }

        bool empty() const {
            return !_func;
        }

    private:
        func_type _func;
        uint32_t _current_id = 0;
    };

}