# FPGA_FINAL_PROJECT

This project implements a simplified FPGA placement legalization tool. Given an FPGA architecture and an initial placement of circuit instances, it assigns each instance to a valid hardware resource (CLB, RAM, or DSP) without conflicts.

The tool uses a BFS-based local search strategy to find the nearest available resource for each instance, ensuring no overlaps while preserving spatial locality. Instances are processed in a deterministic order to maintain consistent results.

This project demonstrates a core component of the FPGA CAD flow—placement legalization—and provides a foundation for future extensions such as wirelength optimization, timing-driven placement, and global assignment algorithms.

1.use make command to compile legalizer.cpp

2.All testcases are in the input directory
