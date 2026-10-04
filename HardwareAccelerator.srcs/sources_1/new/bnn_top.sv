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

bnn_core core (
    .clk(clk), .rst(reset), .start(start),
    .image(image[0]), .digit(led), .done()
);

endmodule
