<!-- HEADER -->
# pynq-voronoi-accelerator
Demonstration of hardware acceleration with the PYNQ-Z1 FPGA on a Voronoi diagram, along with other HPC methods.


<!-- TABLE OF CONTENTS -->
### Table of Contents
<ol>
  <li><a href="#about-the-project">About The Project</a></li>
  <li>
    <a href="#instructions">Instructions</a>
    <ol>
      <li> <a href="#hardwaresoftware-requirements">Hardware/Software Requirements</a>
      <li> <a href="#pynq-implementation">PYNQ Implementation</a>
      <li> <a href="#c-python-implementations">C/Python Implementations</a>
      <li> <a href="#makefile">Makefile</a>
      <li> <a href="#graphing">Graphing</a>
    </ol>
  </li>
  <li><a href="#results">Results</a></li>
</ol>


<!-- ABOUT THE PROJECT -->
## About The Project
This repository contains code relating to Voronoi diagrams, which partition coordinates within a 2D plane into regions, each associated with a predefined center seed/coordinate. The Euclidean distance between each coordinate on the plane and each of the seeds is calculated and each coordinate is then assigned to the region of the closest seed.

A simplified version of this algorithm is used with fixed problem size and seed values to serve as a constant during testing. Various HPC methods are implemented for comparison of  execution times on different platforms. The primary goal of this research was to explore hardware acceleration using the Digilent PYNQ-Z1 FPGA and evaluate how it performs in comparison to CPU and GPU-based implementations. The following methods were used for comparison:

1. Sequential Python 
2. Parallel Python using Numba
3. Sequential C
4. Parallel C using OpenMP
5. Parallel C using OpenMP (GPU)
6. HLS with PYNQ-Z1


<!-- SETUP -->
## Instructions

If anybody wishes to replicate this project to expand on what is done here, or simply test it out for themselves, a comprehensive set of instructions has been included on how to run everything.

### Hardware/Software Requirements

The following hardware and software is needed to deploy the HLS-generated overlay onto the FPGA:

- Digilent PYNQ-Z1 FPGA (v3.0.1 image)
- AMD Vitis HLS (v2023.1)
- AMD Vivado Design Suite (v2023.1)
- Jupyter Notebook (pre-installed on PYNQ-Z1 image)

The following software is needed for all C and Python implementations:

- Python (v3.12.3)
- pip (v24.0)
- gcc (v13.3.0)
- Optional: gnuplot (v6.0) 

> **NOTE:** Using other software versions of these tools may result in this instruction set not working properly.

Execution times for C and Python implementations (e.g., sequential, Numba, OpenMP) will vary significantly depending on the hardware of the host machine. Below are the specifications of the machine used during testing:

```
--------------------[ SYSTEM INFORMATION ]--------------------
Hostname: XXXXXXXXXXXXXXXXXXXXXXXXX
CPU: AMD Ryzen 7 4800H with Radeon Graphics
Cores: 16
Threads per core: 2
Memory: 7.4Gi
```

> **NOTE:** Benchmark result logs automatically include system specs for reproducibility and comparison across machines.

All Python and C files were compiled and executed using Windows Subsystem for Linux (WSL2). The provided Makefile relies on Linux-specific commands for logging and execution, so it is intended for use within in a Linux-based environment. The source code can still be run manually on Windows or macOS if the necessary tools and dependencies are installed.

### PYNQ Implementation

This section describes how to build and deploy the hardware-accelerated Voronoi algorithm as a custom overlay on the Digilent PYNQ-Z1 FPGA using Vitis HLS and Vivado Design Suite.

1. Generate IP using Vitis HLS
    1. Open Vitis HLS and create a new project.
        - Name the project (e.g., voronoi_ip)

            ![Name HLS Project](assets/name_hls.png)

        - Add the [voronoi_hls.cpp](src/pynq/voronoi_hls.cpp) source file to the project.
        - Set the top function name to your HLS function (calculate_voronoi)

            ![Add HLS Sources](assets/sources_hls.png)

        - Set the part to `xc7z020clg400-1`.

            ![HLS Part](assets/part_hls.png)
        
    2. Click "Run C Synthesis" to generate RTL. Make sure there are no errors after completion.

        ![C Synthesis Button](assets/synthesis_btn.png)
        ![C Synthesis Window](assets/synthesis_win.png)
    
    3. Click "Export RTL", keep the default settings, and save the generated IP.

        ![Export RTL Button](assets/export_rtl_btn.png)
        ![Export RTL Window](assets/export_rtl_win.png)

2. Build Overlay in Vivado
    1. Open Vivado and create a new project:
        - Name the project (e.g., voronoi_overlay).

            ![Name Vivado Project](assets/name_vivado.png)

        - Keep default settings (make sure that RTL project is selected, do not add sources or constraints).

            ![RTL Project](assets/rtl_vivado.png)

        - Set the part to `xc7z020clg400-1`.

            ![Vivado Part](assets/part_vivado.png)

    2. Add your HLS-generated IP:
        - Go to "Project Manager" > "Settings" > "IP" > "Repository", and add the path of the HLS project.

            ![Add IP Repository](assets/ip_repo.png)

        - Refresh the IP catalog; your IP block should appear.
    3. Create a block design ("IP Integrator" > "Create Block Design").

        ![Create Block Design](assets/create_block_design.png)

    4. In the diagram:
        - Add ZYNQ7 Processing System (Double click on block > "PS-PL Configuration" > Expand "HP Slave AXI Interface" > Enable "S AXI HP0 interface").
        - Add your custom Voronoi IP.
        - Run "Connection Automation" and "Block Automation" to ensure everything connects properly.
    5. Run "Validate Design" to check for issues. The block diagram should look similar to this:

        ![Block Diagram](assets/block_diagram.png)

    6. Create HDL Wrapper (Sources > Right-click design > "Create HDL Wrapper").

        ![Create HDL Wrapper](assets/hdl_wrapper.png)

    7. Run Generate Bitstream ("Program and Debug" > "Generate Bitstream").

        ![Generate Bitstream](assets/gen_bitstream.png)

