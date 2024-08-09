// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Program.h"

Basic_Program::Basic_Program() noexcept // здесь открываются и считываются файлы конфигурации
{
	nlohmann::json chats_json, users_json;

	std::fstream USERS(USERS_FILE, std::ios::in);

	if (USERS.is_open())
	{
		try
		{
			USERS >> users_json;
			User::_Id_Counter = users_json["_Id_Counter"];
			for (auto &j : users_json["_users"])
			{
				_users.emplace_back(std::make_shared<User>(User::from_json(j)));
			}
		}
		catch (const std::exception &e)
		{
			std::cout << e.what();
			sleep(1000);
		}
		USERS.close();
	}

	std::fstream CHATS(CHATS_FILE, std::ios::in);

	if (CHATS.is_open())
	{
		try
		{
			CHATS >> chats_json;
			Chat::_Id_Counter = chats_json["_Id_Counter"];
			for (auto &j : chats_json["_chats"])
			{
				auto chat = std::make_shared<Chat>(Chat::from_json(j));
				_chats.emplace_back(chat);
				for (auto user_id : j["_users_id"])
				{
					auto user_to_add = std::find_if(
						_users.begin(),
						_users.end(),
						[=](std::shared_ptr<User> el) -> bool
						{
							return el->id() == user_id;
						});
					chat->add_user(*user_to_add);
				}
				for (size_t index = 0; index < _chats.back()->messages().size(); ++index)
				{
					auto user_from = std::find_if(
						_users.begin(),
						_users.end(),
						[=](std::shared_ptr<User> el) -> bool
						{
							return el->id() == j["_messages"][index]["_from_id"];
						});
					chat->messages()[index]->_from = *user_from;
				}
			}
		}
		catch (const std::exception &e)
		{
			std::cout << e.what();
			sleep(1000);
		}
		CHATS.close();
	}
}

Basic_Program::~Basic_Program() // здесь создаются файлы конфигурации
{
	nlohmann::json chats_json, users_json;
	chats_json["_Id_Counter"] = Chat::_Id_Counter;
	users_json["_Id_Counter"] = User::_Id_Counter;
	chats_json["_chats"] = nlohmann::json::array();
	for (auto &chat : _chats)
	{
		chats_json["_chats"].emplace_back(Chat::to_json(*chat));
	}
	users_json["_users"] = nlohmann::json::array();
	for (auto &user : _users)
	{
		users_json["_users"].emplace_back(User::to_json(*user));
	}

	std::fstream USERS(USERS_FILE, std::ios::out | std::ios::trunc);

	if (USERS.is_open())
	{
		try
		{
			USERS << std::setw(4) << users_json;
		}
		catch (const std::exception &e)
		{
			std::cout << e.what();
			sleep(1000);
		}
		USERS.close();
	}

	std::fstream CHATS(CHATS_FILE, std::ios::out | std::ios::trunc);

	if (CHATS.is_open())
	{
		try
		{
			CHATS << std::setw(4) << chats_json;
		}
		catch (const std::exception &e)
		{
			std::cout << e.what();
		}
		CHATS.close();
	}
	fs::permissions(USERS_FILE, fs::perms::owner_all); // Забираем права на файлы у всех, кроме владельца
	fs::permissions(CHATS_FILE, fs::perms::owner_all);
}

