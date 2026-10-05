//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//Copyright 2022-2026 Advanced Micro Devices, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2026.1 (win64) Build 6511674 Tue Jun 16 11:02:23 MDT 2026
//Date        : Sun Oct  4 19:40:43 2026
//Host        : LAPTOP-E4LD1OHV running 64-bit major release  (build 9200)
//Command     : generate_target driverSystem_wrapper.bd
//Design      : driverSystem_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module driverSystem_wrapper
   (io10_irq,
    io1_rst,
    io2_dc,
    io3_mosi,
    io4_sck,
    io7_cs,
    io8_miso,
    io9_tcs,
    reset,
    sys_clock,
    usb_uart_rxd,
    usb_uart_txd);
  input io10_irq;
  output [0:0]io1_rst;
  output [0:0]io2_dc;
  output io3_mosi;
  output io4_sck;
  output [0:0]io7_cs;
  input io8_miso;
  output [0:0]io9_tcs;
  input reset;
  input sys_clock;
  input usb_uart_rxd;
  output usb_uart_txd;

  wire io10_irq;
  wire [0:0]io1_rst;
  wire [0:0]io2_dc;
  wire io3_mosi;
  wire io4_sck;
  wire [0:0]io7_cs;
  wire io8_miso;
  wire [0:0]io9_tcs;
  wire reset;
  wire sys_clock;
  wire usb_uart_rxd;
  wire usb_uart_txd;

  driverSystem driverSystem_i
       (.io10_irq(io10_irq),
        .io1_rst(io1_rst),
        .io2_dc(io2_dc),
        .io3_mosi(io3_mosi),
        .io4_sck(io4_sck),
        .io7_cs(io7_cs),
        .io8_miso(io8_miso),
        .io9_tcs(io9_tcs),
        .reset(reset),
        .sys_clock(sys_clock),
        .usb_uart_rxd(usb_uart_rxd),
        .usb_uart_txd(usb_uart_txd));
endmodule
