﻿#include "command.h"

static const char NAME_SAPCE_CHAR = ' ';

static string getFirstName(const string& in) {
	return in.substr(0, in.find_first_of(NAME_SAPCE_CHAR));
}

static string getLastName(const string& in) {
	return in.substr(in.find_first_of(NAME_SAPCE_CHAR) + 1);
}

static string getPhoneNumMid(const string& in) {
	return in.substr(4, 4);
}

static string getPhoneNumLast(const string& in) {
	return in.substr(9, 4);
}

static void Print(const string& outstr) {
	cout << outstr << endl;
}

static string Output(string name, bool opt, const employeeList& list) {
	string ret = "";
	if (list.size() == 0) {
		Print(name + ",NONE");
		ret = name + ",NONE\n";
		return ret;
	}

	if (opt) {
		int i = 0;
		for (auto empl : list) {
			Print(name + ',' + empl.to_string());
			ret += name + ',' + empl.to_string() + "\n";
			if (i++ == 4) break;
		}
	}
	else {
		Print(name + ',' + to_string(list.size()));
		ret += name + ',' + to_string(list.size()) + "\n";
	}

	return ret;
}

SearchInput Command::get_search_input(void) {
	SearchInput search_input;
	search_input.search_pattern = get_command()[5];

	if ("employeeNum" == get_command()[4]) {
		search_input.search_type = SearchType::EMPLOYEE_NUM;
	}
	else if ("name" == get_command()[4]) {
		if ("-f" == get_command()[2]) {
			search_input.search_type = SearchType::FIRST_NAME;
		}
		else if ("-l" == get_command()[2]) {
			search_input.search_type = SearchType::LAST_NAME;
		}
		else {
			search_input.search_type = SearchType::NAME;
		}
	}
	else if ("cl" == get_command()[4]) {
		search_input.search_type = SearchType::CL;
	}
	else if ("phoneNum" == get_command()[4]) {
		if ("-m" == get_command()[2]) {
			search_input.search_type = SearchType::PHONE_NUM_MID;
		}
		else if ("-l" == get_command()[2]) {
			search_input.search_type = SearchType::PHONE_NUM_LAST;
		}
		else {
			search_input.search_type = SearchType::PHONE_NUM;
		}
	}
	else if ("birthday" == get_command()[4]) {
		if ("-y" == get_command()[2]) {
			search_input.search_type = SearchType::BIRTHDAY_YEAR;
		}
		else if ("-m" == get_command()[2]) {
			search_input.search_type = SearchType::BIRTHDAY_MONTH;
		}
		else if ("-d" == get_command()[2]) {
			search_input.search_type = SearchType::BIRTHDAY_DAY;
		}
		else {
			search_input.search_type = SearchType::BIRTHDAY;
		}
	}
	else if ("certi" == get_command()[4])
	{
		search_input.search_type = SearchType::CERTI;
	}

	return search_input;
}

string AddCommand::Process(Employees* database) {
	Employee employee = { get_command()[4], get_command()[5], get_command()[6], get_command()[7], get_command()[8], get_command()[9] };
	database->Add(employee);

	return "";
}

void AddCommand::CommandValidation() {
	if (ADD_COMMAND_SIZE != get_command().size()) {
		print_error_msg();
	}
}

string DelCommand::Process(Employees* database) {
	shared_ptr<Search> search(new Search());
	search->SetEmployeeList(database);
	employeeList delete_employee_list = std::move(search->DoSearch(get_search_input()));

	for (auto delete_employee : delete_employee_list) {
		database->Del(delete_employee);
	}

	return Output(get_name(), get_command()[1] == "-p", delete_employee_list);
}

void DelCommand::CommandValidation() {
	if (DEL_COMMAND_SIZE != get_command().size()) {
		print_error_msg();
	}
}

string SearchCommand::Process(Employees* database) {
	shared_ptr<Search> search(new Search());
	search->SetEmployeeList(database);
	employeeList search_employee_list = std::move(search->DoSearch(get_search_input()));

	return Output(get_name(), get_command()[1] == "-p", search_employee_list);
}

void SearchCommand::CommandValidation() {
	if (SEARCH_COMMAND_SIZE != get_command().size()) {
		print_error_msg();
	}
}

using mod_func = function<void(Employee&)>;

static mod_func GetModFunction(string target, string modify_val);

string ModifyCommand::Process(Employees* database) {
	shared_ptr<Search> search(new Search());
	search->SetEmployeeList(database);
	employeeList modify_employee_list = std::move(search->DoSearch(get_search_input()));

	for (auto employee : modify_employee_list) {
		database->Modify(employee, GetModFunction(get_command()[6], get_command()[7]));
	}

	return Output(get_name(), get_command()[1] == "-p", modify_employee_list);
}

void ModifyCommand::CommandValidation() {
	if (MODIFY_COMMAND_SIZE != get_command().size()) {
		print_error_msg();
	}
}

static mod_func GetModFunction(string target, string modify_val) {
	if (target == "name")
		return [modify_val](Employee& empl) { empl.first_name = getFirstName(modify_val); empl.last_name = getLastName(modify_val); };

	if (target == "cl")
		return [modify_val](Employee& empl) { empl.cl = StrToCL(modify_val); };

	if (target == "phoneNum")
		return [modify_val](Employee& empl) { empl.phone_num_mid = stoi(getPhoneNumMid(modify_val)); empl.phone_num_last = stoi(getPhoneNumLast(modify_val)); };

	if (target == "birthday")
		return [modify_val](Employee& empl) { empl.birth = stoi(modify_val); };

	if (target == "certi")
		return [modify_val](Employee& empl) { empl.certi = StrToCerti(modify_val); };

	throw invalid_argument("알 수 없는 column 수정 시도");
};