void Basic_Program::run()
{
#ifdef _WIN64

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

#endif // _WIN64

	while (true)
	{
		clear_screen();
		if (_logined_user.expired())
		{
			std::cout << QUIT_CMD " - выход.\tl - войти.\ts - создать аккаунт." << std::endl;
			std::string command;
			bool is_break;

			while ((is_break = get_string(command)) &&
				command != "l" && command != "s")
				; // будет ждать ввода правильного символа

			if (!is_break)
			{
				break;
			}
			if (command == "l")
			{
				log_in();
			}
			if (command == "s")
			{
				sign_up();
			}
		}
		else
		{
			if (_current_chat.expired())
			{
				std::cout << QUIT_CMD " - выход.\ts - выбрать чат.\td - удалить чат\t\tn - новый чат." << std::endl;
				std::string command;
				bool is_continue;

				while ((is_continue = get_string(command)) &&
					command != "s" && command != "d" && command != "n")
					;

				if (!is_continue)
				{
					log_out();
					continue;
				}
				if (command == "s" || command == "d")
				{
					show_chats();
					std::string number_string;
					size_t number = 0;
					auto &user_chats = _logined_user.lock()->chats();
					auto size = user_chats.size();

					std::cout << "Введите номер: ";
					if (!get_number(number, size))
					{
						continue;
					}

					auto index = size - number - 1;

					if (command == "s")
					{
						_current_chat = user_chats[index];
					}
					if (command == "d")
					{
						// выходим из чата. Если чат пуст, удаляем его
						auto user_chat = user_chats[index].lock();
						auto &user_chat_users = user_chat->users();
						auto iter = std::find_if(
							user_chat_users.begin(),
							user_chat_users.end(),
							[=](std::weak_ptr<User> el) -> bool
							{
								return el.lock() == _logined_user.lock();
							});

						user_chat->new_message(std::make_shared<Message>(std::move("*Покинул чат*"), _logined_user));
						user_chat_users.erase(iter);
						if (user_chat_users.empty())
						{
							auto chat_to_erase = std::find(
								_chats.begin(),
								_chats.end(),
								user_chat);
							_chats.erase(chat_to_erase);
						}
						user_chats.erase(user_chats.begin() + index);
					}
				}
				if (command == "n")
				{
					// Создаём новый чат с выбранным пользователем и добавляем его во все соответствующие вектора
					clear_screen();
					show_users();
					size_t number = 0;
					bool is_continue;

					std::cout << "Введите номер: ";
					while (is_continue = get_number(number, _users.size()))
					{
						if (_users[number] == _logined_user.lock())
						{
							std::cout << std::endl << "В беседе не может быть два одинаковых пользователя\n"
								"Введите корректный номер: ";
							continue;
						}
						break;
					}
					if (!is_continue)
					{
						continue;
					}

					std::cout << std::endl << "Введите название: ";
					std::string title;
					if (!get_string(title, NAME_MIN_LENGTH))
					{
						continue;
					}
					_chats.emplace_back(std::make_shared<Chat>(std::move(title)));
					_chats.back()->add_user(_logined_user.lock());
					_chats.back()->add_user(_users[number]);
					_current_chat = _chats.back();
				}
			}
			else
			{
				std::cout << "Введите сообщение или одну из следующих команд:" << std::endl << ADD_USERS_CMD " - добавить участника\t" SHOW_USERS_CMD " - показать участников беседы" << std::endl << std::endl;
				show_messages();
				std::string message_string;
				while (true)
				{
					;
					if (!get_string(message_string, 0, false, true))
					{
						_current_chat.reset();
						break;
					}
					if (!message_string.empty())
					{
						if (message_string[0] == '/')
						{
							do_command(message_string);
							break;
						}
						else
						{
							_current_chat.lock()->new_message(std::make_shared<Message>(std::move(message_string), _logined_user));

							// Перемещаем _current_chat с новым сообщением в конец массива
							auto &user_chat = _logined_user.lock()->chats();
							if (_current_chat.lock() != user_chat.back().lock())
							{
								auto iter = std::find_if(
									user_chat.begin(),
									user_chat.end(),
									[=](std::weak_ptr<Chat> el) -> bool
									{
										return el.lock() == _current_chat.lock();
									});
								auto item = *iter;
								user_chat.erase(iter);
								user_chat.emplace_back(std::move(item));
							}

							print_message(_current_chat.lock()->messages().back());
						}
					}
				}
			}
		}
	}
}

void Basic_Program::log_in()
{
	std::string login, password;
	while (true)
	{
		clear_screen();
		std::cout << "Логин: ";
		if (!get_string(login))
		{
			break;
		}
		auto iter = _users.empty() ? _users.end() : std::find_if(_users.begin(), _users.end(), [=](std::shared_ptr<User> el) -> bool
			{
				return el->login() == login;
			});

		if (iter == _users.end())
		{
			std::cout << std::endl << "Пользователя с таким логином не существует";
			sleep(2000);
			continue;
		}
		std::cout << std::endl << "Пароль: ";
		if (!get_string(password, PASSWORD_MIN_LENGTH, true))
		{
			continue;
		}
		if (std::hash<std::string>{}(password) != (*iter)->password_hash())
		{
			std::cout << std::endl << "Неправильный пароль";
			sleep(2000);
			continue;
		}
		_logined_user = *iter;
		break;
	}
}

