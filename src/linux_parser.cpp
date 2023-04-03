#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO:DONE Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float val;
  float memTotal = -1.0;
  float memFree = -1.0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // Parse line to only keep "<key> <val>"
      line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
      line.erase(std::remove(line.begin(), line.end(), 'k'), line.end());
      line.erase(std::remove(line.begin(), line.end(), 'B'), line.end());
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> val;

      // Search for <keys> -> they do not require to be in a sepcific order
      if (key == "MemTotal") memTotal = val;

      if (key == "MemFree") memFree = val;
    }
  }
  // Check if both keys have been found -> if not return a -1.0 to denote
  // something went wrong in the GUI
  if (abs(memTotal - (-1.0)) < 0.1 || abs(memFree - (-1.0)) < 0.1)
    return -1.0;
  else
    return (memTotal - memFree) / memTotal;
}

// TODO:DONE Read and return the system uptime
long int LinuxParser::UpTime() {
  string line;
  long int upTime = -1;

  std::ifstream inputFile(kProcDirectory + kUptimeFilename);

  if (inputFile.is_open()) {
    std::getline(inputFile, line);
    std::istringstream linestream(line);

    // return only uptime of the system (not the time spent in the idle process)
    linestream >> upTime;
  }

  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO:DONE Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream inputFile(kProcDirectory + kStatFilename);
  vector<string> fields;

  if (inputFile.is_open()) {
    string line;
    string key;
    string val;
    // We only need to get the first line.
    std::getline(inputFile, line);
    // We create a string stream
    std::istringstream lineStream(line);

    lineStream >> key;
    // We make sure that the key is the aggregate CPU -> may not be needed
    if (key == "cpu") {
      // iterate over all the values present in the CPU aggregate line
      while (lineStream >> val) fields.push_back(val);
    }
  }

  return fields;
}

// TODO:DONE Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") return value;
    }
  }

  return -1;
}

// TODO:DONE Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") return value;
    }
  }

  return -1;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
