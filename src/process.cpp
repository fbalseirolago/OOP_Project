#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  command_ = LinuxParser::Command(pid);
  user_ = LinuxParser::User(pid);
  upTime_ = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const {
  float totalTime = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  float startTime = LinuxParser::UpTime(pid_);
  float upTime = LinuxParser::UpTime();
  float retVal = 0.0;
  if (upTime != startTime) retVal = totalTime / (upTime - startTime);

  return retVal;
}

string Process::Command() { return command_; }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

long int Process::UpTime() { return upTime_; }

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}