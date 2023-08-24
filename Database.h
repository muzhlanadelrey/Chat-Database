﻿#pragma once
#include <iostream>
#include <mysql.h>
#include <mutex>
#include <shared_mutex>

class DataBase {
private:
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	std::mutex dbMutex; //мьютекс для защиты доступа к данным базы данных
	std::shared_mutex sharedMutex; //разделяемая блокировка

	void createDataBase();//создание новой базы данных, если не найдена старая
	void createTables();//создание таблиц, если они отсутствуют
	void createAdmin();//создание пользователя с правами администратора, если такого нет

public:
	void dataBaseConnect(); // подключение к базе данных
	void dataBaseDisconnect(); // отключение от базы данных

	bool checkLogin(const std::string& login); //проверка логина при входе на сервер
	bool checkPassword(const std::string& password);//проверка пароля при входе на сервер

	bool checkUserLogin(const std::string& login);//проверка логина пользователя
	bool checkUserPassword(const std::string& password);//проверка пароля пользователя
	bool isBanned(const std::string& login);//проверка забанен ли пользователь

	std::string takeUserName(const std::string& login);//возвращает имя пользователя по логину

	bool checkLoginExistsInDB(const std::string& login);//проверка не занят ли логин
	bool checkNameExistsInDB(const std::string& name);//проверка не занято ли имя

	bool addNewUser(const std::string& login, const std::string& password, const std::string& name);//добавляет нового пользователя в БД
	std::string takeAllUsersNameStatus();//возвращает список всех пользователей и их статус

	std::string takeAllUsersNameStatusIsBann();//информация для админа
	void setBannStatus(const std::string& name, bool bann);//изменить статус бана пользователя

	void setUserStatus(const std::string& login, bool online);//устанавливает статус пользователя online/offline
	void resetAllUsersStatus();//устанавливает статусы всех пользователей = 0

	void addMessageToAll(const std::string& from, const std::string& to, const std::string& text, bool toAll);//добавление нового сообщения для всех в базу данных
	void addMessageToDB(const std::string& from, const std::string& to, const std::string& text);//добавление нового сообщения для пользователя в базу данных

	std::string loadMessagesToAll();//сообщения для всех пользователей
	std::string loadMessagesToUser(const std::string& to);//сообщения для текущего пользователя

	//выводит список пользователей из БД
	void showAllUsers();
};