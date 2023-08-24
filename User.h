﻿#pragma once
#include <string>
#include <fstream>

using namespace std;

class User
{
public:
	User() = default;
	~User() = default;

	User(const string& login_user, const string& name_user, const string& password_user) : _login(login_user), _name(name_user), _password(password_user) {} // êîíñòðóêòîð ñ ïàðàìåòðàìè

	void setUserLogin(const string& login);
	string& getUserLogin();
	void setUserName(const string& name);
	string& getUserName();
	void setUserPassword(const string& password);
	string& getUserPassword();

private:
	string _login;
	string _name;
	string _password;
};