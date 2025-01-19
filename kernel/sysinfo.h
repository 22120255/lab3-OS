struct sysinfo {
  uint64 freemem;   // amount of free memory (bytes)
  uint64 nproc;     // number of process
  uint64 loadavg[3]; // Load average (1, 5, 15 phút)
};