3. Export Overlay Files
    1. Once bitstream generation is complete:
        - Go to "File" > "Export" > "Export Bitstream File" and save  `.bit` file to somewhere easily accessible.
        - In File Explorer, go to this directory within the project `...\master-project\master-project.gen\sources_1\bd\Voronoi\hw_handoff` and copy the `.hwh` file found within to same place as the `.bit` file
        - Rename both files so that they have the same name (e.g., "voronoi.bit" and "voronoi.hwh") - this is necessary for the overlay to work correctly.
4. Deploy to PYNQ-Z1
    1. Upload the `.bit` and `.hwh` files to the PYNQ-Z1
    2. In the same folder, create a new Jupyter notebook and copy over the blocks of code found within [voronoi_pynq.ipynb](src/pynq/voronoi_pynq.ipynb).
    3. Make sure the `.bit` filename matches the filename argument in this line of code: `ol = Overlay("xxxxxxxxxx.bit")`
    4. Run the notebook and you should see the resulting execution time and visual verification showing that the algorithm worked properly!

### C/Python Implementations

This section gives a brief overview of the various C/Python implementations. These are organized withi the `c` and `python` sub-directories of the `src` directory and include:

- `voronoi_seq.py`: Sequential Python implementation.

- `voronoi_numba.py`: Parallel Python implementation using Numba for multithreading.

- `voronoi_seq.c`: Sequential C implementation.

- `voronoi_omp.c`: Parallel C implementation using OpenMP for multithreading.

- `voronoi_omp_gpu.c`: Parallel C implementation using OpenMP for GPU offloading (system must support GPU operations).

Each file is extremely similar in structure; memory is allocated for the resulting plane, Euclidean distances are calculated for each coordinate on the plane, and each coordinate is placed into the appropriate Voronoi regions. Each file outputs the resulting execution time and problem size.

To run these implementations, each file's header contains the necessary compilation arguments for manual execution, or they can be run using the provided Makefile (see following section).

### Makefile
A [Makefile](Makefile) is provided to help automate the process of compiling, executing, and benchmarking the various HPC implementations. Specifically, this includes creating the Python virtual environment, compiling the C source files into binaries, executing each, and recording the output with relevent system information in a log file. To do this, simply run `make` from the top-level directory.

If you wish to remove the existing virtual environment, binary files, and logs, run `make clean`. This can be split into separate commands if you do not wish to remove everything all at once.
```sh
make clean_venv
make clean_bin
make clean_logs
make clean_gnu
```

### Graphing
To visualize what the Voronoi algorithm looks like on a 2D plane, the Numba parallelized Python implementation was used in [voronoi_visual.ipynb](src/dev_tools/voronoi_visual.ipynb) along with matplotlib. As an additional visualization method, [voronoi_visual.c](src/dev_tools/voronoi_visual.c) has been included to play around with. This code uses the OpenMP parallelized C implementation combined with GNUplot for visualization, allowing customization of the number of seeds and also randomization of where they are placed on the plane. You can run `make graph` in the top-level directory to compile and run this code. 

> **NOTE:** The GNUplot visualization was mainly included for my personal benefit (broadening my programming skills with C tools/libraries). It requires GNUplot to be installed before running using `sudo apt-get install gnuplot`. GNUplot also takes much longer to run than the Python visualization, which is the recommended method.

To visualize the timing differences between implementations, [timing_comparison.ipynb](src/dev_tools/timing_comparison.ipynb) was created to scrape values from a user-selected log file and display the resulting times in a bar-graph. The Python implementations take much longer than the others to run, making it difficult to visualize the timing differences between the PYNQ and C implementations, so there is an option to exclude the Python implementations from the graph.

> **IMPORTANT:** The recorded time for the HLS w/PYNQ-Z1 needs to be manually updated within [Makefile](Makefile) using the `PYNQ_TIME` variable. Due to the limitations of the PYNQ web server environment, I was unable to find a feasible method of recording the PYNQ execution times within a log file automatically.


<!-- RESULTS -->
## Results

INSERT RESULT ANALYSIS HERE

These times were taken from a sample run during development. The log that includes the shown times and relevant system information can be viewed [here](src/dev_tools/sample.log).

| Method  | Time (sec) |
| ------------- | ------------- |
| HLS w/PYNQ-Z1  | Content Cell  |
| Parallel C w/OpenMP  | Content Cell  |
| Parallel C w/OpenMP   | Content Cell  |
| Sequential C  | Content Cell  |
| Parallel Python w/Numba  | Content Cell  |
| Sequential Python  | Content Cell  |