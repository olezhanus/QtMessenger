#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "nlohmann/json.hpp"
#include "User.h"
#include "Message.h"
class User;
class Message;

class Chat : public std::enable_shared_from_this<Chat>
{
public:
	friend class Basic_Program;

	Chat(std::string &&title) noexcept;
	Chat(const Chat &other) = delete;
	Chat(Chat &&other) noexcept;
	Chat &operator=(const Chat &other) = delete;
	Chat &operator=(Chat &&other) noexcept;
	auto id() const noexcept -> size_t;
	auto users() noexcept -> std::vector<std::weak_ptr<User>> &;
	auto messages() noexcept -> std::vector<std::shared_ptr<Message>> &;
	auto title() const noexcept -> const std::string &;
	void add_user(std::shared_ptr<User> user);
	void new_message(std::shared_ptr<Message> message);
	~Chat() noexcept = default;

	static Chat from_json(const nlohmann::json &j);
	static nlohmann::json to_json(const Chat &chat);

private:
	Chat(const nlohmann::json &j) noexcept;
	static size_t _Id_Counter;
	size_t _id;
	std::string _title;
	std::vector<std::weak_ptr<User>> _users;
	std::vector<std::shared_ptr<Message>> _messages;
};
