# Nino-Sketch

As software systems become more complex, rigorous monitoring and analysis of system behaviors and anomalies become essential for ensuring reliability, performance, and security. This requires managing high-speed data streams from various sources, such as network flows, within tight and fast memory constraints. In this paper, we present Nino-Sketch, an accurate and memory-efficient probabilistic data structure designed for real-time analysis in resource-constrained environments. To improve memory efficiency, we integrate all fields into a single unit and operate each counter on a bit-level, allowing flexible size adjustments based on the tracked item's frequency or persistence. We protect important data by categorizing hot and cold items into different stages and using a probability-based eviction method. We apply Nino-Sketch to four distinct detection tasks, and through extensive evaluation, we demonstrate that it achieves outstanding detection accuracy and commendable processing speed.

## Compile and Run the Examples
Nino-Sketch is implemented in C++. Below, we provide instructions for compiling the examples on Ubuntu using g++ and make.

### Requirements
Before proceeding, please ensure that the following requirements are met:

- g++ and make are installed on your system. Our experimental platform uses Ubuntu 20.04 and g++ 9.4.0.
- The libpcap library is installed on your system. Most Linux distributions include libpcap, and it can be installed using package managers such as apt-get in Ubuntu.

### Dataset
- You can download the pcap file and specify the path of each pcap file in the iptraces.txt file.

### Different Detection Tasks
- The Nino_frequency folder is for frequency estimation and heavy item detection.
- The Nino_persistence folder is for persistence estimation and persistent item lookup.

### Compile
- To compile the examples, use the following commands with make:

```
    $ make main_hitter
```

### Run
- To execute the examples, run the following commands. The program will output statistics about the detection accuracy.

```
    $ ./main_hitter
```
