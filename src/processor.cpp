#include "processor.h"
// FBL: Add linux_parser to access namespace methods
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

float Processor::Utilization() {
  vector<string> cpuTimesStr = LinuxParser::CpuUtilization();
  vector<float> cpuTimes;

  // transform the complete vector to a vector of long ints for computation
  // back inserter needed to guarantee that there are enough elements to hold
  // the output in the transform operation
  std::transform(cpuTimesStr.begin(), cpuTimesStr.end(),
                 std::back_inserter(cpuTimes),
                 [](string time) { return std::stof(time); });

  // Formula taken from:
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  float idle = cpuTimes[LinuxParser::kIdle_] + cpuTimes[LinuxParser::kIOwait_];
  float nonIdle =
      cpuTimes[LinuxParser::kUser_] + cpuTimes[LinuxParser::kNice_] +
      cpuTimes[LinuxParser::kSystem_] + cpuTimes[LinuxParser::kIRQ_] +
      cpuTimes[LinuxParser::kSoftIRQ_] + cpuTimes[LinuxParser::kSteal_];
  float total = idle + nonIdle;

  return (total - idle) / total;
}