﻿#pragma once
#include <string>
#include <fstream>
using namespace std;

class Message
{
public:
	Message() = default; 
	~Message() = default;

	Message(const string& login_user, const string& name_current_reciption, const string& message_current) : _sender(login_user), _recipient(name_current_reciption), _message(message_current) {} // êîíñòðóêòîð ñ ïàðàìåòðàìè

	void setMessage(const string& message);
	string getMessage() const;

	void setSender(const string& sender);
	string getSender() const;

	void setRecipient(const string& recipient);
	string getRecipient() const;

private:
	string _sender;
	string _recipient; 
	string _message;
};