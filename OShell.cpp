#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "cmd.hpp"
#include "eval.hpp"

string readFile(string filename) {
	char* buffer = 0;
	long length;
	FILE* f = fopen(filename.c_str(), "rb");

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = new char[length + 1];
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		buffer[length] = '\0';
		fclose(f);
	}

	if (!buffer) {
		printf("Unable to open file %s!", filename.c_str());
		exit(-1);
	}
	return string(buffer);
}

vector<string> split(const string& s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

string replace(string str, string what, string replacement) {
	int iLength = what.length();
	size_t index = 0;
	while (true) {
		index = str.find(what, index);
		if (index == std::string::npos) {
			break;
		}
		str.replace(index, iLength, replacement);
		index += iLength;
	}
	return str;
}

int ifSkip = 0;
map<string, string> variables;

bool isTrue(string condition) {
	return eval(condition) != 0;
}

void runFile(string filename);

void execute(string command) {
	if (command == "") {
		return;
	}
	for (pair<string, string> arg : variables) {
		command = replace(command, "${" + arg.first + "}", arg.second);
	}
	vector<string> args = split(command, ' ');
	if (ifSkip == 1) {
		if (args[0] == "else") {
			ifSkip = 0;
			if (args.size() > 1) {
				fprintf(stderr, "Too many arguments in command else!\n");
			}
			return;
		} else if (args[0] == "elseif") {
			if (args.size() == 1) {
				fprintf(stderr, "Not enough arguments in command elseif!\n");
			} else {
				if (isTrue(command.substr(7))) {
					ifSkip = 0;
				}
			}
			return;
		}
	} else if (ifSkip == 0) {
		if (args[0] == "else") {
			ifSkip = 1;
			if (args.size() > 1) {
				fprintf(stderr, "Too many arguments in command else!\n");
			}
			return;
		} else if (args[0] == "elseif") {
			if (args.size() == 1) {
				fprintf(stderr, "Not enough arguments in command elseif!\n");
			}
			ifSkip = 1;
			return;
		}
	}
	if (ifSkip != 0) {
		if (args[0] == "if") {
			ifSkip++;
		} else if (args[0] == "endif") {
			ifSkip--;
			if (args.size() > 1) {
				fprintf(stderr, "Too many arguments in command endif!\n");
			}
		}
	} else {
		if (args.size() > 1 && args[1] == "=") {
			if (args.size() == 2) {
				fprintf(stderr, "Not enough arguments for variable assignment!\n");
			} else {
				string expression = command.substr(args[0].length() + 3);
				if (expression[0] == '"') {
					variables[args[0]] = expression.substr(1, expression.length() - 2);
				} else {
					variables[args[0]] = to_string(eval(expression));
				}
			}
		} else if (args[0] == "echo") {
			if (args.size() == 1) {
				fprintf(stderr, "Not enough arguments in command echo!\n");
			} else {
				printf("%s\n", command.substr(5).c_str());
			}
		} else if (args[0] == "if") {
			if (args.size() == 1) {
				fprintf(stderr, "Not enough arguments in command if!\n");
			} else {
				ifSkip += !isTrue(command.substr(3));
			}
		} else if (args[0] == "run") {
			if (args.size() == 1) {
				fprintf(stderr, "Not enough arguments in command run!\n");
			} else {
				runFile(command.substr(4));
			}
		} else if (args[0] != "endif") {
			printf("%s\n", command.c_str());
			system(command.c_str());
		}
	}
}

void runFile(string filename) {
	string text = readFile(filename);
	string command = "";
	int index = 0;
	while (true) {
		if (text[index] == '\n' || text[index] == '\0') {
			execute(command);
			command = "";
			index++;
			if (text[index - 1] == '\0') {
				return;
			}
		} else if (text[index] == '\\') {
			command += text[++index];
			index++;
		} else {
			command += text[index++];
		}
	}
}

int main(int argc, char** argv) {
	if (argc == 1) {
		string command = "";
		printf(">>> ");
		while (true) {
			char c;
			scanf("%c", &c);
			if (c == '\n') {
				execute(command);
				printf(">>> ");
				command = "";
			} else if (c == '\\') {
				scanf("%c", &c);
				command += c;
			} else {
				command += c;
			}
		}
	} else {
		string filename = argv[1];
		for (int i = 2; i < argc; i++) {
			if (argv[i][0] == '-') {
				variables["cmd." + string(argv[i] + 1)] = (argc - i == 1) || argv[i + 1][0] == '-' ? "1" : string(argv[i + 1]);
			}
		}
		runFile(filename);
	}
	return 0;
}