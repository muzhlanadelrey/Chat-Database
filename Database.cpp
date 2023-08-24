﻿#include <iostream>
#include <sstream>
#include <mysql.h>
#include "Database.h"
#include <filesystem>
#include <string>

//создание БД если нужно
void DataBase::createDataBase()
{
    if (mysql_select_db(&mysql, "chat_database") == 0) {
        std::cout << "База данных готова к работе" << std::endl;
        return;
    }

    if (mysql_query(&mysql, "CREATE DATABASE chat_database") == 0) {
        std::cout << "База данных отсутствует. Создана новая база данных" << std::endl;
    }
    else {
        std::cout << "ОШИБКА: Не удалось создать базу данных" << mysql_error(&mysql) << std::endl;
    }
}
    //создание таблиц
void DataBase::createTables()
{
    // Проверка наличия таблиц users и messages
    const char* checkTablesQuery = "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES "
        "WHERE TABLE_SCHEMA = 'chat_database' "
        "AND TABLE_NAME IN ('users', 'messages')";

    if (mysql_query(&mysql, checkTablesQuery) != 0) {
        std::cout << "Ошибка: не удалось проверить таблицы" << mysql_error(&mysql) << std::endl;
        return;
    }

    res = mysql_store_result(&mysql);

    if (res == nullptr) {
        std::cout << "Ошибка: не удалось получить результат запроса" << mysql_error(&mysql) << std::endl;
        return;
    }

    int numTables = mysql_num_rows(res);

    mysql_free_result(res);

    if (numTables == 2) {
        std::cout << "Таблицы проверены" << std::endl;
        return;
    }
    else
    {
        // таблица users
        const char* createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "login VARCHAR(255) NOT NULL UNIQUE,"
            "password VARCHAR(255) NOT NULL,"
            "name VARCHAR(255) NOT NULL UNIQUE,"
            "online BOOLEAN DEFAULT FALSE,"
            "isbanned BOOLEAN DEFAULT FALSE"
            ")";
        //таблица messages
        const char* createMessagesTable = "CREATE TABLE IF NOT EXISTS messages ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "from_id VARCHAR(255) NOT NULL,"
            "to_id VARCHAR(255) NOT NULL,"
            "to_all BOOLEAN DEFAULT FALSE,"
            "text TEXT NOT NULL,"
            "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ")";

        // проверка, что таблицы успешно созданы
        if (mysql_query(&mysql, createUsersTable) == 0 &&
            mysql_query(&mysql, createMessagesTable) == 0) {
            std::cout << "Таблицы успешно созданы" << std::endl;
        }
        else {
            std::cout << "Ошибка: не удалось создать таблицы" << mysql_error(&mysql) << std::endl;
        }
    }
}
//создание пользователя admin по умолчанию
void DataBase::createAdmin()
{
    const char* selectAdmin = "SELECT * FROM users WHERE login='admin'";

    if (mysql_query(&mysql, selectAdmin) != 0) {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_close(&mysql);
        return;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result != NULL) {
        if (mysql_num_rows(result) > 0) {
            std::cout << "Администратор готов" << std::endl;
        }
        else {
            const char* insertAdmin = "INSERT INTO users (login, password, name) VALUES ('admin', 'admin', 'Администратор')";

            if (mysql_query(&mysql, insertAdmin) == 0) {
                std::cout << "Администратор создан" << std::endl;
            }
            else {
                std::cout << "Ошибка при выполнении запроса INSERT: " << mysql_error(&mysql) << std::endl;
            }
        }

        mysql_free_result(result);
    }
    else {
        std::cout << "Ошибка при получении результата запроса SELECT: " << mysql_error(&mysql) << std::endl;
    }
}
//соединение с БД
void DataBase::dataBaseConnect()
{
    mysql_init(&mysql);
    if (&mysql == nullptr)
    {
        std::cout << "Ошибка: не удалось создать MySQL-дескриптор" << std::endl;
    }

    // Подключаемся к серверу
    if (!mysql_real_connect(&mysql, "localhost", "root", "root", "chat_database", NULL, NULL, 0))
    {
        std::cout << "Ошибка: не удалось подключиться к базе данных " << mysql_error(&mysql) << std::endl;
    }
    else
    {
        std::cout << "Соединение с базой данных установлено" << std::endl;
    }

    mysql_set_character_set(&mysql, "cp1251");
    //Смотрим изменилась ли кодировка на нужную, по умолчанию идёт latin1
    std::cout << "Установка кодировки: " << mysql_character_set_name(&mysql) << std::endl;

    createDataBase(); //проверяем базу данных на существование, если нет - создаем новую
    createTables(); //проверка таблиц и создание новых, если необходимо
    createAdmin();////создание пользователя с правами администратора, если такого нет

}
//отключение от БД
void DataBase::dataBaseDisconnect()
{
    mysql_close(&mysql);
    std::cout << "База данных отключена..." << std::endl;
}
//проверка логина пр входе на сервер
bool DataBase::checkLogin(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "Ошибка при получении результата запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result);

    return (num_rows > 0);
}
//проверка пароля пр входе на сервер
bool DataBase::checkPassword(const std::string& password)
{
    std::string query = "SELECT * FROM users WHERE password='" + password + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "Ошибка при получении результата запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result);

    return (num_rows > 0);
}
//запрос для проверки логина пользователя
bool DataBase::checkUserLogin(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "Ошибка при получении результата запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result);

    return (num_rows > 0);
}
//запрос для проверки пароля пользователя
bool DataBase::checkUserPassword(const std::string& password)
{
    std::string query = "SELECT * FROM users WHERE password='" + password + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "Ошибка при получении результата запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result);

    return (num_rows > 0);
}
//проверка статуса блокировки
bool DataBase::isBanned(const std::string& login)
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    std::string query = "SELECT isbanned FROM users WHERE login = '" + login + "';";

    if (mysql_query(&mysql, query.c_str()))
    {
        std::cerr << "Ошибка выполнения запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    res = mysql_store_result(&mysql);
    if (res == nullptr)
    {
        std::cerr << "Ошибка получения результата запроса: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    bool isBanned = false;

    // Если пользователь найден, получаем значение столбца isbanned
    if (mysql_num_rows(res) > 0)
    {
        row = mysql_fetch_row(res);
        isBanned = (std::stoi(row[0]) != 0); // Преобразуем значение в bool
    }

    // Освобождаем результат запроса
    mysql_free_result(res);

    return isBanned;
}
//запрос на получение имени пользователя по логину
std::string DataBase::takeUserName(const std::string& login)
{
    std::string userName;

    std::string query = "SELECT name FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                userName = row[0];
            }
            mysql_free_result(res);
        }
    }
    else
    {
        std::cerr << "Ошибка выполнения запроса: " << mysql_error(&mysql) << std::endl;
    }

    return userName;
}
//проверка существования логина в БД
bool DataBase::checkLoginExistsInDB(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0) {
        std::cerr << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result != nullptr) {
        int num_rows = mysql_num_rows(result);

        mysql_free_result(result);

        return (num_rows > 0);
    }
    else {
        std::cerr << "Ошибка при получении результата запроса SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_free_result(result);
        return false;
    }
}
//проверка существования имени в БД
bool DataBase::checkNameExistsInDB(const std::string& name)
{
    if (name == "all" || name == "All" || name == "ALL") {
        return true;
    }

    std::string query = "SELECT * FROM users WHERE name='" + name + "'";

    if (mysql_query(&mysql, query.c_str()) != 0) {
        std::cout << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    res = mysql_store_result(&mysql);
    if (res != nullptr) {
        int num_rows = mysql_num_rows(res);

        mysql_free_result(res);
        return (num_rows > 0);
    }
    else {
        std::cout << "Ошибка при получении результата запроса SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_free_result(res);
        return false;
    }
}
//добавление пользователя в БД
bool DataBase::addNewUser(const std::string& login, const std::string& password, const std::string& name)
{
    std::unique_lock<std::shared_mutex> lock(sharedMutex); //мьютекс перед выполнением операции записи

    std::string query = "INSERT INTO users (login, password, name) VALUES ('" + login + "', '" + password + "', '" + name + "')";

    if (mysql_query(&mysql, query.c_str()) == 0) {
        std::cout << "Новый пользователь добавлен в базу данных" << std::endl;
        return true;
    }
    else {
        std::cout << "Ошибка при выполнении запроса INSERT: " << mysql_error(&mysql) << std::endl;
        return false;
    }
}
//список пользователей и их статус
std::string DataBase::takeAllUsersNameStatus()
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    std::string allUsersData;

    std::string query = "SELECT name, online FROM users WHERE isbanned = 0";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            int numFields = mysql_num_fields(res);

            // Перебор строк с результатами запроса
            while ((row = mysql_fetch_row(res)))
            {
                for (int i = 0; i < numFields; ++i)
                {
                    std::string name = row[i++];
                    std::string status = row[i];
                    allUsersData += "|" + name + "|" + status;
                }
            }
            mysql_free_result(res);
        }
    }
    else
    {
        std::cerr << "Ошибка выполнения запроса: " << mysql_error(&mysql) << std::endl;
    }

    allUsersData = "all" + allUsersData;

    return allUsersData;
}
//список пользователей и их статус, статус блокировки
std::string DataBase::takeAllUsersNameStatusIsBann()
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    std::string allUsersData;

    std::string query = "SELECT name, online, isbanned FROM users";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            int numFields = mysql_num_fields(res);

            while ((row = mysql_fetch_row(res)))
            {
                std::string name = row[0];
                std::string status = row[1];
                std::string bann = row[2];
                allUsersData += name + "|" + status + "|" + bann + "|";
            }
            mysql_free_result(res);
        }
        else
        {
            std::cerr << "Ошибка выполнения запроса: " << mysql_error(&mysql) << std::endl;
        }
    }
    else
    {
        std::cerr << "Ошибка выполнения запроса: " << mysql_error(&mysql) << std::endl;
    }

    allUsersData = "inf|" + allUsersData;

    return allUsersData;
}
//изменение статуса блокировки пользователя
void DataBase::setBannStatus(const std::string& name, bool bann)
{
    std::unique_lock<std::shared_mutex> lock(sharedMutex); //мьютекс перед выполнением операции записи

    int status = bann ? 1 : 0;

    std::ostringstream queryStream;
    queryStream << "UPDATE users SET isbanned = " << status << " WHERE name = '" << name << "'";

    std::string query = queryStream.str();
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "Пользователь " << name << " заблокирован" << std::endl;
    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//изменение статуса пользователя
void DataBase::setUserStatus(const std::string& login, bool online)
{
    std::unique_lock<std::shared_mutex> lock(sharedMutex); //мьютекс перед выполнением операции записи

    int status = online ? 1 : 0;

    std::ostringstream queryStream;
    queryStream << "UPDATE users SET online = " << status << " WHERE login = '" << login << "'";

    std::string query = queryStream.str();
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "Статус пользователя " << login << " обновлен" << std::endl;
    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//сброс статусов всех пользователей, нужен при запуске сервера
void DataBase::resetAllUsersStatus()
{
    std::lock_guard<std::mutex> lock(dbMutex); //мьютекс перед выполнением операции

    std::string query = "UPDATE users SET online = 0";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "Статус всех пользователей сброшен в базе данных" << std::endl;
    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//добавление сообщения для всех в БД
void DataBase::addMessageToAll(const std::string& from, const std::string& to, const std::string& text, bool toAll)
{
    std::unique_lock<std::shared_mutex> lock(sharedMutex); //мьютекс перед выполнением операции записи

    // Создаем запрос для добавления нового сообщения в таблицу "messages"
    std::string query = "INSERT INTO messages (from_id, to_id, text, to_all) VALUES ('" + from + "', '" + to + "', '" + text + "', " + "1" + ")";
    // Выполняем запрос к базе данных
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "Новое сообщение добавлено в базу данных" << std::endl;

    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса INSERT: " << mysql_error(&mysql) << std::endl;
    }
}
//добавление сообщения для пользователя в БД
void DataBase::addMessageToDB(const std::string& from, const std::string& to, const std::string& text)
{
    std::unique_lock<std::shared_mutex> lock(sharedMutex); //мьютекс перед выполнением операции записи

    std::string query = "INSERT INTO messages (from_id, to_id, text) VALUES ('" + from + "', '" + to + "', '" + text + "')";
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "Новое сообщение добавлено в базу данных" << std::endl;

    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса INSERT: " << mysql_error(&mysql) << std::endl;
    }
}
//вывод из БД сообщения для всех
std::string DataBase::loadMessagesToAll()
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    std::string messages;

    std::string query = "SELECT timestamp, from_id, text FROM messages WHERE to_all = 1";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(&mysql);
        if (result != nullptr)
        {
            int numFields = mysql_num_fields(result);

            while ((row = mysql_fetch_row(result)))
            {
                std::string timestamp = row[0];
                std::string from_id = row[1];
                std::string text = row[2];

                messages = messages + "|" + timestamp + "|" + from_id + "|" + text;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
    }
    messages = "lma" + messages;
    return messages;
}
//вывод из бд сообщений от и для пользователя
std::string DataBase::loadMessagesToUser(const std::string& to)
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    std::string messages;

    std::string query = "SELECT timestamp, from_id, to_id, text FROM messages WHERE (from_id = '" + to + "' OR to_id = '" + to + "') AND to_all = 0 "
        "ORDER BY timestamp ASC"; // Сортировка по времени добавления (возможно не нужна)

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(&mysql);
        if (result != nullptr)
        {
            int numFields = mysql_num_fields(result);

            while ((row = mysql_fetch_row(result)))
            {
                std::string timestamp = row[0];
                std::string from_id = row[1];
                std::string to_id = row[2];
                std::string text = row[3];

                messages = messages + "|" + timestamp + "|" + from_id + "|" + to_id + "|" + text;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        std::cerr << "Ошибка при выполнении запроса SELECT: " << mysql_error(&mysql) << std::endl;
    }

    messages = "lmu" + messages;
    return messages;
}

//////////////////////////////////тестовый метод////////////////////////////
//выводит содержимое таблицы users
void DataBase::showAllUsers()
{
    std::shared_lock<std::shared_mutex> lock(sharedMutex); //Блокируем на время чтение данных

    mysql_query(&mysql, "SELECT * FROM users"); //Делаем запрос к таблице

    //Выводим все что есть в базе через цикл
    if (res = mysql_store_result(&mysql)) {
        while (row = mysql_fetch_row(res)) {
            for (int i = 0; i < mysql_num_fields(res); i++) {
                std::cout << row[i] << "  ";
            }
            std::cout << std::endl;
        }
    }
}