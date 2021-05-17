#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_

#include <stddef.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "base/base_export.h"
#include "build/build_config.h"

namespace base {
class  BASE_EXPORT CommandLine {
public:
	using StringVector = std::vector<std::string>;
private:
	static CommandLine* current_process_commandline_;
	StringVector args;
public:
	static CommandLine* ForCurrentProcess() {
		return current_process_commandline_;
	}
	bool HasSwitch(const char sw[]) {
		std::string sw_name = "-" + std::string(sw);
		for (size_t i = 0; i < args.size(); i++) {
			if (args[i] == sw_name) {
				return true;
			}
		}
		return false;
	}
	static bool Init(int argc, const char** argv) {
		current_process_commandline_ = new CommandLine();
		// i = 0: courgette (command name)
		for (int i = 1; i < argc; i++) {
			current_process_commandline_->args.push_back(argv[i]);
		}
		return true;
	}
	StringVector GetArgs() {
		StringVector args_files;
		size_t i = 0;
		for (i = 0; i < args.size(); i++) {
			if (args[i][0] != '-') {
				break;
			}
		}
		for (;i < args.size(); i++) {
			args_files.push_back(args[i]);
		}
		return args_files;
	}

	std::string GetSwitchValueASCII(std::string option) {
	// TODO
		return std::string("3");
	}
};
CommandLine* CommandLine::current_process_commandline_ = nullptr;
}  // namespace base
#endif // BASE_COMMAND_LINE_H_

