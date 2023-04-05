#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System(void) {
  kernel_ = LinuxParser::Kernel();
  os_ = LinuxParser::OperatingSystem();
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<int> pidsFound = LinuxParser::Pids();
  processes_.clear();
  // Create new processes
  for (int pid : pidsFound) {
    processes_.emplace_back(pid);
  }

  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return os_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }