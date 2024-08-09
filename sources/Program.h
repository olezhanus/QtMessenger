#pragma once

#ifdef _WIN64

#include <Windows.h>

#endif // _WIN64
#ifdef __linux__

#include <unistd.h>

#endif // __linux__

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include "nlohmann/json.hpp"
#include <cstdio>
#include <climits>
#include "Chat.h"
#include "User.h"
#include "Message.h"

namespace fs = std::filesystem;

#define USERS_FILE "users.json"
#define CHATS_FILE "chats.json"

#define QUIT_CMD "/q"
#define SHOW_USERS_CMD "/shu"
#define ADD_USERS_CMD "/addu"
#define LOGIN_MIN_LENGTH 1
#define PASSWORD_MIN_LENGTH 1
#define NAME_MIN_LENGTH 1

class Basic_Program // Базовый класс программы с консольным интерфейсом. от него можно наследовать другие реализации UI
{
public:
	Basic_Program() noexcept;
	Basic_Program(const Basic_Program &other) = delete;
	Basic_Program(Basic_Program &&other) = delete;
	Basic_Program &operator=(const Basic_Program &other) = delete;
	Basic_Program &operator=(Basic_Program &&other) = delete;
	virtual void run();
	static void sleep(time_t milliseconds);
	static void clear_screen();

	virtual ~Basic_Program();

protected:
	std::vector<std::shared_ptr<User>> _users;
	std::vector<std::shared_ptr<Chat>> _chats; // Список всех чатов
	std::weak_ptr<User> _logined_user;
	std::weak_ptr<Chat> _current_chat;
	virtual void log_in();
	virtual void sign_up();
	virtual void log_out() noexcept;
	virtual void show_chats();
	virtual void show_users();
	virtual void show_messages() noexcept;
	virtual void print_message(const std::shared_ptr<Message> &mes) noexcept;

	virtual void do_command(const std::string &command);
	virtual bool get_string(std::string &out, size_t min_length = 0, bool is_password = false, bool need_to_erase = false) noexcept;
	virtual bool get_number(size_t &out, size_t max_number);
};
