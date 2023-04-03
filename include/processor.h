#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  enum timeTypes {
    USER = 0,
    NICE,
    SYSTEM,
    IDLE,
    IOWAIT,
    IRQ,
    SOFTIRQ,
    STEAL,
    GUEST,
    GUEST_NICE
  };
  // TODO: Declare any necessary private members
 private:
};

#endif