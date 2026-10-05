`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/04/2026 10:48:07 AM
// Design Name: 
// Module Name: bnn_core
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


module bnn_core (
    input  logic         clk,
    input  logic         rst,
    input  logic         start,
    input  logic [783:0] image,
    output logic [3:0]   digit,
    output logic         done
);

 /*
LAYER DETAILS

1st Layer: 784 into 256 neurons, outputting 256 neurons. Plug THAT into layer2. so a seperate wire.
Layer 2: Same thing 256 neurons so 256 outputs plugged into 10 neurons.
Layer 3: when u get to layer 3 put ARGMAX as 1 in the parameters. Also we need sequential so 3 seperate done wires. and no thresh
*/

logic [255:0] layer1_output, layer2_output;
logic         layer1_done, layer2_done;
 
layer #(.INPUTS_I(784), .WIDTH_O(256),
        .WEIGHT_FILE("layer1_weights.mem"), .THRESH_FILE("layer1_thresh.mem")) L1 (
    .clk(clk), .rst(rst), .start(start), .input_bits(image),
    .output_bits(layer1_output), .digit(), .done(layer1_done));
 
layer #(.INPUTS_I(256), .WIDTH_O(256),
        .WEIGHT_FILE("layer2_weights.mem"), .THRESH_FILE("layer2_thresh.mem")) L2 (
    .clk(clk), .rst(rst), .start(layer1_done), .input_bits(layer1_output),
    .output_bits(layer2_output), .digit(), .done(layer2_done));
 
layer #(.INPUTS_I(256), .WIDTH_O(10), .ARGMAX(1),
        .WEIGHT_FILE("layer3_weights.mem")) L3 (
    .clk(clk), .rst(rst), .start(layer2_done), .input_bits(layer2_output),
    .output_bits(), .digit(digit), .done(done));
 
endmodule
`default_nettype wire