void Basic_Program::sign_up()
{
	std::string login, password, username;
	while (true)
	{
		clear_screen();
		std::cout << "Логин: ";
		if (!get_string(login, LOGIN_MIN_LENGTH))
		{
			break;
		}
		auto iter = _users.empty() ? _users.end() : std::find_if(_users.begin(), _users.end(), [=](std::shared_ptr<User> el) -> bool
			{
				return el->login() == login;
			});
		if (iter != _users.end())
		{
			std::cout << std::endl << "Пользователь с таким логином уже существует";
			sleep(2000);
			continue;
		}
		std::cout << std::endl << "Пароль: ";
		if (!get_string(password, PASSWORD_MIN_LENGTH, true))
		{
			continue;
		}
		std::cout << std::endl << "Введите имя пользователя: ";
		if (!get_string(username, NAME_MIN_LENGTH))
		{
			continue;
		}
		_users.emplace_back(std::make_shared<User>(std::move(username), std::move(login), std::hash<std::string>{}(password)));
		_logined_user = *(_users.end() - 1);
		break;
	}
}

void Basic_Program::log_out() noexcept
{
	_logined_user.reset();
}

void Basic_Program::show_chats()
{
	auto size = _logined_user.lock()->chats().size();
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << i << '\t' << _logined_user.lock()->chats()[size - i - 1].lock()->title() << std::endl << std::endl;
	}
}

void Basic_Program::show_users()
{
	for (size_t i = 0; i < _users.size(); ++i)
	{
		std::cout << i << '\t' << _users[i]->username() << std::endl << std::endl;
	}
}

void Basic_Program::show_messages() noexcept
{
	for (auto &mes : _current_chat.lock()->messages())
	{
		print_message(mes);
	}
}

void Basic_Program::print_message(const std::shared_ptr<Message> &mes) noexcept
{
#ifdef _WIN64
	time_t t = mes->date();
	tm tm;
	localtime_s(&tm, &t);
	auto date = std::put_time(&tm, "%Y %T");
#endif // _WIN64

#ifdef __linux__
	time_t t = mes->date();
	auto date = std::put_time(std::localtime(&t), "%Y %T");
#endif // __linux__

	std::cout << (mes->from().lock() == _logined_user.lock() ? "Вы" : mes->from().lock()->username()) << ':' << std::endl
		<< mes->message() << std::endl
		<< date << std::endl << std::endl << std::endl;
}

void Basic_Program::do_command(const std::string &command)
{
	if (command == ADD_USERS_CMD)
	{
		clear_screen();
		show_users();
		size_t number = 0;
		bool is_return;

		auto &chat_users = _current_chat.lock()->users();
		std::cout << "Введите номер: ";
		while (is_return = get_number(number, _users.size()))
		{
			if (std::find_if(
				chat_users.begin(),
				chat_users.end(),
				[=](std::weak_ptr<User> el) -> bool
				{
					return el.lock() == _users[number];
				}) != chat_users.end())
			{
				std::cout << std::endl
					<< "В беседе не может быть два одинаковых пользователя" << std::endl
					<< "Введите корректный номер: ";
				continue;
			}
			break;
		}
		if (!is_return)
		{
			return;
		}

		_current_chat.lock()->add_user(_users[number]);
	}
	else if (command == SHOW_USERS_CMD)
	{
		clear_screen();
		for (auto &user : _current_chat.lock()->users())
		{
			std::cout << user.lock()->username() << "\n\n";
		}
		std::cin.ignore(LLONG_MAX, '\n');
	}
	else
	{
		std::cout << std::endl << "Неизвестная команда" << std::endl;
	}
}

bool Basic_Program::get_string(std::string &out, size_t min_length, bool is_password, bool need_to_erase) noexcept
{
	std::string str;
	while (true)
	{
		std::getline(std::cin, str);
		if (str == QUIT_CMD)
		{

			return false;
		}
		if (str.length() >= min_length)
		{
			if (need_to_erase)
			{
				for (auto &c : str)
				{
					std::cout << "\b \b";
				}
			}
			out = std::move(str);

			return true;
		}
		else
		{
			std::cout << std::endl << "Введите минимум " << min_length << " символов" << std::endl;
			str.clear();
		}
	}
}

bool Basic_Program::get_number(size_t &out, size_t max_number)
{
	std::string number_string;
	size_t number = 0;
	bool is_continue;
	while (is_continue = get_string(number_string))
	{
#ifdef _WIN64
		if (!sscanf_s(number_string.c_str(), "%zu", &number) || number >= max_number)
#endif // _WIN64

#ifdef __linux__
		if (!sscanf(number_string.c_str(), "%zu", &number) || number >= max_number)
#endif // __linux__
		{
			std::cout << std::endl << "Введите корректный номер: ";
			continue;
		}
		out = number;
		break;
	}

	return is_continue;
}

void Basic_Program::sleep(time_t milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Basic_Program::clear_screen()
{
#ifdef _WIN64
	system("cls");
#endif // _WIN64

#ifdef __linux__
	system("clear");
#endif // __linux__
}
