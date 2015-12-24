openreroc_pwm
=======
[ROS](http://ros.org) is Robot Operating System developed by [OSRF](http://osrfoundation.org/) and open source communities.
This package supports a motor control by PWM using an FPGA board ([ZedBoard](http://zedboard.org/) Xilinx). 
The duty ratio of the mortor can be inputted from openreroc_pwm. 
 
**Git**:         https://github.com/Kumikomi/OpenReroc.git   
**Author**:      Kazushi Yamashina (Utsunomiya University)  
**Copyright**:   2015, Kazushi Yamashina, Utsunomiya University    
**License**:      new BSD License   
**Latest Version**: 0.1.0  

Directry tree
=======
```
openreroc_pwm 
|-include/ 
|-msg/ 
|-hardware
	|-src/
    |-image/
|-src/ 
|-CMakeLists.txt 
|-package.xml  
|-LICENSE.txt
```

Requirements
======

##Platform for ROS system

- [ZedBoard](http://zedboard.org/)
- [xillinux-1.3c](http://xillybus.com/xillinux)
 - Xillinux is used to communicate between FPGA logic and ARM processor. Xillinux is a platform for Zynq that is released by Xillybus Ltd. Linux (Ubuntu) OS runs on the ARM processor. Xillinux can access to FPGA logic through a specific device file.
- ROS (hydro or groovy) please install on xillinux!
- ssh server

##Software

- ISE 14.7 (for hardware synthesis)

How to build software
=======
Please replace **catkin_ws** to your work space name.

```
git clone https://github.com/Kumikomi/OpenReroc.git 
cp -rf OpenReroc/openreroc_pwm ~/catkin_ws/src
cd ~/catkin_ws 
catkin_make 
```

Test Run
======= 
1. [Xillinux installation](http://xillybus.com/downloads/doc/xillybus_getting_started_zynq.pdf)
 - http://xillybus.com/xillinux

2. Hardware bitstream installation
Please replace **xillydemo.bit** on the SD card with `openreroc_pwm/hardware/image/openreroc_pwm.bit`

3. Insert SD card & boot system

4. Run sample nodes 

**terminal 1**
```
cd ~/catkin_ws/
source devel/setup.bash
roscore &
rosrun openreroc_pwm openreroc_pwm
```
**terminal 2**
```
cd ~/catkin_ws/
source devel/setup.bash
rosrun openreroc_pwm sample_input
```

How to build hardware
====== 
It's too complex to describe all the necessary procedure to build hardware, so some hints are shown below.
1 : Place of Soruce code `hardware/src`  
2 : Pin assignment: add the code below to **xillydemo.ucf**  

```verilog
# Pmod JB
NET  dir_out_r  LOC = W12 | IOSTANDARD = LVCMOS33;
NET  en_out_r 	LOC = W11 | IOSTANDARD = LVCMOS33;
NET  dir_out_l LOC = V12 | IOSTANDARD = LVCMOS33;
NET  en_out_l  LOC = W10 | IOSTANDARD = LVCMOS33;
NET sub_port[0] LOC = V10 | IOSTANDARD = LVCMOS33;
NET sub_port[1] LOC = W8 | IOSTANDARD = LVCMOS33;
NET sub_port[2] LOC = V9 | IOSTANDARD = LVCMOS33;
NET sub_port[3] LOC = V8 | IOSTANDARD = LVCMOS33;


NET PS_GPIO[32] LOC = M20  | IOSTANDARD=LVCMOS33;  # "FMC-LA00_CC_N"
NET PS_GPIO[33] LOC = M19  | IOSTANDARD=LVCMOS33;  # "FMC-LA00_CC_P"
NET PS_GPIO[34] LOC = N20  | IOSTANDARD=LVCMOS33;  # "FMC-LA01_CC_N"
NET PS_GPIO[35] LOC = N19  | IOSTANDARD=LVCMOS33;  # "FMC-LA01_CC_P"
NET PS_GPIO[36] LOC = P18  | IOSTANDARD=LVCMOS33;  # "FMC-LA02_N"
NET PS_GPIO[37] LOC = P17  | IOSTANDARD=LVCMOS33;  # "FMC-LA02_P"
NET PS_GPIO[38] LOC = P22  | IOSTANDARD=LVCMOS33;  # "FMC-LA03_N"
NET PS_GPIO[39] LOC = N22  | IOSTANDARD=LVCMOS33;  # "FMC-LA03_P"
```

3 : Add the ports to Top module **xillydemo.v**

```verilog
output dir_out_r,
output dir_out_l,
output en_out_r,
output en_out_l,
input [3:0] sub_port
```

4 : Add the FIFO connection to Top module **xillydemo.v**

```verilog
// 32-bit loopback
//   fifo_32x512 fifo_32
//     (
//      .clk(bus_clk),
//      .srst(!user_w_write_32_open && !user_r_read_32_open),
//      .din(user_w_write_32_data),
//      .wr_en(user_w_write_32_wren),
//      .rd_en(user_r_read_32_rden),
//      .dout(user_r_read_32_data),
//      .full(user_w_write_32_full),
//      .empty(user_r_read_32_empty)
//      );

motor_ctl motor_ctl
(
.clk(bus_clk),
.rst_32(!user_w_write_32_open && !user_r_read_32_open),
.din_32(user_w_write_32_data),
.wr_en_32(user_w_write_32_wren),
.rd_en_32(user_r_read_32_rden),
.dout_32(user_r_read_32_data),
.full_32(user_w_write_32_full),
.empty_32(user_r_read_32_empty),

.dir_out_r(dir_out_r),
.dir_out_l(dir_out_l),
.en_out_r(en_out_r),
.en_out_l(en_out_l)
);
```

