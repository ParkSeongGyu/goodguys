#include <iostream>
#include <string>
#include <memory>

#include "file_input_manager.h"
#include "file_output_manager.h"
#include "input_parser_manager.h"
#include "../Command/Command.h"
#include "../Employees/Employees.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3 || !argv[1] || !argv[2]) return EXIT_FAILURE;

	unique_ptr< FileInputManager> inputmanager = make_unique<FileInputManager>(argv[1]);
	unique_ptr< FileOutputManager> outputmanager = make_unique<FileOutputManager>(argv[2]);
	unique_ptr< InputParserManager> parsermanager = make_unique<InputParserManager>();

	if (!inputmanager->IsFileValid()) return EXIT_FAILURE;

	vector<Command*> command_list = parsermanager->GetCommandList(inputmanager->GetInputStringsFromFile());

	Employees* employees = CreateEmployees();
	for (auto a_command : command_list) {
		string result_string = a_command->Process(employees);
		outputmanager->WriteResultToFile(result_string);
	}

	return EXIT_SUCCESS;
}

