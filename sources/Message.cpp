// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Message.h"

Message::Message(std::string &&message, std::weak_ptr<User> from) noexcept :
	_date(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())),
	_message(message),
	_from(std::move(from))
{
}

Message::Message(Message &&other) noexcept :
	_date(other._date),
	_message(std::move(other._message)),
	_from(std::move(other._from))
{
	other._date = 0;
}

Message &Message::operator=(Message &&other) noexcept
{
	if (&other == this)
	{
		return *this;
	}
	_date = std::move(other._date);
	_message = std::move(other._message);
	_date = other._date;
	other._date = 0;
	return *this;
}

auto Message::date() const noexcept -> time_t
{
	return _date;
}

auto Message::message()const noexcept -> const std::string &
{
	return _message;
}

auto Message::from() const noexcept -> const std::weak_ptr<User>
{
	return _from;
}

Message Message::from_json(const nlohmann::json &j)
{
	if (j.find("_date") != j.end() &&
		j.find("_message") != j.end() &&
		j.find("_from_id") != j.end())
	{
		return Message(j);
	}
	else
	{
		throw std::exception();
	}
}

nlohmann::json Message::to_json(const Message &mes)
{
	nlohmann::json j;
	j["_date"] = mes._date;
	j["_message"] = mes._message;
	j["_from_id"] = mes._from.lock()->id();
	return j;
}

Message::Message(const nlohmann::json &j) noexcept :
	_date(j["_date"]),
	_message(j["_message"])
{
}

