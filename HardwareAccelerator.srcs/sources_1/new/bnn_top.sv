`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/04/2026 08:25:11 AM
// Design Name: 
// Module Name: bnn_top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments: get 100mhz clock and configure in xdc. configure Io buttons and leds for testing
// 
//////////////////////////////////////////////////////////////////////////////////

//CLK100MHZ, btn[any] for reset and led[3:0] for showing me the number 0-9

module bnn_top(
    input logic clk, btn, //need 100 MHZ, 12 is for DDR
    output logic [3:0] led //0-9
);

//Testing Image
logic [783:0] image [0:0];
initial $readmemb("test_image.mem", image);


//Double flopping, 2 bits is 2 FF. Acutla value is sampled then put in next cycle, so 2 clk cycles
logic [1:0] rst_s = 2'b0;
always_ff @(posedge clk)
    rst_s <= {rst_s[0], btn};
logic reset;
assign reset = rst_s[1];

logic start_running = 1'b0;
always_ff @(posedge clk)
    start_running <= !reset;
logic start;

assign start = !start_running && !reset;


/*
LAYER DETAILS

1st Layer: 784 into 256 neurons, outputting 256 neurons. Plug THAT into layer2. so a seperate wire.
Layer 2: Same thing 256 neurons so 256 outputs plugged into 10 neurons.
Layer 3: when u get to layer 3 put ARGMAX as 1 in the parameters. Also we need sequential so 3 seperate done wires. and no thresh
*/

//Layer Stuff
logic [255:0] layer1_output, layer2_output;
logic layer1_done, layer2_done, layer3_done;
logic [3:0] digit;

layer #(.INPUTS_I(784), .WIDTH_O(256), .WEIGHT_FILE("layer1_weights.mem"), .THRESH_FILE("layer1_thresh.mem")) L1 (
    .clk(clk), .rst(reset), .start(start), .input_bits(image[0]), .output_bits(layer1_output), .digit(), .done(layer1_done));

layer #(.INPUTS_I(256), .WIDTH_O(256), .WEIGHT_FILE("layer2_weights.mem"), .THRESH_FILE("layer2_thresh.mem")) L2 (
    .clk(clk), .rst(reset), .start(layer1_done), .input_bits(layer1_output), .output_bits(layer2_output), .digit(), .done(layer2_done));

layer #(.INPUTS_I(256), .WIDTH_O(10), .WEIGHT_FILE("layer3_weights.mem"),.ARGMAX(1)) L3 (
    .clk(clk), .rst(reset), .start(layer2_done), .input_bits(layer2_output), .output_bits(), .digit(digit), .done(layer3_done));   



assign led = digit;
endmodule
