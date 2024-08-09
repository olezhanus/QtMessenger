#pragma once
#include <string>
#include <vector>
#include <exception>
#include "nlohmann/json.hpp"
#include "Chat.h"
class Chat;

class User : public std::enable_shared_from_this<User>
{
public:
	friend class Basic_Program;

	User(std::string &&username, std::string &&login, size_t password) noexcept;
	User(const User &other) = delete;
	User(User &&other) noexcept;
	User &operator=(const User &other) = delete;
	User &operator=(User &&other) noexcept;
	auto id() const noexcept -> size_t;
	auto username() const noexcept -> const std::string &;
	auto login() const noexcept -> const std::string &;
	auto password_hash() const noexcept -> size_t;
	auto chats() noexcept -> std::vector<std::weak_ptr<Chat>> &;
	void set_username(const std::string &new_username) noexcept;
	~User() noexcept = default;

	static User from_json(const nlohmann::json &j);
	static nlohmann::json to_json(const User &user);

private:
	User(const nlohmann::json &j) noexcept;
	static size_t _Id_Counter;
	size_t _id;
	std::string _username;
	std::string _login;
	size_t _password_hash;
	std::vector<std::weak_ptr<Chat>> _chats; // Список чатов, где есть данный пользователь
};
