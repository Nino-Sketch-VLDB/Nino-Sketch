# Nino-Sketch

Rigorous monitoring and analysis of system behaviors and anomalies are crucial for ensuring reliability, performance, and security. This task involves managing high-speed data streams from various sources, such as network flows, within stringent memory constraints. In this paper, we introduce Nino-Sketch, a highly accurate and memory-efficient probabilistic data structure tailored for real-time analysis in resource-constrained environments. To enhance memory efficiency, Nino-Sketch integrates all fields into a single unit and operates counters at the bit level, enabling flexible size adjustments based on the frequency or persistence of tracked items. We safeguard critical data by categorizing items into hot and cold stages and employing a probability-based eviction method. Through extensive evaluation across four distinct detection tasks, we demonstrate that Nino-Sketch delivers exceptional detection accuracy and impressive processing speed.

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
