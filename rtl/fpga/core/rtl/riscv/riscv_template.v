/*
 * This IP is the the RISC-V32I instruction set core implementation.
 * 
 * Copyright (C) 2021  Iulian Gheorghiu (morgoth@devboard.tech)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


`undef ADDR_BUS_WIDTH
`define ADDR_BUS_WIDTH			16

`undef ROM_ADDR_WIDTH
`define ROM_ADDR_WIDTH			15

`undef RAM_ADDR_WIDTH
`define RAM_ADDR_WIDTH			12

`undef C_EXTENSION
`define C_EXTENSION	"FALSE"
`undef SYNCHRONOUS_ROM
`define SYNCHRONOUS_ROM "TRUE"
`undef SYNCHRONOUS_RAM
`define SYNCHRONOUS_RAM "TRUE"


module riscv_template # (
	parameter PLATFORM = "XILINX",
	parameter DEBUG = "FALSE",

	parameter CORE_CLK_FREQ = 74.25,
	parameter PIXEL_CLOCK = 25.00
	) (
	input rst_i,
	input clk_i,
	input clk_pixel_i,
	input pgm_write_clk_i,
	input [`ADDR_BUS_WIDTH - 1:0]pgm_addr_write_i,
	input [7:0]pgm_data_write_i,
	input pgm_write_i,

	output v_sync_o,
	output h_sync_o,
	output video_en_o,
	output [7:0]video_o,
	
	inout [3:0]bank_0,
	inout [7:0]bank_1,
	inout [7:0]bank_2,
	inout [7:0]bank_3,
	output [3:0]bank_ctl,
	inout cart_tran_pin30,
	output cart_tran_pin30_dir,
	output cart_pin30_pwroff_reset,
	inout cart_tran_pin31,
	output cart_tran_pin31_dir,

	output [5:0]buttons_i,
	output reg Buzzer1_o,
	output reg Buzzer2_o
	);

wire [`ADDR_BUS_WIDTH - 1:0]data_addr;
wire [31:0]data_out;
wire data_wr_w;
wire data_wr_h;
wire data_wr_b;
wire [31:0]data_in;
wire data_rd_w;
wire data_rd_h;
wire data_rd_b;

wire data_en = data_addr[`ADDR_BUS_WIDTH - 1: `ADDR_BUS_WIDTH - 2] == 2'b01;
wire pgm_en = data_addr[`ADDR_BUS_WIDTH - 1];
wire io_en = ~pgm_en && ~data_en && |data_addr[`ADDR_BUS_WIDTH - 3:8];

wire rom_select;
wire ram_select;

wire int_sig;
wire int_ack;

wire [7:0]video_bus_out;
video #(
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	
	.BASE_ADDR('h100)
) video (
	.rst_i(rst_i),
	.clk_core_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wrb_i(data_wr_b & io_en),
	.bus_i(data_out[7:0]),
	.rdb_i(data_rd_b & io_en),
	.bus_o(video_bus_out),

	.clk_pixel_i(clk_pixel_i),
	.v_sync_o(v_sync_o),
	.h_sync_o(h_sync_o),
	.video_en_o(video_en_o),
	.video_o(video_o)
);
wire [7:0]pio_bank0_bus_out;
wire [7:0]pio_bank0_out;
assign bank_0 = pio_bank0_out;
wire dir_bank0;
atmega_pio # (
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	.USE_CLEAR_SET("TRUE"),
	.BASE_ADDR('h110),
	.PINMASK('hFF),
	.PULLUP_MASK('h0),
	.PULLDN_MASK('h0),
	.INVERSE_MASK('h0),
	.OUT_ENABLED_MASK('hFF),
	.INITIAL_OUTPUT_VALUE('h00),
	.INITIAL_DIR_VALUE('h00)
)pio_bank_0(
	.rst_i(rst_i),
	.clk_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wr_i(data_wr_b & io_en),
	.rd_i(data_rd_b & io_en),
	.bus_i(data_out[7:0]),
	.bus_o(pio_bank0_bus_out),

	.io_i(bank_0),
	.io_o(pio_bank0_out),
	.io_dir_o(dir_bank0),
	.pio_out_io_connect_o()
);

wire [7:0]pio_bank1_bus_out;
wire [7:0]pio_bank1_out;
assign bank_1 = pio_bank1_out;
wire dir_bank1;
atmega_pio # (
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	.USE_CLEAR_SET("TRUE"),
	.BASE_ADDR('h118),
	.PINMASK('hFF),
	.PULLUP_MASK('h0),
	.PULLDN_MASK('h0),
	.INVERSE_MASK('h0),
	.OUT_ENABLED_MASK('hFF),
	.INITIAL_OUTPUT_VALUE('h00),
	.INITIAL_DIR_VALUE('h00)
) pio_bank_1 (
	.rst_i(rst_i),
	.clk_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wr_i(data_wr_b & io_en),
	.rd_i(data_rd_b & io_en),
	.bus_i(data_out[7:0]),
	.bus_o(pio_bank1_bus_out),

	.io_i(bank_1),
	.io_o(pio_bank1_out),
	.io_dir_o(dir_bank1),
	.pio_out_io_connect_o()
);

wire [7:0]pio_bank2_bus_out;
wire [7:0]pio_bank2_out;
assign bank_2 = pio_bank2_out;
wire dir_bank2;
atmega_pio # (
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	.USE_CLEAR_SET("TRUE"),
	.BASE_ADDR('h120),
	.PINMASK('hFF),
	.PULLUP_MASK('h0),
	.PULLDN_MASK('h0),
	.INVERSE_MASK('h0),
	.OUT_ENABLED_MASK('hFF),
	.INITIAL_OUTPUT_VALUE('h00),
	.INITIAL_DIR_VALUE('h00)
) pio_bank_2 (
	.rst_i(rst_i),
	.clk_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wr_i(data_wr_b & io_en),
	.rd_i(data_rd_b & io_en),
	.bus_i(data_out[7:0]),
	.bus_o(pio_bank2_bus_out),

	.io_i(bank_2),
	.io_o(pio_bank2_out),
	.io_dir_o(dir_bank2),
	.pio_out_io_connect_o()
);

wire [7:0]pio_bank3_bus_out;
wire [7:0]pio_bank3_out;
assign bank_3 = pio_bank3_out;
wire dir_bank3;
atmega_pio # (
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	.USE_CLEAR_SET("TRUE"),
	.BASE_ADDR('h128),
	.PINMASK('hFF),
	.PULLUP_MASK('h0),
	.PULLDN_MASK('h0),
	.INVERSE_MASK('h0),
	.OUT_ENABLED_MASK('hFF),
	.INITIAL_OUTPUT_VALUE('h00),
	.INITIAL_DIR_VALUE('h00)
) pio_bank_3 (
	.rst_i(rst_i),
	.clk_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wr_i(data_wr_b & io_en),
	.rd_i(data_rd_b & io_en),
	.bus_i(data_out[7:0]),
	.bus_o(pio_bank3_bus_out),

	.io_i(bank_3),
	.io_o(pio_bank3_out),
	.io_dir_o(dir_bank3),
	.pio_out_io_connect_o()
);

assign bank_ctl = {dir_bank3, dir_bank2, dir_bank1, dir_bank0};

wire [7:0]pio_cart_tran_pin_bus_out;
wire [7:0]pio_cart_tran_pin_out;
assign {cart_tran_pin31, cart_tran_pin30} = pio_cart_tran_pin_out[7:6];
wire [7:0]cart_tran_pin_dir;
assign {cart_tran_pin31_dir, cart_tran_pin30_dir} = cart_tran_pin_dir[7:6];
atmega_pio # (
	.PLATFORM(PLATFORM),
	.BUS_ADDR_DATA_LEN(`ADDR_BUS_WIDTH - 3),
	.PORT_WIDTH(8),
	.USE_CLEAR_SET("TRUE"),
	.BASE_ADDR('h130),
	.PINMASK('hC0),
	.PULLUP_MASK('h0),
	.PULLDN_MASK('h0),
	.INVERSE_MASK('h0),
	.OUT_ENABLED_MASK('hC0),
	.INITIAL_OUTPUT_VALUE('h00),
	.INITIAL_DIR_VALUE('h00)
) pio_tran_pin (
	.rst_i(rst_i),
	.clk_i(clk_i),

	.addr_i(data_addr[`ADDR_BUS_WIDTH - 3:0]),
	.wr_i(data_wr_b & io_en),
	.rd_i(data_rd_b & io_en),
	.bus_i(data_out[7:0]),
	.bus_o(pio_cart_tran_pin_bus_out),

	.io_i({cart_tran_pin31, cart_tran_pin30, 6'h00}),
	.io_o(pio_cart_tran_pin_out),
	.io_dir_o(cart_tran_pin_dir),
	.pio_out_io_connect_o()
);

rtc #(
	.PERIOD_STATIC(74250),
	.CNT_SIZE(17)
)rtc_inst(
	.rst_i(rst_i),
	.clk_i(clk_i),
	.clk_cnt_i(clk_i),
	.top_i(),
	.int_o(int_sig),
	.int_ack_i(int_ack)
);

wire [31:0]pgm_data;
wire [`ADDR_BUS_WIDTH - 1:0]pgm_addr;
wire [31:0]rom_data_bus_out;
rom_dp # (
	.PLATFORM(PLATFORM),
	.ADDR_BUS_LEN(`ROM_ADDR_WIDTH),
	.EXTENSION_C(`C_EXTENSION),
	.ROM_PATH(DEBUG == "TRUE" ? "/home/morgoth/Documents/GitHub/AnaloguePocketRiscV-Template/rtl/fpga/core/rtl/riscv/core1ROM_LightTest" : ""),
	.SYNCHRONOUS_OUTPUT(`SYNCHRONOUS_ROM)
)rom_dp_inst(
	.wr_clk(pgm_write_clk_i),
	.addr_wr(pgm_addr_write_i),
	.data_wr(pgm_data_write_i),
	.wr(pgm_write_i),
	.clk(clk_i),
	.addr_p1(pgm_addr[`ROM_ADDR_WIDTH - 1 : 0]),
	.cs_p1(1'b1),
	.out_p1(pgm_data),
	.data_p1_rdy(),
	.addr_p2(data_addr[`ROM_ADDR_WIDTH - 1 : 0]),
	.cs_p2(pgm_en),
	.out_p2(rom_data_bus_out),
	.data_p2_rdy(rom_select)
);
 
wire [31:0]ram_data_bus_out;
ram # (
	.PLATFORM(PLATFORM),
	.ADDR_BUS_LEN(`RAM_ADDR_WIDTH),
	.SYNCHRONOUS_OUTPUT(`SYNCHRONOUS_RAM)
)ram_inst(
	.clk(clk_i),
	.addr(data_addr[`RAM_ADDR_WIDTH - 1: 0]),
	.cs(data_en),
	.out(ram_data_bus_out),
	.data_rdy(ram_select),
	.in(data_out),
	.write_w(data_wr_w),
	.write_h(data_wr_h),
	.write_b(data_wr_b)
);

wire [7:0]data_bus_out;
reg [7:0]data_bus_latch;
reg data_bus_select_latch;
always @(posedge clk_i) data_bus_latch <= data_bus_out;
always @(posedge clk_i) data_bus_select_latch <= data_rd_b & io_en;
// IO's are designed to asynchronously read data, we need a latch.
io_bus_dmux #(
	.NR_OF_BUSSES_IN(6),
	.BITS_PER_BUS(8),
	.USE_OR_METHOD("TRUE")
) io_bus_dmux_inst(
	.bus_in({
		pio_cart_tran_pin_bus_out,
		pio_bank3_bus_out,
		pio_bank2_bus_out,
		pio_bank1_bus_out,
		pio_bank0_bus_out,
		video_bus_out
	}),
	.bus_sel(6'hFF),
	.bus_out(data_bus_out)
);

io_bus_dmux #(
	.NR_OF_BUSSES_IN(3),
	.BITS_PER_BUS(32),
	.USE_OR_METHOD("TRUE")
) data_bus_dmux_inst(
	.bus_in({ 
		{24'h0, data_bus_latch},
		ram_data_bus_out,
		rom_data_bus_out
	}),
	.bus_sel({
		data_bus_select_latch,
		ram_select,
		rom_select
	}),
	.bus_out(data_in)
);
 
risc_v # (
	.DEBUG(DEBUG),
	.PLATFORM(PLATFORM),
	.SINGLE_BUS("FALSE"),
	.READ_SIG_ENABLED("TRUE"),
	.DATA_BUS_ALWAYS_CONNECTED("FALSE"),
	.ADDR_BUS_WIDTH(`ADDR_BUS_WIDTH),
	.RESET_VECTOR(16'h8000),
	.EXTENSION_M("FALSE"),
	.EXTENSION_MDIV("FALSE"),
	.EXTENSION_C(`C_EXTENSION),
	.WATCHDOG_CNT_WIDTH(0),
	.VECTOR_INT_TABLE_SIZE(1),
	.STALL_INPUT_LINES(0)
)risc_v_inst(
	.rst_i(rst_i),
	.clk_i(clk_i),
	.core_stall_i(),
	.pgm_addr_o(pgm_addr),
	.pgm_inst_i(pgm_data),
	.data_addr_o(data_addr),
	.data_o(data_out),
	.data_wr_w_o(data_wr_w),
	.data_wr_h_o(data_wr_h),
	.data_wr_b_o(data_wr_b),
	.data_wr_ack_i(),
	.data_i(data_in),
	.data_rd_w_o(data_rd_w),
	.data_rd_h_o(data_rd_h),
	.data_rd_b_o(data_rd_b),
	.data_rd_valid_i(),
	.int_sig_i(int_sig),
	.int_ack_o(int_ack),
	.int_en_o()
);

endmodule