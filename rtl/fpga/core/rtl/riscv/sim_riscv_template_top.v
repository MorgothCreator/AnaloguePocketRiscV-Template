`timescale 1ns / 1ps

module sim_riscv_template_top (
	output v_sync,
	output h_sync,
	output video_en,
	output video
);
 
reg rst = 0;
reg clk = 0;
always	#(1000/(74.25 / 2))	clk	<=	~clk;

initial begin
	wait(clk);
	wait(~clk);
	wait(clk);
	wait(~clk);
	wait(clk);
	rst = 1'b1;
	wait(~clk);
	wait(clk);
	wait(~clk);
	wait(clk);
	wait(~clk);
	rst = 1'b0;
	#200000;
	$finish;
end

riscv_template # (
	.PLATFORM("ALTERA"),
	.DEBUG("TRUE"),

	.CORE_CLK_FREQ(74.25),
	.PIXEL_CLOCK(25.00)
	) riscv_template_inst (
	.rst_i(rst),
	.clk_i(clk),
	.clk_pixel_i(clk),
	.pgm_write_clk_i(),
	.pgm_addr_write_i(),
	.pgm_data_write_i(),
	.pgm_write_i(),

	.v_sync_o(v_sync),
	.h_sync_o(h_sync),
	.video_en_o(video_en),
	.video_o(video),

	.buttons_i(),
	.Buzzer1_o(),
	.Buzzer2_o()
	);
endmodule