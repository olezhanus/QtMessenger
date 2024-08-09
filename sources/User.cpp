// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "User.h"

size_t User::_Id_Counter{0};

User::User(std::string &&username, std::string &&login, size_t password) noexcept : _id(++_Id_Counter),
																					_username(username),
																					_login(login),
																					_password_hash(password) // id будет начинаться с 1. 0 - для пустой переменной User.
{
}

User::User(User &&other) noexcept : _id(other._id),
									_username(std::move(other._username)),
									_login(std::move(other._login)),
									_password_hash(other._password_hash),
									_chats(std::move(other._chats))
{
	other._password_hash = 0;
	other._id = 0;
}

User &User::operator=(User &&other) noexcept
{
	if (&other == this)
	{
		return *this;
	}
	_id = other._id;
	_username = std::move(other._username);
	_login = std::move(other._login);
	_password_hash = other._password_hash;
	_chats = std::move(other._chats);
	other._id = 0;
	other._password_hash = 0;
	return *this;
}

auto User::id() const noexcept -> size_t
{
	return _id;
}

auto User::username() const noexcept -> const std::string &
{
	return _username;
}

auto User::login() const noexcept -> const std::string &
{
	return _login;
}

auto User::password_hash() const noexcept -> size_t
{
	return _password_hash;
}

auto User::chats() noexcept -> std::vector<std::weak_ptr<Chat>> &
{
	return _chats;
}

void User::set_username(const std::string &new_username) noexcept
{
	if (_username == new_username)
	{
		return;
	}
	_username = new_username;
}

User User::from_json(const nlohmann::json &j)
{
	if (
		j.find("_id") != j.end() &&
		j.find("_username") != j.end() &&
		j.find("_login") != j.end() &&
		j.find("_password_hash") != j.end() &&
		j.find("_chats_id") != j.end())
	{
		return User(j);
	}
	else
	{
		throw std::exception();
	}
}

nlohmann::json User::to_json(const User &user)
{
	nlohmann::json j;
	j["_id"] = user._id;
	j["_username"] = user._username;
	j["_login"] = user._login;
	j["_password_hash"] = user._password_hash;
	j["_chats_id"] = nlohmann::json::array();
	for (auto &i : user._chats)
	{
		j["_chats_id"].push_back(i.lock()->id());
	}
	return j;
}

User::User(const nlohmann::json &j) noexcept : _id(j["_id"]),
											   _username(j["_username"]),
											   _login(j["_login"]),
											   _password_hash(j["_password_hash"])
{
}
