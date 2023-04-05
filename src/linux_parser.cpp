#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream inputFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  string value;
  vector<string> fields;

  if (inputFile.is_open()) {
    std::getline(inputFile, line);
    std::istringstream lineStream(line);
    while (lineStream >> value) fields.push_back(value);

    return (std::stol(fields[pUtime_ - 1]) + std::stol(fields[pStime_ - 1]) +
            std::stol(fields[pCuTime_ - 1]) + std::stol(fields[pCsTime_ - 1]));
  }

  return -1;
}

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
string LinuxParser::Command(int pid) {
  string pidStr = std::to_string(pid);
  std::ifstream inputFile(kProcDirectory + pidStr + kCmdlineFilename);
  string line = "CMD_NOT_FOUND";

  if (inputFile.is_open()) {
    std::getline(inputFile, line);
  }

  return line.substr(0, 50);
}

string LinuxParser::Ram(int pid) {
  std::ifstream inputFile(kProcDirectory + std::to_string(pid) +
                          kStatusFilename);
  string line;
  string key;
  string val;

  if (inputFile.is_open()) {
    while (std::getline(inputFile, line)) {
      string key;
      string value;
      // Remove all the white spaces and k and B reference
      line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
      line.erase(std::remove(line.begin(), line.end(), 'k'), line.end());
      line.erase(std::remove(line.begin(), line.end(), 'B'), line.end());
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream lineStream(line);
      lineStream >> key >> val;

      if (key == "VmSize")
        // return value but in Megabytes -> simply remove last three positions
        return val.erase(val.length() - 3);
    }
  }

  return "";
}

string LinuxParser::Uid(int pid) {
  string pidStr = std::to_string(pid);
  std::ifstream inputFile(kProcDirectory + pidStr + kStatusFilename);
  string line;

  if (inputFile.is_open()) {
    while (std::getline(inputFile, line)) {
      string key;
      string value;
      // Remove all the white spaces
      line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "Uid") return value;
    }
  }

  return "";
}

string LinuxParser::User(int pid) {
  string uidTarget = Uid(pid);
  string line;
  std::ifstream inputFile(kPasswordPath);

  if (inputFile.is_open()) {
    while (std::getline(inputFile, line)) {
      string user;
      string _access;
      string uid;
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> user >> _access >> uid;
      if (uid == uidTarget) return user.substr(0, 6);
    }
  }

  return "NOT_FOUND_USER";
}

long LinuxParser::UpTime(int pid) {
  std::ifstream inputFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  vector<string> fields;
  string value;
  if (inputFile.is_open()) {
    std::getline(inputFile, line);
    std::istringstream lineStream(line);
    while (lineStream >> value) fields.push_back(value);

    return std::stol(fields[pStartTime_ - 1]) / sysconf(_SC_CLK_TCK);
  }

  return -1;
}
