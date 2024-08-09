// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Chat.h"

size_t Chat::_Id_Counter{0};

Chat::Chat(std::string &&title) noexcept : _id(++_Id_Counter),
										   _title(title)
{
}

Chat::Chat(Chat &&other) noexcept : _id(other._id),
									_users(std::move(other._users)),
									_messages(std::move(other._messages)),
									_title(std::move(other._title))
{
	other._id = 0;
}

Chat &Chat::operator=(Chat &&other) noexcept
{
	if (&other == this)
	{
		return *this;
	}
	_users = std::move(other._users);
	_messages = std::move(other._messages);
	_title = std::move(other._title);
	return *this;
}

auto Chat::id() const noexcept -> size_t
{
	return _id;
}

auto Chat::users() noexcept -> std::vector<std::weak_ptr<User>> &
{
	return _users;
}

auto Chat::messages() noexcept -> std::vector<std::shared_ptr<Message>> &
{
	return _messages;
}

auto Chat::title() const noexcept -> const std::string &
{
	return _title;
}

void Chat::add_user(std::shared_ptr<User> user)
{
	if (user->id() != 0)
	{
		_users.emplace_back(std::move(user));
		_users.back().lock()->chats().emplace_back(shared_from_this());
	}
}

void Chat::new_message(std::shared_ptr<Message> message)
{
	_messages.emplace_back(std::move(message));
}

Chat Chat::from_json(const nlohmann::json &j)
{
	if (j.find("_id") != j.end() &&
		j.find("_title") != j.end() &&
		j.find("_users_id") != j.end() &&
		j.find("_messages") != j.end())
	{
		return Chat(j);
	}
	else
	{
		throw std::exception();
	}
}

nlohmann::json Chat::to_json(const Chat &chat)
{
	nlohmann::json j;
	j["_id"] = chat._id;
	j["_title"] = chat._title;
	j["_users_id"] = nlohmann::json::array();
	for (auto &i : chat._users)
	{
		j["_users_id"].push_back(i.lock()->id());
	}
	j["_messages"] = nlohmann::json::array();
	for (auto &i : chat._messages)
	{
		j["_messages"].emplace_back(Message::to_json(*i));
	}
	return j;
}

Chat::Chat(const nlohmann::json &j) noexcept : _id(j["_id"]),
											   _title(j["_title"])
{
	for (auto jmes : j["_messages"])
	{
		try
		{
			_messages.emplace_back(std::make_shared<Message>(Message::from_json(jmes)));
		}
		catch (const std::exception &e)
		{
			std::cout << e.what();
		}
	}
}
