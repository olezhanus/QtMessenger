#pragma once
#include <chrono>
#include <memory>
#include <string>
#include "nlohmann/json.hpp"
#include "User.h"
class User;

class Message : public std::enable_shared_from_this<Message>
{
public:
	friend class Basic_Program;

	Message(std::string &&message, std::weak_ptr<User> from) noexcept;
	Message(const Message &other) = delete;
	Message(Message &&other) noexcept;
	Message &operator=(const Message &other) = delete;
	Message &operator=(Message &&other) noexcept;
	auto date() const noexcept -> time_t;
	auto message() const noexcept -> const std::string &;
	auto from() const noexcept -> const std::weak_ptr<User>;
	~Message() noexcept = default;

	static Message from_json(const nlohmann::json &j);
	static nlohmann::json to_json(const Message &mes);
private:
	Message(const nlohmann::json &j) noexcept;
	time_t _date;
	std::string _message;
	std::weak_ptr<User> _from;
};
