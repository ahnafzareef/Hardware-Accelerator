`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/03/2026 02:28:58 PM
// Design Name: 
// Module Name: layer
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module layer#(

    parameter CHUNK = 64,
    parameter INPUTS_I = 784, //784, 256, 256
    parameter WIDTH_O = 256, //for each layer the output width changes
    localparam CHUNKS = (INPUTS_I + CHUNK -1) / CHUNK

)(

    input logic clk, rst, start,
    input logic [INPUTS_I - 1: 0] input_reg,

    output logic [WIDTH_O - 1: 0] output_reg,
    output logic done
);





endmodule
